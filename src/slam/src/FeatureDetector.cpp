#include <FeatureDetector.h>
#include <util.h>

FeatureDetector::FeatureDetector (std::pair<Frame, Frame> &predFrames, std::pair<Frame, Frame> &succFrames)
: predFrames (&predFrames), succFrames (&succFrames)
{

    Frame *frames[FRAME_AMOUNT] = { &predFrames.first, &predFrames.second, &succFrames.second,
                                    &succFrames.first };

    std::vector<std::pair<int, int>> coord[FRAME_AMOUNT][TYPE_AMOUNT];

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        frames[i]->suppression2D (supprArea, frames[i]->getBlobConvolution (), coord[i][BLOB_MAX], coord[i][BLOB_MIN]);
        frames[i]->suppression2D (supprArea, frames[i]->getCornerConvolution (), coord[i][CORNER_MAX], coord[i][CORNER_MIN]);

        for (int k = 0; k < TYPE_AMOUNT; k++)
        {

            for (int j = 0; j < coord[i][k].size (); j++)
            {
                detectedPoints[i][k].emplace_back (
                FeaturePoint (*frames[i], coord[i][k][j].first, coord[i][k][j].second));
            }
        }
    }


    // todo: log it
    int allPoints = 0;
    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        allPoints = 0;
        std::cout << "BlobMax points: " << detectedPoints[i][BLOB_MAX].size();
        allPoints += detectedPoints[i][BLOB_MAX].size();

        std::cout << ", BlobMin points: " << detectedPoints[i][BLOB_MIN].size();
        allPoints += detectedPoints[i][BLOB_MIN].size();

        std::cout << ", CornerMax points: " << detectedPoints[i][CORNER_MAX].size();
        allPoints += detectedPoints[i][CORNER_MAX].size();

        std::cout << ", CornerMin points: " << detectedPoints[i][CORNER_MIN].size();
        allPoints += detectedPoints[i][CORNER_MIN].size();

        std::cout << ", all points: " << allPoints << "\n";
    }

}
std::vector<FeaturePoint> &FeatureDetector::getDetectedPoints (frameNumber frame, pointType type)
{
    return detectedPoints[frame][type];
}
