/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#include "video_frame.hpp"

#include <iostream>

extern "C"
{
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

// HTML uses RGBA pixels, what to use for YUV420p?
const unsigned int m_bitsPerPixel = 32;

void VideoFrame::FrameDeleter::operator()(AVFrame* frame)
{
  if (frame != nullptr)
  {
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
  }
}

VideoFrame::VideoFrame(unsigned int width, unsigned int height, AVPixelFormat pixelFormat)
  : m_width(width), m_height(height), m_pixelFormat(pixelFormat)
{
}

VideoFrame::~VideoFrame() = default;

bool VideoFrame::Initialize()
{
  // Validate state
  if (m_width == 0 || m_height == 0)
  {
    std::cerr << "Invalid dimensions, width = " << m_width << ", height = " << m_height
              << std::endl;
    return false;
  }

  // Allocate frame
  m_frame.reset(av_frame_alloc());
  if (!m_frame)
  {
    std::cerr << "Failed to allocate frame" << std::endl;
    return false;
  }

  // Set frame details
  m_frame->width = m_width;
  m_frame->height = m_height;
  m_frame->format = m_pixelFormat;

  int result = av_frame_get_buffer(m_frame.get(), 0);
  if (result < 0)
  {
    std::cerr << "Can't allocate buffer for output frame: " << av_err2str(result) << std::endl;
    return false;
  }

  // Make sure the encoder doesn't keep ref to this frame as we'll modify it
  result = av_frame_make_writable(m_frame.get());
  if (result < 0)
  {
    std::cerr << "Failed to make frame writable: " << av_err2str(result) << std::endl;
    return false;
  }

  return true;
}
