//
// Created by elena on 20.03.19.
//

#include "FeaturePoint.h"

FeaturePoint::FeaturePoint (Frame frame, int row, int col)
: descriptor (vector<int> ()), row (row), col (col)
{

    //    .  .  .  .  #  .  #  .  .  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    #  .  .  .  .  .  .  .  .  .  #
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  #  .  #  .  #  .  #  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  #  .  #  .  #  .  #  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    #  .  .  .  .  .  .  .  .  .  #
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  .  .  #  .  #  .  .  .  .

    vector<Mat> sobels = { frame.getXSobelConvolution (), frame.getYSobelConvolution () };
    for (auto s : sobels)
    {
        descriptor.push_back (s.at<uchar> (row - 5, col - 1));
        descriptor.push_back (s.at<uchar> (row - 5, col + 1));

        descriptor.push_back (s.at<uchar> (row - 3, col - 5));
        descriptor.push_back (s.at<uchar> (row - 3, col + 5));

        descriptor.push_back (s.at<uchar> (row - 1, col - 3));
        descriptor.push_back (s.at<uchar> (row - 1, col - 1));
        descriptor.push_back (s.at<uchar> (row - 1, col + 1));
        descriptor.push_back (s.at<uchar> (row - 1, col + 3));

        descriptor.push_back (s.at<uchar> (row + 1, col - 3));
        descriptor.push_back (s.at<uchar> (row + 1, col - 1));
        descriptor.push_back (s.at<uchar> (row + 1, col + 1));
        descriptor.push_back (s.at<uchar> (row + 1, col + 3));

        descriptor.push_back (s.at<uchar> (row + 3, col - 5));
        descriptor.push_back (s.at<uchar> (row + 3, col + 5));

        descriptor.push_back (s.at<uchar> (row + 5, col - 1));
        descriptor.push_back (s.at<uchar> (row + 5, col + 1));
    }
}
