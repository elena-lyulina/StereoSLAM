#include "FeatureTracker.h"
#include "FeatureDetector.h"
#include "FeatureMatcher.h"
#include <opencv2/highgui.hpp>


void FeatureTracker::handleNextFrames (std::pair<Frame, Frame> &frames)
{
    predFrames = succFrames;
    succFrames = &frames;

    fd = FeatureDetector (*predFrames, *succFrames);
    fm = FeatureMatcher (fd);
}

std::vector<fp_tuple> *FeatureTracker::getCyclicallyMatchedPoints ()
{
    return fm.getMatchedPointsAllTypes ();
}

void FeatureTracker::getImagesRGB (cv::Mat *imagesRGB)
{
    Frame *frames[FRAME_AMOUNT] = { &predFrames->first, &predFrames->second, &succFrames->first,
                                    &succFrames->second };

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        imagesRGB[i] =
        cv::Mat (frames[i]->getImage ().rows, frames[i]->getImage ().cols, CV_8UC3, cv::Scalar::all (0));
        cvtColor (frames[i]->getImage (), imagesRGB[i], cv::COLOR_GRAY2BGR);
    }
}


void FeatureTracker::showMPOnImages (cv::Mat *imagesRGB, int type)
{
    std::vector<fp_tuple> *points = getCyclicallyMatchedPoints ();
    cv::RNG rng (12345);
    cv::Scalar color = cv::Scalar (rng.uniform (10, 255), rng.uniform (10, 255), rng.uniform (10, 255));
    for (int i = 0; i < points[type].size (); i++)
    {
        circle (imagesRGB[0],
                cv::Point (std::get<0> (points[type][i])->col, std::get<0> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[1],
                cv::Point (std::get<1> (points[type][i])->col, std::get<1> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[2],
                cv::Point (std::get<2> (points[type][i])->col, std::get<2> (points[type][i])->row),
                3, color, -1);
        circle (imagesRGB[3],
                cv::Point (std::get<3> (points[type][i])->col, std::get<3> (points[type][i])->row),
                3, color, -1);

        color = cv::Scalar (rng.uniform (10, 255), rng.uniform (10, 255), rng.uniform (10, 255));
    }

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        namedWindow ("image" + std::to_string (i), cv::WINDOW_AUTOSIZE);
        imshow ("image" + std::to_string (i), imagesRGB[i]);
    }

    cv::waitKey ();
}

void FeatureTracker::showMP (int typeAmount, pointTypes types[])
{
    cv::Mat imagesRGB[FRAME_AMOUNT];
    getImagesRGB (imagesRGB);
    for (int i = 0; i < typeAmount; i++)
    {
        showMPOnImages (imagesRGB, types[i]);
    }
}
