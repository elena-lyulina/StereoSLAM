#pragma once

#include <FeatureDetector.h>
#include <FeaturePoint.h>
#include <immintrin.h>
#include <iostream>
#include <opencv2/imgproc.hpp>

typedef std::tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *> fp_tuple;

class FeatureMatcher
{
    public:
    FeatureMatcher (FeatureDetector &fd);
    std::vector<fp_tuple> getMatchedPoints (pointType type);
    std::vector<fp_tuple> *getMatchedPointsAllTypes ();
    static int SAD_8x48 (const uint8_t (&arr1)[FeaturePoint::mdSize],
                         const uint8_t (&arr2)[FeaturePoint::mdSize]);
    static int SAD_32x48 (const int32_t (&arr1)[FeaturePoint::mdSize],
                          const int32_t (&arr2)[FeaturePoint::mdSize]);
    static int SAD_32x8 (__m256i a1, __m256i a2);

    private:
    static void doMatchingCircle (const std::vector<FeaturePoint> &leftPred,
                                  const std::vector<FeaturePoint> &rightPred,
                                  const std::vector<FeaturePoint> &rightSucc,
                                  const std::vector<FeaturePoint> &leftSucc,
                                  std::vector<fp_tuple> &matched);


    std::vector<fp_tuple> matchedPoints[TYPE_AMOUNT];
};
