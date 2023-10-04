/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "scene.hpp"

scene::TplDoubles scene::CalcSceneScore(const cv::GMat& prevImg,
                                        const cv::GMat& nextImg,
                                        const cv::GOpaque<double>& prevMafd,
                                        unsigned int width,
                                        unsigned int height)
{
  return GCalcSceneScore::on(prevImg, nextImg, prevMafd, width, height);
}
