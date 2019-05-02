#pragma once
#include <FeaturePoint.h>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <util.h>


class FeatureDetector
{
    public:
    FeatureDetector (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames);
    std::vector<FeaturePoint> &getDetectedPoints (frameNumber frame, pointType type);
    static const int supprArea = 5;


    private:
    std::pair<Frame, Frame> *predFrames;
    std::pair<Frame, Frame> *succFrames;

    std::vector<FeaturePoint> detectedPoints[FRAME_AMOUNT][TYPE_AMOUNT];
};
