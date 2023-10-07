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

extern "C"
{
#include <libavformat/avformat.h>
}

struct AVFrame;

class VideoFrame
{
public:
  VideoFrame(unsigned int width, unsigned int height, AVPixelFormat pixelFormat);
  ~VideoFrame();

  // Lifecycle functions
  bool Initialize();

  // Accessors
  unsigned int Width() const { return m_width; }
  unsigned int Height() const { return m_height; }
  AVPixelFormat PixelFormat() const { return m_pixelFormat; }
  AVFrame* FrameHandle() { return m_frame.get(); }

private:
  // Utilities
  struct FrameDeleter
  {
    void operator()(AVFrame* frame);
  };

  // Constants
  static const unsigned int m_bitsPerPixel; // TODO: Different value needed for YUV420p?

  // Construction parameters
  const unsigned int m_width;
  const unsigned int m_height;
  const AVPixelFormat m_pixelFormat;

  // Video resources
  std::unique_ptr<AVFrame, FrameDeleter> m_frame;
};
