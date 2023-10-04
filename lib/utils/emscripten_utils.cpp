/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "emscripten_utils.hpp"

unsigned int EmscriptenUtils::ArrayLength(const emscripten::val& array)
{
  return array["length"].as<unsigned int>();
}

void EmscriptenUtils::GetArrayData(const emscripten::val& array,
                                   uint8_t* dest,
                                   unsigned int destLength)
{
  emscripten::val memoryView{emscripten::typed_memory_view(destLength, dest)};
  memoryView.call<void>("set", array);
}
