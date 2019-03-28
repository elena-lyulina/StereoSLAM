#include "FeatureDetection.h"
#include "FeaturePoint.h"
#include <numeric>


void FeatureDetection::findMatches (vector<FeaturePoint> candidates1,
                                    vector<FeaturePoint> candidates2,
                                    vector<pair<FeaturePoint, FeaturePoint>> &matched,
                                    int error)
{
    vector<int> SAD(candidates1[0].descriptor.size());
    int (*absolute) (int) = &abs;
    int r;
    for (auto fp1 : candidates1)
    {
        for (auto fp2 : candidates2)
        {

            transform (fp1.descriptor.begin (), fp1.descriptor.end (), fp2.descriptor.begin (),
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

int SADofDescriptors(FeaturePoint fp1, FeaturePoint fp2) {
    vector<int> SAD(fp1.descriptor.size());
    int (*absolute) (int) = &abs;

    transform (fp1.descriptor.begin (), fp1.descriptor.end (), fp2.descriptor.begin (),
               SAD.begin (), std::minus<int> ());

    transform (SAD.begin (), SAD.end (), SAD.begin (), absolute);
    return accumulate (SAD.begin (), SAD.end (), 0);
}


FeaturePoint findMatchesOnArea(FeaturePoint p, vector<FeaturePoint> candidates, int areaSize) {
    int minError = SADofDescriptors(p, candidates[0]);
    FeaturePoint matchedPoint = candidates[0];
    for (auto rsPoint : candidates) {
        if (abs(rsPoint.col - p.col) <= areaSize && abs(rsPoint.row - p.row) <=  areaSize) {

            int error = SADofDescriptors(rsPoint, p);

            if (error < minError) {
                minError = error;
                matchedPoint = rsPoint;
            }
        }
    }

    return matchedPoint;

}

template <typename Comparator>
FeaturePoint findMatchesOnStrip(FeaturePoint p, vector<FeaturePoint> candidates, int stripWidth, Comparator comp) {
    int minError = SADofDescriptors(p, candidates[0]);
    FeaturePoint matchedPoint = candidates[0];

    for (auto rsPoint : candidates) {
        if (comp(rsPoint.col, p.col) && abs(rsPoint.row - p.row) <=  stripWidth) {

            int error = SADofDescriptors(rsPoint, p);

            if (error < minError) {
                minError = error;
                matchedPoint = rsPoint;
            }
        }
    }

    return matchedPoint;

}


void FeatureDetection::doMatchingCircle(vector<FeaturePoint> leftPred,
                                        vector<FeaturePoint> rightPred,
                                        vector<FeaturePoint> rightSucc,
                                        vector<FeaturePoint> leftSucc,
                                        vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> &matched) {

    int stripWidth = 10;
    int areaWidth = 50;

    for (auto lpPoint : leftPred) {

        // checking strip on right pred image, it should be to the right of lpPoint
        FeaturePoint rpMatchedPoint = findMatchesOnStrip(lpPoint, rightPred, stripWidth, std::less<int>());

        // checking area on right succ image
        FeaturePoint rsMatchedPoint = findMatchesOnArea(rpMatchedPoint, rightSucc, areaWidth);

        // checking strip on left succ image, it should be to the left of rsMatchedPoint
        FeaturePoint lsMatchedPoint = findMatchesOnStrip(rsMatchedPoint, leftSucc, stripWidth, std::greater<int>());

        // checking are on left pred image
        FeaturePoint lpMatchedPoint = findMatchesOnArea(lsMatchedPoint, leftPred, areaWidth);

        if (lpPoint.uniqueIdentifier == lpMatchedPoint.uniqueIdentifier) {
            matched.emplace_back(lpMatchedPoint, rpMatchedPoint, rsMatchedPoint, lsMatchedPoint);
        }

        
    }

}
