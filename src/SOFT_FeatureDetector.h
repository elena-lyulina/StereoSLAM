#pragma once
#include "FeaturePoint.h"
#include <iostream>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


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

class SOFT_FeatureDetector
{
    public:
    SOFT_FeatureDetector (pair<Frame, Frame> &predFrames, pair<Frame, Frame> &succFrames);
    vector<FeaturePoint> &getDetectedPoints (int frame, int type);


    private:
    pair<Frame, Frame> *predFrames;
    pair<Frame, Frame> *succFrames;

    vector<FeaturePoint> detectedPoints[FRAME_AMOUNT][TYPE_AMOUNT];
};
