#include "FeatureDetector.h"

FeatureDetector::FeatureDetector (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames)
: predFrames (&predFrames), succFrames (&succFrames)
{

    int supprArea = 5;
    Frame *frames[FRAME_AMOUNT] = { &predFrames.first, &predFrames.second, &succFrames.second,
                                    &succFrames.first };

    std::vector<std::pair<int, int>> coord[FRAME_AMOUNT][TYPE_AMOUNT];

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
std::vector<FeaturePoint> &FeatureDetector::getDetectedPoints (int frame, int type)
{
    return detectedPoints[frame][type];
}
