/*
 * Copyright (C) 2020-2023 retro.ai
 * This file is part of retro-dapp - https://github.com/RetroAI/retro-dapp
 *
 * SPDX-License-Identifier: Apache-2.0
 * See LICENSE.md for more information.
 */

#include "vision_graph.hpp"

#include "api/imgproc.hpp"
#include "api/scene.hpp"
#include "api/video.hpp"
#include "kernels/cpu/cpu_imgproc.hpp"
#include "kernels/cpu/cpu_scene.hpp"
#include "kernels/cpu/cpu_video.hpp"

#include <tuple>

#include <opencv2/gapi/cpu/imgproc.hpp>
#include <opencv2/gapi/cpu/video.hpp>
#include <opencv2/gapi/gcomputation.hpp>

VisionGraph::~VisionGraph() = default;

void VisionGraph::Compile(unsigned int width,
                          unsigned int height,
                          const cv::Mat& currentFrame,
                          const cv::Mat& currentGrayscale,
                          const cv::Mat& previousGrayscale)
{
  // Inputs
  double previousMafd = 0.0;
  std::vector<cv::Point2f> currentPoints;
  std::vector<std::vector<cv::Point2f>> pointHistory;
  cv::Mat previousCameraMatrix;
  cv::Scalar maxFeatures;
  cv::Scalar minDistance;

  // Declare graph
  // The version of a pipeline expression with a lambda-based constructor is
  // used to keep all temporary objects in a dedicated scope

  // Convert to grayscale
  cv::GComputation grayscalePipeline(
      []()
      {
        // Input
        cv::GMat rgbaImage;

        // Output
        cv::GMat grayscaleImage;

        grayscaleImage = imgproc::RGBA2Gray(rgbaImage);

        return cv::GComputation(cv::GIn(rgbaImage), cv::GOut(grayscaleImage));
      });

  // Find features
  cv::GComputation featurePipeline(
      [width, height]()
      {
        // Input
        cv::GMat grayscaleImage;
        cv::GScalar maxFeatures;
        cv::GScalar minDistance;

        // Output
        cv::GArray<cv::Point2f> features;

        features = imgproc::GoodFeaturesToTrack(grayscaleImage, maxFeatures, minDistance);

        return cv::GComputation(cv::GIn(grayscaleImage, maxFeatures, minDistance),
                                cv::GOut(features));
      });

  // Calculate scene score
  cv::GComputation sceneScorePipeline(
      [width, height]()
      {
        // Input
        cv::GMat prevImg;
        cv::GMat nextImg;
        cv::GOpaque<double> prevMafd;

        // Output
        cv::GOpaque<double> newMafd;
        cv::GOpaque<double> sceneScore;

        std::tie(newMafd, sceneScore) =
            scene::CalcSceneScore(prevImg, nextImg, prevMafd, width, height);

        return cv::GComputation(cv::GIn(prevImg, nextImg, prevMafd), cv::GOut(newMafd, sceneScore));
      });

  // Calculate optical flow
  cv::GComputation opticalFlowPipeline(
      []()
      {
        // Input
        cv::GMat prevImg;
        cv::GMat nextImg;
        cv::GArray<cv::Point2f> previousPoints;
        cv::GArray<std::vector<cv::Point2f>> pointHistory;

        // Intermediate
        cv::GArray<cv::Point2f> predictedPoints;

        // Output
        cv::GArray<cv::Point2f> newPoints;
        cv::GArray<uchar> status;
        cv::GArray<float> errors;

        // Predict next discovered points for optical flow
        predictedPoints = video::PredictPoints(pointHistory);

        // Perform optical flow calculation
        std::tie(newPoints, status, errors) =
            video::CalcOpticalFlow(prevImg, nextImg, previousPoints, predictedPoints);

        return cv::GComputation(cv::GIn(prevImg, nextImg, previousPoints, pointHistory),
                                cv::GOut(newPoints, status, errors));
      });

  // Declare custom and gapi kernels
  static auto kernels =
      cv::gapi::combine(cv::gapi::imgproc::cpu::kernels(), cv::gapi::video::cpu::kernels(),
                        imgproc::kernels(), scene::kernels(), video::kernels());

  // Compile computation graphs in serial mode
  m_applyGrayscale =
      grayscalePipeline.compile(cv::descr_of(currentFrame), cv::compile_args(kernels));
  m_findFeatures =
      featurePipeline.compile(cv::descr_of(currentGrayscale), cv::descr_of(maxFeatures),
                              cv::descr_of(minDistance), cv::compile_args(kernels));
  m_calcSceneScore =
      sceneScorePipeline.compile(cv::descr_of(previousGrayscale), cv::descr_of(currentGrayscale),
                                 cv::descr_of(previousMafd), cv::compile_args(kernels));
  m_calcOpticalFlow = opticalFlowPipeline.compile(
      cv::descr_of(previousGrayscale), cv::descr_of(currentGrayscale), cv::descr_of(currentPoints),
      cv::descr_of(pointHistory), cv::compile_args(kernels));
}

void VisionGraph::ApplyGrayscale(
    // Input
    const cv::Mat& currentFrame,
    // Output
    cv::Mat& currentGrayscale)
{
  auto inVector = cv::gin(currentFrame);
  auto outVector = cv::gout(currentGrayscale);
  m_applyGrayscale(std::move(inVector), std::move(outVector));
}

void VisionGraph::CalcSceneScore(
    // Input
    const cv::Mat& previousGrayscale,
    const cv::Mat& currentGrayscale,
    double previousMafd,
    // Output
    double& currentMafd,
    double& sceneScore)
{
  auto inVector = cv::gin(previousGrayscale, currentGrayscale, previousMafd);
  auto outVector = cv::gout(currentMafd, sceneScore);
  m_calcSceneScore(std::move(inVector), std::move(outVector));
}

void VisionGraph::CalcOpticalFlow(
    // Input
    const cv::Mat& previousGrayscale,
    const cv::Mat& currentGrayscale,
    const std::vector<cv::Point2f>& previousPoints,
    const std::vector<std::vector<cv::Point2f>>& pointHistory,
    // Output
    std::vector<cv::Point2f>& currentPoints,
    std::vector<uchar>& status,
    std::vector<float>& errors)
{
  auto inVector = cv::gin(previousGrayscale, currentGrayscale, previousPoints, pointHistory);
  auto outVector = cv::gout(currentPoints, status, errors);
  m_calcOpticalFlow(std::move(inVector), std::move(outVector));
}

void VisionGraph::FindFeatures(
    // Input
    const cv::Mat& currentGrayscale,
    unsigned int maxFeatures,
    double minDistance,
    // Output
    std::vector<cv::Point2f>& currentPoints)
{
  cv::Scalar maxFeaturesScalar(maxFeatures > 0 ? static_cast<double>(maxFeatures) : -1.0);
  cv::Scalar minDistanceScalar(minDistance);

  auto inVector = cv::gin(currentGrayscale, maxFeaturesScalar, minDistanceScalar);
  auto outVector = cv::gout(currentPoints);
  m_findFeatures(std::move(inVector), std::move(outVector));
}
