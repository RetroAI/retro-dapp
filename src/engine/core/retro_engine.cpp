/*
 * Copyright (C) 2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

#include "retro_engine.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

#include <GLES3/gl3.h>

namespace
{
constexpr const char* CANVAS_ID = "#retroEngine";
}

RetroEngine::RetroEngine() : m_webGLContext(0)
{
}

RetroEngine::~RetroEngine()
{
  Deinitialize();
}

bool RetroEngine::Initialize()
{
  if (!InitializeWebGL())
    return false;

  return true;
}

void RetroEngine::Deinitialize()
{
  DeinitializeWebGL();
}

bool RetroEngine::InitializeWebGL()
{
  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);

  attrs.alpha = EM_TRUE;
  attrs.depth = EM_TRUE;
  attrs.stencil = EM_TRUE;
  attrs.antialias = EM_TRUE;
  attrs.preserveDrawingBuffer = EM_FALSE;
  attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
  attrs.failIfMajorPerformanceCaveat = EM_FALSE;
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;

  const EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context =
      emscripten_webgl_create_context(CANVAS_ID, &attrs);
  if (context <= 0)
    throw std::runtime_error("Failed to create WebGL context");

  EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(context);
  if (result != EMSCRIPTEN_RESULT_SUCCESS)
    throw std::runtime_error("Failed to make WebGL context current: " + std::to_string(result));

  // Success
  m_webGLContext = context;

  // Set the clear color to black (RGBA)
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);

  return true;
}

void RetroEngine::DeinitializeWebGL()
{
  if (m_webGLContext > 0)
  {
    EMSCRIPTEN_RESULT result = emscripten_webgl_destroy_context(m_webGLContext);
    if (result != EMSCRIPTEN_RESULT_SUCCESS)
      std::cerr << "Failed to destroy WebGL context (result=" << result << ")" << std::endl;

    m_webGLContext = 0;
  }
}
