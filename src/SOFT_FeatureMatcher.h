#pragma once

#include "FeaturePoint.h"
#include "SOFT_FeatureDetector.h"
#include <iostream>
#include <opencv2/imgproc.hpp>

typedef tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *> fp_tuple;

class SOFT_FeatureMatcher
{
    public:
    SOFT_FeatureMatcher (SOFT_FeatureDetector &fd);
    vector<fp_tuple> getMatchedPoints (int type);
    vector<fp_tuple> *getMatchedPointsAllTypes ();


    private:
    static void doMatchingCircle (const vector<FeaturePoint> &leftPred,
                                  const vector<FeaturePoint> &rightPred,
                                  const vector<FeaturePoint> &rightSucc,
                                  const vector<FeaturePoint> &leftSucc,
                                  vector<fp_tuple> &matched);


    vector<fp_tuple> matchedPoints[TYPE_AMOUNT];
};
