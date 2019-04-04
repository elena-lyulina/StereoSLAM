#include "SOFT_FeatureMatcher.h"
#include <opencv/cv.hpp>


SOFT_FeatureMatcher::SOFT_FeatureMatcher (SOFT_FeatureDetector &fd)
{
    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, BLOB_MAX), fd.getDetectedPoints (RIGHT_PRED, BLOB_MAX),
                      fd.getDetectedPoints (RIGHT_SUCC, BLOB_MAX),
                      fd.getDetectedPoints (LEFT_SUCC, BLOB_MAX), matchedPoints[BLOB_MAX]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, BLOB_MIN), fd.getDetectedPoints (RIGHT_PRED, BLOB_MIN),
                      fd.getDetectedPoints (RIGHT_SUCC, BLOB_MIN),
                      fd.getDetectedPoints (LEFT_SUCC, BLOB_MIN), matchedPoints[BLOB_MIN]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, CORNER_MAX), fd.getDetectedPoints (RIGHT_PRED, CORNER_MAX),
                      fd.getDetectedPoints (RIGHT_SUCC, CORNER_MAX),
                      fd.getDetectedPoints (LEFT_SUCC, CORNER_MAX), matchedPoints[CORNER_MAX]);

    doMatchingCircle (fd.getDetectedPoints (LEFT_PRED, CORNER_MIN), fd.getDetectedPoints (RIGHT_PRED, CORNER_MIN),
                      fd.getDetectedPoints (RIGHT_SUCC, CORNER_MIN),
                      fd.getDetectedPoints (LEFT_SUCC, CORNER_MIN), matchedPoints[CORNER_MIN]);
}

int SADofDescriptors (const FeaturePoint &fp1, const FeaturePoint &fp2)
{
    const int N = FeaturePoint::matchingDescriptorSize;
    int sad = 0;
    for (int i = 0; i < N; ++i)
        sad += std::abs (fp1.matchingDescriptor[i] - fp2.matchingDescriptor[i]);
    return sad;
}


const FeaturePoint *findMatchesOnArea (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int areaSize)
{
    int minError = SADofDescriptors (p, candidates[0]);
    const FeaturePoint *matchedPoint = &candidates[0];
    for (auto &rsPoint : candidates)
    {
        if (abs (rsPoint.col - p.col) <= areaSize && abs (rsPoint.row - p.row) <= areaSize)
        {

            int error = SADofDescriptors (rsPoint, p);

            if (error < minError)
            {
                minError = error;
                matchedPoint = &rsPoint;
            }
        }
    }

    return matchedPoint;
}

template <typename Comparator>
const FeaturePoint *
findMatchesOnStrip (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int stripWidth, Comparator comp)
{
    int minError = SADofDescriptors (p, candidates[0]);
    const FeaturePoint *matchedPoint = &candidates[0];

    for (auto &rsPoint : candidates)
    {
        if (comp (rsPoint.col, p.col) && abs (rsPoint.row - p.row) <= stripWidth)
        {

            int error = SADofDescriptors (rsPoint, p);

            if (error < minError)
            {
                minError = error;
                matchedPoint = &rsPoint;
            }
        }
    }

    return matchedPoint;
}


void SOFT_FeatureMatcher::doMatchingCircle (
const vector<FeaturePoint> &leftPred,
const vector<FeaturePoint> &rightPred,
const vector<FeaturePoint> &rightSucc,
const vector<FeaturePoint> &leftSucc,
vector<tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *>> &matched)
{

    int stripWidth = 10;
    int areaWidth = 50;

    for (auto &lpPoint : leftPred)
    {

        // возвращать указатель или id
        // checking strip on right pred image, it should be to the right of lpPoint
        const FeaturePoint *rpMatchedPoint =
        findMatchesOnStrip (lpPoint, rightPred, stripWidth, std::less<int> ());

        // checking area on right succ image
        const FeaturePoint *rsMatchedPoint = findMatchesOnArea (*rpMatchedPoint, rightSucc, areaWidth);

        // checking strip on left succ image, it should be to the left of rsMatchedPoint
        const FeaturePoint *lsMatchedPoint =
        findMatchesOnStrip (*rsMatchedPoint, leftSucc, stripWidth, std::greater<int> ());

        // checking are on left pred image
        const FeaturePoint *lpMatchedPoint = findMatchesOnArea (*lsMatchedPoint, leftPred, areaWidth);

        if (&lpPoint == lpMatchedPoint)
        {
            matched.emplace_back (lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
        }
    }
}

vector<fp_tuple> SOFT_FeatureMatcher::getMatchedPoints (int type)
{
    return matchedPoints[type];
}
vector<fp_tuple> *SOFT_FeatureMatcher::getMatchedPointsAllTypes ()
{
    return matchedPoints;
}
