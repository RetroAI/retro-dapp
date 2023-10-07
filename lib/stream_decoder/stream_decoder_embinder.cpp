/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#include "stream_decoder.h"

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(stream_decoder)
{
  enum_<StreamDecoderState>("StreamDecoderState")
      .value("Init", StreamDecoderState::Init)
      .value("Running", StreamDecoderState::Running)
      .value("HasFrame", StreamDecoderState::HasFrame)
      .value("Ended", StreamDecoderState::Ended)
      .value("Failed", StreamDecoderState::Failed);

  class_<StreamDecoder>("StreamDecoder")
      .constructor<const std::string&, int, int>()
      .property("state", &StreamDecoder::GetState)
      .property("frameSize", &StreamDecoder::GetFrameSize)
      .property("frameWidth", &StreamDecoder::GetFrameWidth)
      .property("frameHeight", &StreamDecoder::GetFrameHeight)
      .property("timeBaseNum", &StreamDecoder::GetTimeBaseNum)
      .property("timeBaseDen", &StreamDecoder::GetTimeBaseDen)
      .property("realFrameRateNum", &StreamDecoder::GetRealFrameRateNum)
      .property("realFrameRateDen", &StreamDecoder::GetRealFrameRateDen)
      .property("videoBitRate", &StreamDecoder::GetVideoBitRate)
      .function("openVideo", &StreamDecoder::OpenVideo)
      .function("addPacket", &StreamDecoder::AddPacket)
      .function("decode", &StreamDecoder::Decode)
      .function("getFrameBuffer", &StreamDecoder::GetFrameBuffer)
      .function("getFramePts", &StreamDecoder::GetFramePts);
}
