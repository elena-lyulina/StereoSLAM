#pragma once
#include <FeaturePoint.h>
#include <util.h>
#include <iostream>
#include <opencv2/imgproc.hpp>



class FeatureDetector
{
    public:
    FeatureDetector (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames);
    std::vector<FeaturePoint> &getDetectedPoints (frameNumber frame, pointType type);


    private:
    std::pair<Frame, Frame> *predFrames;
    std::pair<Frame, Frame> *succFrames;

    std::vector<FeaturePoint> detectedPoints[FRAME_AMOUNT][TYPE_AMOUNT];
};
