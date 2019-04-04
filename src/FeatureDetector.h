#pragma once
#include "FeaturePoint.h"
#include <iostream>
#include <opencv2/imgproc.hpp>

enum pointTypes
{
    TYPE_AMOUNT = 4,

    BLOB_MAX = 0,
    BLOB_MIN = 1,
    CORNER_MAX = 2,
    CORNER_MIN = 3
};

enum frameNumber
{
    FRAME_AMOUNT = 4,

    LEFT_PRED = 0,
    RIGHT_PRED = 1,
    RIGHT_SUCC = 2,
    LEFT_SUCC = 3
};

class FeatureDetector
{
    public:
    FeatureDetector (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames);
    std::vector<FeaturePoint> &getDetectedPoints (int frame, int type);


    private:
    std::pair<Frame, Frame> *predFrames;
    std::pair<Frame, Frame> *succFrames;

    std::vector<FeaturePoint> detectedPoints[FRAME_AMOUNT][TYPE_AMOUNT];
};
