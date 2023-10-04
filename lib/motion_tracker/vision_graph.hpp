/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#pragma once

#include <stdint.h>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/gapi/gcompiled.hpp>

class VisionGraph
{
public:
  VisionGraph() = default;
  ~VisionGraph();

  void Compile(unsigned int width,
               unsigned int height,
               const cv::Mat& currentFrame,
               const cv::Mat& currentGrayscale,
               const cv::Mat& previousGrayscale);

  void ApplyGrayscale(
      // Input
      const cv::Mat& currentFrame,
      // Output
      cv::Mat& currentGrayscale);

  void CalcSceneScore(
      // Input
      const cv::Mat& previousGrayscale,
      const cv::Mat& currentGrayscale,
      double previousMafd,
      // Output
      double& currentMafd,
      double& sceneScore);

  void CalcOpticalFlow(
      // Input
      const cv::Mat& previousGrayscale,
      const cv::Mat& currentGrayscale,
      const std::vector<cv::Point2f>& previousPoints,
      const std::vector<std::vector<cv::Point2f>>& pointHistory,
      // Output
      std::vector<cv::Point2f>& currentPoints,
      std::vector<uchar>& status,
      std::vector<float>& errors);

  void FindFeatures(
      // Input
      const cv::Mat& currentGrayscale,
      unsigned int maxFeatures,
      double minDistance,
      // Output
      std::vector<cv::Point2f>& currentPoints);

private:
  cv::GCompiled m_applyGrayscale;
  cv::GCompiled m_findFeatures;
  cv::GCompiled m_calcSceneScore;
  cv::GCompiled m_calcOpticalFlow;
};
