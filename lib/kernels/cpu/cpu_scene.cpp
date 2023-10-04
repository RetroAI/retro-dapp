/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "cpu_scene.hpp"

#include "api/scene.hpp"
#include "utils/image_utils.hpp"

#include <opencv2/gapi/cpu/gcpukernel.hpp>

namespace scene
{
// clang-format off
GAPI_OCV_KERNEL(GCPUCalcSceneScore, GCalcSceneScore)
{
  static void run(const cv::Mat& prevImg,
                  const cv::Mat& nextImg,
                  double prevMafd,
                  unsigned int width,
                  unsigned int height,
                  double& nextMafd,
                  double& sceneScore)
  {
    // Calculate the scene score given a frame and its previous frame
    nextMafd = ImageUtils::CalcSceneMAFD(prevImg.data, nextImg.data, width, height);
    sceneScore = ImageUtils::CalcSceneScore(prevMafd, nextMafd);
  }
};
// clang-format on
} // namespace scene

cv::gapi::GKernelPackage scene::kernels()
{
  static auto pkg = cv::gapi::kernels<GCPUCalcSceneScore>();

  return pkg;
}
