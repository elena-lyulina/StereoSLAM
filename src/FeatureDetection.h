//
// Created by elena on 20.03.19.
//

#include "Frame.h"
#include "FeaturePoint.h"

#ifndef STEREOSLAM_FEATUREDETECTION_H
#define STEREOSLAM_FEATUREDETECTION_H

#endif // STEREOSLAM_FEATUREDETECTION_H

class FeatureDetection
{
    public:
    static void findMatches (Frame frame1,
                             Frame frame2,
                             vector<FeaturePoint> candidates1,
                             vector<FeaturePoint> candidates2,
                             vector<pair<FeaturePoint, FeaturePoint>> &matched, int error);
};