/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <type_traits> // for std::add_pointer
#include <vector>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libswscale/swscale.h>
}

namespace emscripten
{
class val;
}

class VideoFrame;

enum class StreamEncoderState
{
  Init,
  Running,
  Ended,
  Failed,
};

class StreamEncoder
{
public:
  /*!
   * \brief Create a stream encoder instance
   */
  StreamEncoder(const std::string& fileName,
                int width,
                int height,
                int timeBaseNum,
                int timeBaseDen,
                int realFrameRateNum,
                int realFrameRateDen,
                int videoBitRate,
                int writeBufferSize,
                int writePacketFnPtr,
                int seekFnPtr);
  ~StreamEncoder();

  // Accessors
  StreamEncoderState GetState() const { return m_state; }
  unsigned int GetVideoWidth() const { return m_width; }
  unsigned int GetVideoHeight() const { return m_height; }
  unsigned int GetTimeBaseNumerator() const { return m_timeBaseNum; }
  unsigned int GetTimeBaseDenominator() const { return m_timeBaseDen; }
  unsigned int GetRealFrameRateNumerator() const { return m_realFrameRateNum; }
  unsigned int GetRealFrameRateDenominator() const { return m_realFrameRateDen; }
  unsigned int GetWriteBufferSize() const { return m_writeBufferSize; }

  // Video API
  bool OpenVideo();
  bool AddFrame(const emscripten::val& frameData, int framePts);
  bool Finalize();

private:
  // Lifecycle functions
  bool Initialize();
  void Deinitialize();

  // Video functions
  bool ReceivePackets();

  // IO functions
  //unsigned int ReadPacket(uint8_t* buffer, unsigned int bufferSize);
  void WritePacket(uint8_t* buffer, unsigned int bufferSize);
  void Seek(int64_t offset, int whence);

  // libav internal IO functions
  //static int ReadPacketInternal(void* context, uint8_t* buffer, int bufferSize);
  static int WritePacketInternal(void* context, uint8_t* buffer, int bufferSize);
  static int64_t SeekInternal(void* context, int64_t offset, int whence);

  // Utility functions
  bool CreateContext();
  bool PrepareVideoEncoder();
  bool InitializeVideoStream();
  void EnableGlobalHeaders();
  void PrintInfo();
  bool WriteHeader();
  static int TranslateWhence(int whence);

  // Types
  using WritePacketFnPtr = std::add_pointer<void(int buffer, int bufferSize)>::type;
  using SeekFnPtr = std::add_pointer<void(int offset, int whence)>::type;

  // Constants
  static const AVPixelFormat m_inputFormat;
  static const AVPixelFormat m_targetFormat;
  static const unsigned int m_cachePageSize;

  // Construction parameters
  const std::string m_fileName;
  const unsigned int m_width;
  const unsigned int m_height;
  const unsigned int m_timeBaseNum;
  const unsigned int m_timeBaseDen;
  const unsigned int m_realFrameRateNum;
  const unsigned int m_realFrameRateDen;
  const unsigned int m_videoBitRate;
  const unsigned int m_writeBufferSize;
  const WritePacketFnPtr m_writePacketFnPtr;
  const SeekFnPtr m_seekFnPtr;

  // State parameters
  StreamEncoderState m_state = StreamEncoderState::Init;

  // Video resources
  AVIOContext* m_ioContext = nullptr;
  AVFormatContext* m_formatContext = nullptr;
  AVStream* m_videoStream = nullptr;
  AVCodecContext* m_codecContext = nullptr;
  SwsContext* m_scaler = nullptr;
  uint8_t* m_inputFrameBuffer = nullptr; // RGBA
  size_t m_inputFrameSize = 0;
  std::unique_ptr<VideoFrame> m_targetFrame; // YUV420p
};
