/*
 * Copyright (C) 2024 retro.ai
 * This file is part of retro3 - https://github.com/retroai/retro3
 *
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * See the file LICENSE.txt for more information.
 */

#pragma once

#include <emscripten/html5.h>

class RetroEngine
{
public:
  RetroEngine();
  ~RetroEngine();

  /*!
   * \brief Initialize the engine
   *
   * \return true if successful, false otherwise
   */
  bool Initialize();

  /*!
   * \brief Deinitialize the engine
   */
  void Deinitialize();

private:
  bool InitializeWebGL();
  void DeinitializeWebGL();

  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_webGLContext;
};
