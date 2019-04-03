#pragma once

#include "FeaturePoint.h"
#include "Frame.h"


class FeatureDetection
{
    public:
    static void findMatches (vector<FeaturePoint> candidates1,
                             vector<FeaturePoint> candidates2,
                             vector<pair<FeaturePoint, FeaturePoint>> &matched,
                             int error);

    static void doMatchingCircle (const vector<FeaturePoint> &leftPred,
                                  const vector<FeaturePoint> &rightPred,
                                  const vector<FeaturePoint> &rightSucc,
                                  const vector<FeaturePoint> &leftSucc,
                                  vector<tuple<const FeaturePoint*, const FeaturePoint*, const FeaturePoint*, const FeaturePoint*>> &matched);
};