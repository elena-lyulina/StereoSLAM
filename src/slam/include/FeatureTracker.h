#pragma once

#include <src/slam/include/FeatureDetector.h>
#include <src/slam/include/FeatureMatcher.h>
#include <Frame.h>


class FeatureTracker
{
    public:
    FeatureTracker (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames)
    : predFrames (&predFrames), succFrames (&succFrames),
      fd (FeatureDetector (predFrames, succFrames)), fm (FeatureMatcher (fd)){};

    void handleNextFrames (std::pair<Frame, Frame> &frames);
    std::vector<fp_tuple> *getCyclicallyMatchedPoints ();
    void showMP (int typeAmount, pointType types[]);

    private:
    std::pair<Frame, Frame> *predFrames;
    std::pair<Frame, Frame> *succFrames;

    FeatureDetector fd;
    FeatureMatcher fm;

    void showMPOnImages (cv::Mat *imagesRGB, pointType type);
    void getImagesRGB (cv::Mat *imagesRGB);
};
