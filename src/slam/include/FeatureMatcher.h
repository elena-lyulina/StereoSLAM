#pragma once

#include <FeatureDetector.h>
#include <FeaturePoint.h>
#include <iostream>
#include <opencv2/imgproc.hpp>

typedef std::tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *> fp_tuple;

class FeatureMatcher
{
    public:
    FeatureMatcher (FeatureDetector &fd);
    std::vector<fp_tuple> getMatchedPoints (pointType type);
    std::vector<fp_tuple> *getMatchedPointsAllTypes ();


    private:
    static void doMatchingCircle (const std::vector<FeaturePoint> &leftPred,
                                  const std::vector<FeaturePoint> &rightPred,
                                  const std::vector<FeaturePoint> &rightSucc,
                                  const std::vector<FeaturePoint> &leftSucc,
                                  std::vector<fp_tuple> &matched);


    std::vector<fp_tuple> matchedPoints[TYPE_AMOUNT];
};
