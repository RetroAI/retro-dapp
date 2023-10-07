/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#include "stream_decoder.h"

#include "utils/emscripten_utils.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

#include <emscripten/val.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
}

StreamDecoder::StreamDecoder(const std::string& fileName, int blockSize, int maxDecodedSize)
  : m_fileName(fileName),
    m_blockSize(std::max(blockSize, 0)),
    m_maxDecodedSize(std::max(maxDecodedSize, 0))
{
}

StreamDecoder::~StreamDecoder()
{
  Deinitialize();
}

bool StreamDecoder::Initialize()
{
  // Preallocate an AVFormatContext to use our custom read function instead of
  // the avformat internal I/O layer
  m_formatContext = avformat_alloc_context();
  if (m_formatContext == nullptr)
  {
    std::cerr << "Failed to allocate AVFormatContext" << std::endl;
    return false;
  }

  // Use nonblocking reads if possible
  m_formatContext->flags |= AVFMT_FLAG_NONBLOCK;

  // The buffer size is very important for performance. For protocols with
  // fixed blocksize it should be set to this blocksize.
  unsigned int bufferSize = 32768; // From IO_BUFFER_SIZE in aviobuf.c
  if (m_blockSize > 1)
    bufferSize = m_blockSize;

  uint8_t* avioContextBuffer = static_cast<uint8_t*>(av_malloc(bufferSize));
  if (avioContextBuffer == nullptr)
  {
    std::cerr << "Failed to allocate buffer for I/O" << std::endl;
    return false;
  }

  // Initialize the I/O context
  m_ioContext = avio_alloc_context(avioContextBuffer, bufferSize, 0, this, ReadPacketInternal,
                                   nullptr, SeekInternal);
  if (m_ioContext == nullptr)
  {
    std::cerr << "Failed to allocate AVIOContext" << std::endl;
    return false;
  }

  if (m_blockSize > 1)
    m_ioContext->max_packet_size = bufferSize;

  // Set the pb field of the AVFormatContext to the newly created AVIOContext
  m_formatContext->pb = m_ioContext;

  int result = avformat_open_input(&m_formatContext, m_fileName.c_str(), nullptr, nullptr);
  if (result < 0)
  {
    std::cerr << "Failed to open input: " << av_err2str(result) << std::endl;
    return false;
  }

  const int streamInfoResult = avformat_find_stream_info(m_formatContext, nullptr);

  // Uncomment to dump video metadata
  std::cout << "=== Detecting input video" << std::endl;
  av_dump_format(m_formatContext, 0, m_fileName.c_str(), 0);

  for (int i = 0; i < m_formatContext->nb_streams; i++)
  {
    auto type = m_formatContext->streams[i]->codec->codec_type;
    if (m_videoStreamId < 0 && type == AVMEDIA_TYPE_VIDEO)
    {
      m_videoStreamId = i;
      break;
    }
  }

  if (m_videoStreamId < 0)
  {
    std::cerr << "No audio/video stream found" << std::endl;
    return false;
  }

  // Get stream parameters
  AVStream* videoStream = m_formatContext->streams[m_videoStreamId];
  m_timeBaseNum = videoStream->time_base.num;
  m_timeBaseDen = videoStream->time_base.den;
  m_realFrameRateNum = videoStream->r_frame_rate.num;
  m_realFrameRateDen = videoStream->r_frame_rate.den;

  // Get codec parameters
  AVCodecParameters* codecParams = videoStream->codecpar;
  m_videoBitRate = videoStream->codecpar->bit_rate;

  auto codec = avcodec_find_decoder(codecParams->codec_id);
  if (codec == nullptr)
  {
    std::cerr << "Failed avcodec_find_decoder(" << codecParams->codec_id << ")" << std::endl;
    return false;
  }

  m_videoCodecContext = avcodec_alloc_context3(codec);
  if (m_videoCodecContext == nullptr)
  {
    std::cerr << "Failed avcodec_alloc_context3()" << std::endl;
    return false;
  }

  result = avcodec_parameters_to_context(m_videoCodecContext, codecParams);
  if (result != 0)
  {
    std::cerr << "Failed avcodec_parameters_to_context(): " << av_err2str(result) << std::endl;
    return false;
  }

  result = avcodec_open2(m_videoCodecContext, codec, nullptr);
  if (result < 0)
  {
    std::cerr << "Failed avcodec_open2(): " << av_err2str(result) << std::endl;
    return false;
  }

  if (m_videoCodecContext->width <= 0 || m_videoCodecContext->height <= 0)
  {
    std::cerr << "Video has invalid dimensions: " << m_videoCodecContext->width << " x "
              << m_videoCodecContext->height << std::endl;
    return false;
  }

  m_width = static_cast<unsigned int>(m_videoCodecContext->width);
  m_height = static_cast<unsigned int>(m_videoCodecContext->height);

  // Reduce destination size to fit max constraint
  if (m_maxDecodedSize > 0)
  {
    while (std::max(m_width, m_height) > m_maxDecodedSize)
    {
      m_width /= 2;
      m_height /= 2;
    }
  }

  if (m_width == 0 || m_height == 0)
  {
    std::cerr << "Invalid dimensions (width = " << m_width << ", height = " << m_height
              << ", max size = " << m_maxDecodedSize << ")" << std::endl;
    return false;
  }

  m_frameSize = av_image_get_buffer_size(m_targetFormat, m_width, m_height, 1);

  m_decodedFrame = av_frame_alloc();
  if (m_decodedFrame == nullptr)
  {
    std::cerr << "Failed to alloc m_decodedFrame" << std::endl;
    return false;
  }

  m_videoBuffer = static_cast<uint8_t*>(av_malloc(m_frameSize));
  if (m_videoBuffer == nullptr)
  {
    std::cerr << "Failed to allocate video buffer" << std::endl;
    return false;
  }

  return true;
}

void StreamDecoder::Deinitialize()
{
  if (m_videoBuffer != nullptr)
  {
    av_free(m_videoBuffer);
    m_videoBuffer = nullptr;
  }

  if (m_decodedFrame != nullptr)
  {
    av_free(m_decodedFrame);
    m_decodedFrame = nullptr;
  }

  if (m_scaler != nullptr)
  {
    sws_freeContext(m_scaler);
    m_scaler = nullptr;
  }

  if (m_videoCodecContext != nullptr)
  {
    avcodec_close(m_videoCodecContext);
    avcodec_free_context(&m_videoCodecContext);
    m_videoCodecContext = nullptr;
  }

  if (m_formatContext != nullptr)
  {
    avformat_close_input(&m_formatContext);
    m_formatContext = nullptr;
  }

  if (m_ioContext != nullptr)
  {
    av_free(m_ioContext->buffer);
    av_free(m_ioContext);
    m_ioContext = nullptr;
  }
}

bool StreamDecoder::OpenVideo()
{
  if (m_state == StreamDecoderState::Init)
  {
    if (Initialize())
      m_state = StreamDecoderState::Running;
    else
      m_state = StreamDecoderState::Failed;
  }

  return (m_state != StreamDecoderState::Failed);
}

void StreamDecoder::AddPacket(const emscripten::val& packet)
{
  const unsigned int dataSize = EmscriptenUtils::ArrayLength(packet);

  std::vector<uint8_t> data(dataSize);

  EmscriptenUtils::GetArrayData(packet, data.data(), dataSize);

  m_packets.emplace_back(std::move(data));
  m_totalSize += dataSize;
}

void StreamDecoder::Decode()
{
  if (m_state == StreamDecoderState::Failed)
    return;

  AVPacket packet{};

  int result = av_read_frame(m_formatContext, &packet);
  if (result == AVERROR_EOF)
  {
    m_state = StreamDecoderState::Ended;
    return;
  }
  else if (result < 0)
  {
    std::cerr << "Error reading frame: " << av_err2str(result) << std::endl;
    m_state = StreamDecoderState::Failed;
    return;
  }

  if (packet.stream_index == m_videoStreamId)
  {
    if (m_scaler == nullptr)
    {
      m_scaler = sws_getContext(m_videoCodecContext->width, m_videoCodecContext->height,
                                m_videoCodecContext->pix_fmt, m_width, m_height, m_targetFormat,
                                SWS_BILINEAR, nullptr, nullptr, nullptr);
    }

    if (m_scaler == nullptr)
    {
      std::cerr << "Failed to create software scaler" << std::endl;
      m_state = StreamDecoderState::Failed;
      return;
    }

    result = avcodec_send_packet(m_videoCodecContext, &packet);
    if (result < 0)
    {
      std::cerr << "Error sending packet: " << av_err2str(result) << std::endl;
      m_state = StreamDecoderState::Failed;
      return;
    }

    result = avcodec_receive_frame(m_videoCodecContext, m_decodedFrame);
    if (result < 0 && result != AVERROR(EAGAIN))
    {
      std::cerr << "Error receiving frame: " << av_err2str(result) << std::endl;
      m_state = StreamDecoderState::Failed;
      return;
    }

    if (result == 0)
    {
      if (m_decodedFrame->pts < 0)
      {
        std::cerr << "Invalid pts: " << m_decodedFrame->pts << std::endl;
        m_state = StreamDecoderState::Failed;
        return;
      }

      const int videoLineSize = av_image_get_buffer_size(m_targetFormat, m_width, 1, 1);

      uint8_t* const dest[] = {m_videoBuffer};
      const int destLineSize[] = {videoLineSize};

      result = sws_scale(m_scaler, m_decodedFrame->data, m_decodedFrame->linesize, 0,
                         m_videoCodecContext->height, dest, destLineSize);

      if (result < 0)
      {
        std::cerr << "Error scaling frame: " << av_err2str(result) << std::endl;
        m_state = StreamDecoderState::Failed;
        return;
      }

      m_videoPts = static_cast<uint64_t>(m_decodedFrame->pts);

      m_state = StreamDecoderState::HasFrame;
    }
  }

  av_packet_unref(&packet);
}

uintptr_t StreamDecoder::GetFrameBuffer()
{
  // Reset state
  m_state = StreamDecoderState::Running;

  return reinterpret_cast<uintptr_t>(m_videoBuffer);
}

unsigned int StreamDecoder::ReadPacket(uint8_t* buffer, unsigned int bufferSize)
{
  if (m_packets.empty())
  {
    std::cerr << "No data to read" << std::endl;
    return 0;
  }

  unsigned int position = m_readPosition;

  for (auto it = m_packets.begin(); it != m_packets.end(); ++it)
  {
    const std::vector<uint8_t>& packet = *it;

    if (position < packet.size())
    {
      const unsigned int remainingPacketSize = packet.size() - position;
      const unsigned int copySize = std::min(bufferSize, remainingPacketSize);

      if (copySize > 0)
      {
        std::memcpy(static_cast<void*>(buffer), static_cast<const void*>(packet.data() + position),
                    copySize);
        m_readPosition += copySize;
      }

      return copySize;
    }

    position -= packet.size();
  }

  return 0;
}

uint64_t StreamDecoder::Seek(uint64_t offset)
{
  if (offset > m_totalSize)
    return -1;

  m_readPosition = offset;

  return m_readPosition;
}

int StreamDecoder::ReadPacketInternal(void* context, uint8_t* buffer, int bufferSize)
{
  StreamDecoder* decoder = static_cast<StreamDecoder*>(context);
  if (decoder == nullptr || bufferSize <= 0)
    return AVERROR_EXIT;

  const unsigned int length = decoder->ReadPacket(buffer, bufferSize);
  if (length == 0)
    return AVERROR(EAGAIN);

  return static_cast<int>(length);
}

int64_t StreamDecoder::SeekInternal(void* context, int64_t offset, int whence)
{
  StreamDecoder* decoder = static_cast<StreamDecoder*>(context);
  if (decoder == nullptr || offset < 0)
    return AVERROR_EXIT;

  if ((whence & AVSEEK_SIZE) != 0)
    return decoder->GetTotalSize();

  switch (whence)
  {
    // Seek from beginning of file
    case SEEK_SET:
      return decoder->Seek(offset);

    // Seek from current position
    case SEEK_CUR:
      return decoder->Seek(decoder->GetReadPosition() + offset);

    // Seek from end of file
    case SEEK_END:
      return decoder->Seek(decoder->GetTotalSize() + offset);

    default:
      break;
  }

  return -1;
}
