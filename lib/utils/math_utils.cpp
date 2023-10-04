/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "math_utils.hpp"

#include <cmath>

double MathUtils::GeometricMean(unsigned int width, unsigned int height)
{
  double product = static_cast<double>(width) * static_cast<double>(height);

  return std::sqrt(product);
}
