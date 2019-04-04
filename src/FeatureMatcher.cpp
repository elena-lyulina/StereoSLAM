#include "FeatureMatcher.h"
#include <opencv/cv.hpp>


FeatureMatcher::FeatureMatcher (FeatureDetector &fd)
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


const FeaturePoint *
findMatchesOnArea (const FeaturePoint &p, const std::vector<FeaturePoint> &candidates, int areaSize)
{
    int minError = SADofDescriptors (p, candidates[0]);
    const FeaturePoint *matchedPoint = &candidates[0];
    for (int i = 0; i < candidates.size (); i++)
    {
        if (abs (candidates[i].col - p.col) <= areaSize && abs (candidates[i].row - p.row) <= areaSize)
        {

            int error = SADofDescriptors (candidates[i], p);

            if (error < minError)
            {
                minError = error;
                matchedPoint = &candidates[i];
            }
        }
    }

    return matchedPoint;
}

template <typename Comparator>
const FeaturePoint *
findMatchesOnStrip (const FeaturePoint &p, const std::vector<FeaturePoint> &candidates, int stripWidth, Comparator comp)
{
    int minError = SADofDescriptors (p, candidates[0]);
    const FeaturePoint *matchedPoint = &candidates[0];

    for (int i = 0; i < candidates.size (); i++)
    {
        if (comp (candidates[i].col, p.col) && abs (candidates[i].row - p.row) <= stripWidth)
        {

            int error = SADofDescriptors (candidates[i], p);

            if (error < minError)
            {
                minError = error;
                matchedPoint = &candidates[i];
            }
        }
    }

    return matchedPoint;
}


void FeatureMatcher::doMatchingCircle (
const std::vector<FeaturePoint> &leftPred,
const std::vector<FeaturePoint> &rightPred,
const std::vector<FeaturePoint> &rightSucc,
const std::vector<FeaturePoint> &leftSucc,
std::vector<std::tuple<const FeaturePoint *, const FeaturePoint *, const FeaturePoint *, const FeaturePoint *>> &matched)
{

    int stripWidth = 10;
    int areaWidth = 50;

    for (int i = 0; i < leftPred.size (); i++)
    {
        // возвращать указатель или id
        // checking strip on right pred image, it should be to the right of leftPred[i]
        const FeaturePoint *rpMatchedPoint =
        findMatchesOnStrip (leftPred[i], rightPred, stripWidth, std::less<int> ());

        // checking area on right succ image
        const FeaturePoint *rsMatchedPoint = findMatchesOnArea (*rpMatchedPoint, rightSucc, areaWidth);

        // checking strip on left succ image, it should be to the left of rsMatchedPoint
        const FeaturePoint *lsMatchedPoint =
        findMatchesOnStrip (*rsMatchedPoint, leftSucc, stripWidth, std::greater<int> ());

        // checking are on left pred image
        const FeaturePoint *lpMatchedPoint = findMatchesOnArea (*lsMatchedPoint, leftPred, areaWidth);

        if (&leftPred[i] == lpMatchedPoint)
        {
            matched.emplace_back (lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
        }
    }
}

std::vector<fp_tuple> FeatureMatcher::getMatchedPoints (int type)
{
    return matchedPoints[type];
}
std::vector<fp_tuple> *FeatureMatcher::getMatchedPointsAllTypes ()
{
    return matchedPoints;
}