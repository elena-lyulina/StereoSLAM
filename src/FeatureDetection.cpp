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
                cout << "size " << matched.size() << endl;
            }
        }
    }
}
