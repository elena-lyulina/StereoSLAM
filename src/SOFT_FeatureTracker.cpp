#include "SOFT_FeatureTracker.h"
#include "SOFT_FeatureDetector.h"
#include "SOFT_FeatureMatcher.h"
#include <opencv2/highgui.hpp>


void SOFT_FeatureTracker::handleNextFrames (pair<Frame, Frame> &frames)
{
    predFrames = succFrames;
    succFrames = &frames;

    fd = SOFT_FeatureDetector (*predFrames, *succFrames);
    fm = SOFT_FeatureMatcher (fd);
}

vector<fp_tuple> *SOFT_FeatureTracker::getCyclicallyMatchedPoints ()
{
    return fm.getMatchedPointsAllTypes ();
}

void SOFT_FeatureTracker::getImagesRGB (Mat *imagesRGB)
{
    Frame *frames[FRAME_AMOUNT] = { &predFrames->first, &predFrames->second, &succFrames->first,
                                    &succFrames->second };

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        imagesRGB[i] =
        Mat (frames[i]->getImage ().rows, frames[i]->getImage ().cols, CV_8UC3, Scalar::all (0));
        cvtColor (frames[i]->getImage (), imagesRGB[i], COLOR_GRAY2BGR);
    }
}


void SOFT_FeatureTracker::showMPOnImages (Mat *imagesRGB, int type)
{
    vector<fp_tuple> *points = getCyclicallyMatchedPoints ();
    RNG rng (12345);
    Scalar color = Scalar (rng.uniform (10, 255), rng.uniform (10, 255), rng.uniform (10, 255));
    for (int i = 0; i < points[type].size (); i++)
    {
        circle (imagesRGB[0], Point (get<0> (points[type][i])->col, get<0> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[1], Point (get<1> (points[type][i])->col, get<1> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[2], Point (get<2> (points[type][i])->col, get<2> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[3], Point (get<3> (points[type][i])->col, get<3> (points[type][i])->row),
                3, color, -1);

        color = Scalar (rng.uniform (10, 255), rng.uniform (10, 255), rng.uniform (10, 255));
    }

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        namedWindow ("image" + to_string (i), WINDOW_AUTOSIZE);
        imshow ("image" + to_string (i), imagesRGB[i]);
    }

    waitKey ();
}

void SOFT_FeatureTracker::showMP (int typeAmount, pointTypes types[])
{
    Mat imagesRGB[FRAME_AMOUNT];
    getImagesRGB (imagesRGB);
    for (int i = 0; i < typeAmount; i++)
    {
        showMPOnImages (imagesRGB, types[i]);
    }
}
