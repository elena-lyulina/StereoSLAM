#pragma once

#include "FeatureMatcher.h"
#include <string>
class DepthGetter
{
    public:
    DepthGetter (parameters param);
    void getDepths (std::vector<fp_tuple> *matchedPoints, std::string file, cv::Mat rgbImage);
    void showDisparity (std::vector<fp_tuple> *matchedPoints);

    parameters param;
};
