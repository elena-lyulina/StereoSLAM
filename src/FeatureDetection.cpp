#include "FeatureDetection.h"
#include "FeaturePoint.h"
#include <numeric>


void FeatureDetection::findMatches (vector<FeaturePoint> candidates1,
                                    vector<FeaturePoint> candidates2,
                                    vector<pair<FeaturePoint, FeaturePoint>> &matched,
                                    int error)
{
    vector<int> SAD (candidates1[0].matchingDescriptor.size ());
    int (*absolute) (int) = &abs;
    int r;
    for (auto fp1 : candidates1)
    {
        for (auto fp2 : candidates2)
        {

            transform (fp1.matchingDescriptor.begin (), fp1.matchingDescriptor.end (), fp2.matchingDescriptor.begin (),
                       SAD.begin (), std::minus<int> ());

            transform (SAD.begin (), SAD.end (), SAD.begin (), absolute);
            r = accumulate (SAD.begin (), SAD.end (), 0);
            if (r < error)
            {
                matched.emplace_back (make_pair (fp1, fp2));
                //  cout << "size: " << matched.size() << " r: " << r <<endl;
            }
        }
    }
}

int SADofDescriptors (const FeaturePoint &fp1, const FeaturePoint &fp2)
{
#if 1
  const int N = fp1.matchingDescriptor.size();
  int sad = 0;
  for (int i = 0; i < N; ++i)
    sad += std::abs(fp1.matchingDescriptor[i] - fp2.matchingDescriptor[i]);
  return sad;
#else
    vector<int> SAD (fp1.matchingDescriptor.size ());
    int (*absolute) (int) = &abs;

    transform (fp1.matchingDescriptor.begin (), fp1.matchingDescriptor.end (), fp2.matchingDescriptor.begin (),
               SAD.begin (), std::minus<int> ());

    transform (SAD.begin (), SAD.end (), SAD.begin (), absolute);
    return accumulate (SAD.begin (), SAD.end (), 0);
#endif
}


const FeaturePoint* findMatchesOnArea (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int areaSize)
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
const FeaturePoint *findMatchesOnStrip (const FeaturePoint &p, const vector<FeaturePoint> &candidates, int stripWidth, Comparator comp)
{
    int minError = SADofDescriptors (p, candidates[0]);
    const FeaturePoint* matchedPoint = &candidates[0];

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


void FeatureDetection::doMatchingCircle (const vector<FeaturePoint> &leftPred,
                                         const vector<FeaturePoint> &rightPred,
                                         const vector<FeaturePoint> &rightSucc,
                                         const vector<FeaturePoint> &leftSucc,
                                         vector<tuple<const FeaturePoint*, const FeaturePoint*, const FeaturePoint*, const FeaturePoint*>> &matched)
{

    int stripWidth = 10;
    int areaWidth = 50;

    for (auto &lpPoint : leftPred)
    {

      // возвращать указатель или id
        // checking strip on right pred image, it should be to the right of lpPoint
        const FeaturePoint* rpMatchedPoint = findMatchesOnStrip (lpPoint, rightPred, stripWidth, std::less<int> ());

        // checking area on right succ image
        const FeaturePoint* rsMatchedPoint = findMatchesOnArea (*rpMatchedPoint, rightSucc, areaWidth);

        // checking strip on left succ image, it should be to the left of rsMatchedPoint
        const FeaturePoint* lsMatchedPoint =
        findMatchesOnStrip (*rsMatchedPoint, leftSucc, stripWidth, std::greater<int> ());

        // checking are on left pred image
        const FeaturePoint* lpMatchedPoint = findMatchesOnArea (*lsMatchedPoint, leftPred, areaWidth);

        if (&lpPoint == lpMatchedPoint)
        {
            matched.emplace_back (lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
        }
    }
}
