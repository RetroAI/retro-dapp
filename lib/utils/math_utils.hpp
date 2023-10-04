/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#pragma once

class MathUtils
{
public:
  /*!
   * \brief Calculate the geometric mean of two dimensions
   *
   * \param width An image width, in pixels
   * \param height An image height, in pixels
   *
   * \return The geometric mean, the square root of the product
   */
  static double GeometricMean(unsigned int width, unsigned int height);
};
