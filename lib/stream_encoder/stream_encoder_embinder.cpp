/*
 * Copyright (C) 2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.txt for more information.
 */

#include "stream_encoder.hpp"

#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(stream_encoder)
{
  enum_<StreamEncoderState>("StreamEncoderState")
      .value("Init", StreamEncoderState::Init)
      .value("Running", StreamEncoderState::Running)
      .value("Ended", StreamEncoderState::Ended)
      .value("Failed", StreamEncoderState::Failed);

  class_<StreamEncoder>("StreamEncoder")
      .constructor<const std::string&, int, int, int, int, int, int, int, int, int, int>()
      .property("state", &StreamEncoder::GetState)
      .property("videoWidth", &StreamEncoder::GetVideoWidth)
      .property("videoHeight", &StreamEncoder::GetVideoHeight)
      .property("timeBaseNum", &StreamEncoder::GetTimeBaseNumerator)
      .property("timeBaseDen", &StreamEncoder::GetTimeBaseDenominator)
      .property("realFrameRateNum", &StreamEncoder::GetRealFrameRateNumerator)
      .property("realFrameRateDen", &StreamEncoder::GetRealFrameRateDenominator)
      .property("writeBufferSize", &StreamEncoder::GetWriteBufferSize)
      .function("openVideo", &StreamEncoder::OpenVideo)
      .function("addFrame", &StreamEncoder::AddFrame)
      .function("finalize", &StreamEncoder::Finalize);
}
