//
// Created by elena on 20.03.19.
//

#include "FeaturePoint.h"
#include "Frame.h"

#pragma once

#ifndef STEREOSLAM_FEATUREDETECTION_H
#define STEREOSLAM_FEATUREDETECTION_H

#endif // STEREOSLAM_FEATUREDETECTION_H

class FeatureDetection
{
    public:
    static void findMatches (vector<FeaturePoint> candidates1,
                             vector<FeaturePoint> candidates2,
                             vector<pair<FeaturePoint, FeaturePoint>> &matched,
                             int error);

    static void doMatchingCircle(vector<FeaturePoint> leftPred,
                                 vector<FeaturePoint> rightPred,
                                 vector<FeaturePoint> rightSucc,
                                 vector<FeaturePoint> leftSucc,
                                 vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> &matched);
};