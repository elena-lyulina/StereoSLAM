#pragma once

#include <Frame.h>
#include <src/slam/include/FeatureDetector.h>
#include <src/slam/include/FeatureMatcher.h>


class FeatureTracker
{
    public:
    FeatureTracker (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames)
    : predFrames (&predFrames), succFrames (&succFrames),
      fd (FeatureDetector (predFrames, succFrames)), fm (FeatureMatcher (fd)){};

    void handleNextFrames (std::pair<Frame, Frame> &frames);
    std::vector<fp_tuple> *getCyclicallyMatchedPoints ();
    void showMP (int typeAmount, pointType types[]);
    void showNMP (int typeAmount, pointType types[]);

    private:
    std::pair<Frame, Frame> *predFrames;
    std::pair<Frame, Frame> *succFrames;
    // BGR
    cv::Scalar colors[TYPE_AMOUNT] = { cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 255)};

    FeatureDetector fd;
    FeatureMatcher fm;

    void showMPOnImages (cv::Mat *imagesRGB, pointType type);
    void showNMPOnImages (cv::Mat *imagesRGB, pointType type, cv::Scalar color);
    void getImagesRGB (cv::Mat *imagesRGB);
};
