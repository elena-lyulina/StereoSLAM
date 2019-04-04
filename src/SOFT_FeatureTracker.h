#pragma once

#include "Frame.h"
#include "SOFT_FeatureDetector.h"
#include "SOFT_FeatureMatcher.h"


class SOFT_FeatureTracker
{
    public:
    SOFT_FeatureTracker (pair<Frame, Frame> &predFrames, pair<Frame, Frame> &succFrames)
    : predFrames (&predFrames), succFrames (&succFrames),
      fd (SOFT_FeatureDetector (predFrames, succFrames)), fm (SOFT_FeatureMatcher (fd)){};

    void handleNextFrames (pair<Frame, Frame> &frames);
    vector<fp_tuple> *getCyclicallyMatchedPoints ();
    void showMP (int typeAmount, pointTypes types[]);

    private:
    pair<Frame, Frame> *predFrames;
    pair<Frame, Frame> *succFrames;

    SOFT_FeatureDetector fd;
    SOFT_FeatureMatcher fm;

    void showMPOnImages (Mat *imagesRGB, int type);
    void getImagesRGB (Mat *imagesRGB);
};
