/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "imgproc.hpp"

#include <opencv2/gapi/imgproc.hpp>

cv::GMat imgproc::RGBA2Gray(const cv::GMat& rgbaImage)
{
  return cv::gapi::RGBA2Gray(rgbaImage);
}

cv::GArray<cv::Point2f> imgproc::GoodFeaturesToTrack(const cv::GMat& grayscaleImage,
                                                     const cv::GScalar& maxFeatures,
                                                     const cv::GScalar& minDistance,
                                                     double qualityLevel,
                                                     const cv::Mat& mask,
                                                     int blockSize,
                                                     bool useHarrisDetector,
                                                     double k)
{
  return GGoodFeatures::on(grayscaleImage, maxFeatures, qualityLevel, minDistance, mask, blockSize,
                           useHarrisDetector, k);
}
