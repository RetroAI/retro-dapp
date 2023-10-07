/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#include "stream_encoder.hpp"

#include "utils/emscripten_utils.hpp"
#include "video/video_frame.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

#include <emscripten/val.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavcodec/codec_par.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>
#include <libavutil/mathematics.h>
#include <libavutil/mem.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

const AVPixelFormat StreamEncoder::m_inputFormat = AV_PIX_FMT_RGBA;
const AVPixelFormat StreamEncoder::m_targetFormat = AV_PIX_FMT_YUV420P;
const unsigned int StreamEncoder::m_cachePageSize = 32768; // From IO_BUFFER_SIZE in aviobuf.c

StreamEncoder::StreamEncoder(const std::string& fileName,
                             int width,
                             int height,
                             int timeBaseNum,
                             int timeBaseDen,
                             int realFrameRateNum,
                             int realFrameRateDen,
                             int videoBitRate,
                             int writeBufferSize,
                             int writePacketFnPtr,
                             int seekFnPtr)
  : m_fileName(fileName),
    m_width(std::max(width, 0)),
    m_height(std::max(height, 0)),
    m_timeBaseNum(std::max(timeBaseNum, 0)),
    m_timeBaseDen(std::max(timeBaseDen, 0)),
    m_realFrameRateNum(std::max(realFrameRateNum, 0)),
    m_realFrameRateDen(std::max(realFrameRateDen, 0)),
    m_videoBitRate(std::max(videoBitRate, 0)),
    m_writeBufferSize(writeBufferSize > 0 ? writeBufferSize : m_cachePageSize),
    m_targetFrame(std::make_unique<VideoFrame>(width, height, m_targetFormat)),
    m_writePacketFnPtr(reinterpret_cast<WritePacketFnPtr>(writePacketFnPtr)),
    m_seekFnPtr(reinterpret_cast<SeekFnPtr>(seekFnPtr))
{
  // Enable libav debugging
  av_log_set_level(AV_LOG_DEBUG);
}

StreamEncoder::~StreamEncoder()
{
  Deinitialize();
}

bool StreamEncoder::Initialize()
{
  // Validate state
  if (m_width == 0 || m_height == 0)
  {
    std::cerr << "Invalid dimensions (width = " << m_width << ", height = " << m_height << ")"
              << std::endl;
    return false;
  }

  if (m_timeBaseNum == 0 || m_timeBaseDen == 0)
  {
    std::cerr << "Invalid time base (numerator = " << m_timeBaseNum
              << ", denominator = " << m_timeBaseDen << ")" << std::endl;
    return false;
  }

  if (m_realFrameRateNum == 0 || m_realFrameRateDen == 0)
  {
    std::cerr << "Invalid frame rate (numerator = " << m_realFrameRateNum
              << ", denominator = " << m_realFrameRateDen << ")" << std::endl;
    return false;
  }

  if (m_writePacketFnPtr == nullptr || m_seekFnPtr == nullptr)
  {
    std::cerr << "Invalid function pointers passed to constructor" << std::endl;
    return false;
  }

  if (!CreateContext())
    return false;

  if (!PrepareVideoEncoder())
    return false;

  if (!InitializeVideoStream())
    return false;

  EnableGlobalHeaders();

  // Dump av format information
  PrintInfo();

  if (!WriteHeader())
    return false;

  // Initialize buffer for YUV420p frame data
  if (!m_targetFrame->Initialize())
    return false;

  return true;
}

bool StreamEncoder::CreateContext()
{
  // Guess encoding format
  AVOutputFormat* outputFormat = av_guess_format(nullptr, m_fileName.c_str(), nullptr);
  if (outputFormat == nullptr)
  {
    std::cerr << "Unknown file extension: " << m_fileName << std::endl;
    return false;
  }

  // Allocate an AVFormatContext to use our custom write function instead of
  // the avformat internal I/O layer
  int result =
      avformat_alloc_output_context2(&m_formatContext, outputFormat, nullptr, m_fileName.c_str());
  if (result < 0)
  {
    std::cerr << "Failed to allocate AVFormatContext: " << av_err2str(result) << std::endl;
    return false;
  }

  // Create a write buffer for our custom I/O context
  uint8_t* avioContextBuffer = static_cast<uint8_t*>(av_malloc(m_writeBufferSize));
  if (avioContextBuffer == nullptr)
  {
    std::cerr << "Failed to allocate buffer for I/O" << std::endl;
    return false;
  }

  // Initialize the I/O context
  m_ioContext = avio_alloc_context(avioContextBuffer, m_writeBufferSize, 1, this, nullptr,
                                   WritePacketInternal, SeekInternal);
  if (m_ioContext == nullptr)
  {
    std::cerr << "Failed to allocate AVIOContext" << std::endl;
    return false;
  }

  // Set the pb field of the AVFormatContext to the newly created AVIOContext
  m_formatContext->pb = m_ioContext;

  return true;
}

bool StreamEncoder::PrepareVideoEncoder()
{
  // It's necessary to open stream codec to link it to "codec" (the encoder)
  const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
  if (codec == nullptr)
  {
    std::cerr << "Error: Can't find H.264 encoder" << std::endl;
    return false;
  }

  m_codecContext = avcodec_alloc_context3(codec);
  if (m_codecContext == nullptr)
  {
    std::cerr << "Error: Can't allocate codec context" << std::endl;
    return false;
  }

  // Fill in codec context
  m_codecContext->bit_rate = m_videoBitRate;
  m_codecContext->pix_fmt = m_targetFormat;
  m_codecContext->width = m_width;
  m_codecContext->height = m_height;
  m_codecContext->time_base =
      AVRational{static_cast<int>(m_realFrameRateDen), static_cast<int>(m_realFrameRateNum)};

  // Prefer reasonable speed over quality
  av_opt_set(m_codecContext->priv_data, "preset", "fast", 0);

  // TODO: Set Constant Rate Factor (CRF) quality if bit_rate is zero. In this
  // case, expose CRF as an initialization parameter to the user
  //av_opt_set_int(m_codecContext->priv_data, "crf", 23, AV_OPT_SEARCH_CHILDREN);

  // H.264 specific options
  /* TODO
  m_codecContext->gop_size = 25; // Emit one intraframe every 25 frames
  m_codecContext->level = 31;
  */

  int result = avcodec_open2(m_codecContext, codec, nullptr);
  if (result < 0)
  {
    std::cerr << "Error opening codec: " << av_err2str(result) << std::endl;
    return false;
  }

  return true;
}

bool StreamEncoder::InitializeVideoStream()
{
  m_videoStream = avformat_new_stream(m_formatContext, nullptr);
  if (m_videoStream == nullptr)
  {
    std::cerr << "Error: Can't create new stream" << std::endl;
    return false;
  }

  m_videoStream->id = m_formatContext->nb_streams - 1;

  // Copy the stream parameters to the muxer
  int result = avcodec_parameters_from_context(m_videoStream->codecpar, m_codecContext);
  if (result < 0)
  {
    std::cerr << "Error setting parameters: " << av_err2str(result) << std::endl;
    return false;
  }

  // Set stream time base as a hint to the muxer before writing the header
  m_videoStream->time_base =
      AVRational{static_cast<int>(m_timeBaseNum), static_cast<int>(m_timeBaseDen)};
  m_videoStream->r_frame_rate =
      AVRational{static_cast<int>(m_realFrameRateNum), static_cast<int>(m_realFrameRateDen)};

  return true;
}

void StreamEncoder::EnableGlobalHeaders()
{
  // Set the flag AV_CODEC_FLAG_GLOBAL_HEADER which tells the encoder that it
  // can use the global headers
  if (m_formatContext->oformat->flags & AVFMT_GLOBALHEADER)
    m_formatContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void StreamEncoder::PrintInfo()
{
  std::cout << "=== Detecting output video" << std::endl;
  av_dump_format(m_formatContext, 0, m_fileName.c_str(), 1);
}

bool StreamEncoder::WriteHeader()
{
  // Set format's private options, to be passed to avformat_write_header()
  AVDictionary* formatOptions = nullptr;

  // Set faststart (requires a seekable write stream)
  // TODO: This requires re-opening the file for reading
  /*
  int result = av_dict_set(&formatOptions, "movflags", "faststart", 0);
  if (result < 0)
  {
    std::cerr << "Error setting option: " << av_err2str(result) << std::endl;
    return false;
  }
  */

  // Write file header if necessary
  int result = avformat_write_header(m_formatContext, &formatOptions);

  if (formatOptions != nullptr)
    av_dict_free(&formatOptions);

  if (result < 0)
  {
    std::cerr << "Error writing header: " << av_err2str(result) << std::endl;
    return false;
  }

  return true;
}

void StreamEncoder::Deinitialize()
{
  if (m_formatContext != nullptr)
  {
    avformat_free_context(m_formatContext);
    m_formatContext = nullptr;
  }

  if (m_ioContext != nullptr)
  {
    av_free(m_ioContext->buffer);
    av_free(m_ioContext);
    m_ioContext = nullptr;
  }

  if (m_inputFrameBuffer != nullptr)
  {
    av_free(m_inputFrameBuffer);
    m_inputFrameBuffer = nullptr;
  }
}

bool StreamEncoder::OpenVideo()
{
  Deinitialize();

  if (Initialize())
  {
    m_state = StreamEncoderState::Running;
    return true;
  }
  else
  {
    m_state = StreamEncoderState::Failed;
    return false;
  }
}

bool StreamEncoder::AddFrame(const emscripten::val& frameData, int framePts)
{
  // Validate state
  if (m_state != StreamEncoderState::Running)
  {
    std::cerr << "Failed to add frame: Not running" << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  const size_t dataSize = EmscriptenUtils::ArrayLength(frameData);

  // Allocate buffer for input frame, if necessary
  if (m_inputFrameSize == 0)
  {
    m_inputFrameSize = static_cast<size_t>(dataSize);
    m_inputFrameBuffer = static_cast<uint8_t*>(av_malloc(dataSize));
  }
  else
  {
    // Verify size hasn't changed
    if (m_inputFrameSize != dataSize)
    {
      std::cerr << "Invalid data size: " << dataSize << " (expected " << m_inputFrameSize << ")"
                << std::endl;
      m_state = StreamEncoderState::Failed;
      return false;
    }
  }

  // Copy data
  EmscriptenUtils::GetArrayData(frameData, m_inputFrameBuffer, m_inputFrameSize);

  // Create scaler, if necessary
  if (m_scaler == nullptr)
  {
    m_scaler = sws_getContext(m_width, m_height, m_inputFormat, m_width, m_height, m_targetFormat,
                              SWS_BILINEAR, nullptr, nullptr, nullptr);
  }

  if (m_scaler == nullptr)
  {
    std::cerr << "Failed to create software scaler" << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  uint8_t* const sourceData[] = {m_inputFrameBuffer};
  const int sourceLineSize[] = {static_cast<int>(m_inputFrameSize / m_height)};

  // Scale pixels from RGBA to YUV420p
  int result =
      sws_scale(m_scaler, sourceData, sourceLineSize, 0, m_height,
                m_targetFrame->FrameHandle()->data, m_targetFrame->FrameHandle()->linesize);

  if (result < 0)
  {
    std::cerr << "Error scaling frame: " << av_err2str(result) << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  //
  // Rescale the presentation timestamp to fix bit rate estimation. We need to
  // rescale the pts from the stream's time_base to the codec time_base to get
  // a simple frame number (e.g. 1, 2, 3).
  //
  // See https://stackoverflow.com/questions/11466184/setting-video-bit-rate-through-ffmpeg-api-is-ignored-for-libx264-codec
  //
  m_targetFrame->FrameHandle()->pts = av_rescale_q(
      static_cast<int64_t>(framePts), m_videoStream->time_base, m_codecContext->time_base);

  // Encode the frame
  result = avcodec_send_frame(m_codecContext, m_targetFrame->FrameHandle());
  if (result < 0)
  {
    std::cerr << "Failed to encode frame: " << av_err2str(result) << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  // Handle encoded packets
  return ReceivePackets();
}

bool StreamEncoder::ReceivePackets()
{
  int result = 0;

  AVPacket videoPacket;
  av_init_packet(&videoPacket);
  videoPacket.stream_index = m_videoStream->index;

  // Process the encoded packets
  while (result >= 0)
  {
    result = avcodec_receive_packet(m_codecContext, &videoPacket);

    if (result == AVERROR(EAGAIN))
    {
      // We'll read again when more frames have been added
      break;
    }
    else if (result == AVERROR_EOF)
    {
      // No more packets to read
      break;
    }
    else if (result < 0)
    {
      std::cerr << "Error receiving packet: " << av_err2str(result) << std::endl;
      m_state = StreamEncoderState::Failed;
      return false;
    }

    // Rescale the pts, dts and duration of the encoded video packet to the
    // stream time base
    av_packet_rescale_ts(&videoPacket, m_codecContext->time_base, m_videoStream->time_base);

    // Write the frame. Packet is freed by the write.
    result = av_interleaved_write_frame(m_formatContext, &videoPacket);
    if (result < 0)
    {
      std::cerr << "Error writing packet: " << av_err2str(result) << std::endl;
      m_state = StreamEncoderState::Failed;
      return false;
    }
  }

  return true;
}

bool StreamEncoder::Finalize()
{
  // Validate state
  if (m_state == StreamEncoderState::Ended)
  {
    // Ignore double calls to finalize
    return true;
  }

  if (m_state != StreamEncoderState::Running)
  {
    std::cerr << "Failed to finalize: Not running" << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  m_state = StreamEncoderState::Ended;

  // Send NULL frame, in which case it is considered a flush packet. This
  // signals the end of the stream.
  int result = avcodec_send_frame(m_codecContext, nullptr);
  if (result < 0)
  {
    std::cerr << "Error ending stream: " << av_err2str(result) << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  // Handle remaining encoded packets
  if (!ReceivePackets())
    return false;

  // Write file trailer before exit
  result = av_write_trailer(m_formatContext);
  if (result < 0)
  {
    std::cerr << "Error writing trailer: " << av_err2str(result) << std::endl;
    m_state = StreamEncoderState::Failed;
    return false;
  }

  Deinitialize();

  return true;
}

/*
unsigned int StreamEncoder::ReadPacket(uint8_t* buffer, unsigned int bufferSize)
{
  // TODO

  return -1;
}
*/

void StreamEncoder::WritePacket(uint8_t* buffer, unsigned int bufferSize)
{
  const int bufPtr = reinterpret_cast<int>(buffer);
  const int bufSize = static_cast<int>(bufferSize);
  m_writePacketFnPtr(bufPtr, bufSize);
}

void StreamEncoder::Seek(int64_t offset, int whence)
{
  m_seekFnPtr(static_cast<int>(offset), whence);
}

/*
int StreamEncoder::ReadPacketInternal(void* context, uint8_t* buffer, int bufferSize)
{
  StreamEncoder* encoder = static_cast<StreamEncoder*>(context);
  if (encoder == nullptr || bufferSize <= 0)
    return AVERROR_EXIT;

  const unsigned int length = encoder->ReadPacket(buffer, bufferSize);
  if (length == 0)
    return AVERROR(EAGAIN);

  return static_cast<int>(length);
}
*/

int StreamEncoder::WritePacketInternal(void* context, uint8_t* buffer, int bufferSize)
{
  StreamEncoder* encoder = static_cast<StreamEncoder*>(context);
  if (encoder == nullptr || bufferSize <= 0)
    return AVERROR_EXIT;

  encoder->WritePacket(buffer, static_cast<unsigned int>(bufferSize));

  return bufferSize;
}

int64_t StreamEncoder::SeekInternal(void* context, int64_t offset, int whence)
{
  StreamEncoder* encoder = static_cast<StreamEncoder*>(context);
  if (encoder == nullptr)
    return AVERROR_EXIT;

  encoder->Seek(offset, TranslateWhence(whence));

  return 0;
}

int StreamEncoder::TranslateWhence(int whence)
{
  switch (whence)
  {
    // Seek from beginning of file
    case SEEK_SET:
      return 0;

    // Seek from current position
    case SEEK_CUR:
      return 1;

    // Seek from end of file
    case SEEK_END:
      return 2;

    default:
      break;
  }

  throw whence;
}
