/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "cpu_imgproc.hpp"

#include "api/imgproc.hpp"

#include <opencv2/gapi/cpu/gcpukernel.hpp>
#include <opencv2/imgproc.hpp>

namespace imgproc
{
// clang-format off
GAPI_OCV_KERNEL(GCPUGoodFeatures, GGoodFeatures)
{
  static void run(const cv::Mat& image,
                  const cv::Scalar& maxCorners,
                  double qualityLevel,
                  const cv::Scalar& minDistance,
                  const cv::Mat& mask,
                  int blockSize,
                  bool useHarrisDetector,
                  double k,
                  std::vector<cv::Point2f>& out)
  {
    // Find good features
    cv::goodFeaturesToTrack(image, out, static_cast<int>(maxCorners[0]), qualityLevel,
                            minDistance[0], mask, blockSize, useHarrisDetector, k);
  }
};
// clang-format on
} // namespace imgproc

cv::gapi::GKernelPackage imgproc::kernels()
{
  static auto pkg = cv::gapi::kernels<GCPUGoodFeatures>();

  return pkg;
}
