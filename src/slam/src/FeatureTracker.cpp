#include <FeatureDetector.h>
#include <FeatureMatcher.h>
#include <FeatureTracker.h>
#include <opencv2/highgui.hpp>
#include <random>

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
    Frame *frames[FRAME_AMOUNT] = { &predFrames->first, &predFrames->second, &succFrames->second,
                                    &succFrames->first };

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        imagesRGB[i] =
        cv::Mat (frames[i]->getImage ().rows, frames[i]->getImage ().cols, CV_8UC3, cv::Scalar::all (0));
        cvtColor (frames[i]->getImage (), imagesRGB[i], cv::COLOR_GRAY2BGR);
    }
}


void FeatureTracker::showMPOnImages (cv::Mat *imagesRGB, pointType type)
{
    std::vector<fp_tuple> *points = getCyclicallyMatchedPoints ();
    std::random_device rd;
    std::mt19937 gen (rd ());
    std::uniform_int_distribution<> dis (10, 255);
    cv::Scalar color = cv::Scalar (dis (gen), dis (gen), dis (gen));
    for (int i = 0; i < points[type].size (); i++)
    {
        circle (imagesRGB[LEFT_PRED],
                cv::Point (std::get<LEFT_PRED> (points[type][i])->col,
                           std::get<LEFT_PRED> (points[type][i])->row),
                3, color, -1);

        circle (imagesRGB[RIGHT_PRED],
                cv::Point (std::get<RIGHT_PRED> (points[type][i])->col,
                           std::get<RIGHT_PRED> (points[type][i])->row),
                3, color, -1);

        circle (imagesRGB[RIGHT_SUCC],
                cv::Point (std::get<RIGHT_SUCC> (points[type][i])->col,
                           std::get<RIGHT_SUCC> (points[type][i])->row),
                3, color, -1);

        circle (imagesRGB[LEFT_SUCC],
                cv::Point (std::get<LEFT_SUCC> (points[type][i])->col,
                           std::get<LEFT_SUCC> (points[type][i])->row),
                3, color, -1);

        color = cv::Scalar (dis (gen), dis (gen), dis (gen));
    }

    for (int i = 0; i < FRAME_AMOUNT; i++)
    {
        namedWindow ("mp: image" + std::to_string (i), cv::WINDOW_AUTOSIZE);
        imshow ("mp: image" + std::to_string (i), imagesRGB[i]);
    }
}

void FeatureTracker::showNMPOnImages (cv::Mat *imagesRGB, pointType type, cv::Scalar color)
{
    for (int fn = LEFT_PRED; fn < FRAME_AMOUNT; fn++)
    {

        std::vector<FeaturePoint> &points = fd.getDetectedPoints (static_cast<frameNumber> (fn), type);

        for (int i = 0; i < points.size (); i++)

        {
            circle (imagesRGB[fn], cv::Point (points[i].col, points[i].row), 2, color, -1);
        }

        namedWindow ("nmp: image" + std::to_string (fn), cv::WINDOW_AUTOSIZE);
        imshow ("nmp: image" + std::to_string (fn), imagesRGB[fn]);
    }
}

void FeatureTracker::showMP (int typeAmount, pointType types[])
{
    cv::Mat imagesRGB[FRAME_AMOUNT];
    getImagesRGB (imagesRGB);
    for (int i = 0; i < typeAmount; i++)
    {
        showMPOnImages (imagesRGB, types[i]);
    }
}

void FeatureTracker::showNMP (int typeAmount, pointType types[])
{
    cv::Mat imagesRGB[FRAME_AMOUNT];
    getImagesRGB (imagesRGB);
    for (int i = 0; i < typeAmount; i++)
    {
        showNMPOnImages (imagesRGB, types[i], colors[i]);
    }
}
