/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "cpu_video.hpp"

#include "api/video.hpp"

#include <opencv2/gapi/cpu/gcpukernel.hpp>
#include <opencv2/gapi/cpu/imgproc.hpp>

namespace video
{
// clang-format off
GAPI_OCV_KERNEL(GCPUPredictPoints, GPredictPoints)
{
  static void run(const std::vector<std::vector<cv::Point2f>>& pointHistory,
                  std::vector<cv::Point2f>& predictedPoints)
  {
    // Predict points for optical flow
    predictedPoints.resize(pointHistory[0].size());

    // TODO
    predictedPoints = pointHistory.back();
  }
};
// clang-format on
} // namespace video

cv::gapi::GKernelPackage video::kernels()
{
  static auto pkg = cv::gapi::kernels<GCPUPredictPoints>();

  return pkg;
}
