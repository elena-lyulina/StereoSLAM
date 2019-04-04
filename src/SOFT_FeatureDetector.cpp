#include "SOFT_FeatureDetector.h"
#include <opencv/cv.hpp>


SOFT_FeatureDetector::SOFT_FeatureDetector (pair<Frame, Frame> &predFrames, pair<Frame, Frame> &succFrames)
: predFrames (&predFrames), succFrames (&succFrames)
{

    int supprArea = 10;
    Frame *frames[FRAME_AMOUNT] = { &predFrames.first, &predFrames.second, &succFrames.second,
                                    &succFrames.first };

    vector<pair<int, int>> coord[FRAME_AMOUNT][TYPE_AMOUNT];

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        frames[i]->suppression2D (supprArea, frames[i]->getBlobConvolution (), coord[i][0], coord[i][1]);
        frames[i]->suppression2D (supprArea, frames[i]->getCornerConvolution (), coord[i][2], coord[i][3]);

        for (int k = 0; k < TYPE_AMOUNT; k++)
        {
            for (int j = 0; j < coord[i][k].size (); j++)
            {
                detectedPoints[i][k].emplace_back (
                FeaturePoint (*frames[i], coord[i][k][j].first, coord[i][k][j].second));
            }
        }
    }
}
vector<FeaturePoint> &SOFT_FeatureDetector::getDetectedPoints (int frame, int type)
{
    return detectedPoints[frame][type];
}
