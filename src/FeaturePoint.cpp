//
// Created by elena on 20.03.19.
//

#include "FeaturePoint.h"

FeaturePoint::FeaturePoint (Frame &frame, int row, int col)
: row (row), col (col), frame (frame)
{
    // Matching patch:
    //    .  .  .  .  #  .  #  .  .  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    #  .  #  .  #  .  #  .  #  .  #
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  #  .  #  .  #  .  #  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  #  .  #  .  #  .  #  .  .
    //    .  .  .  .  .  .  .  .  .  .  .
    //    #  .  #  .  #  .  #  .  #  .  #
    //    .  .  .  .  .  .  .  .  .  .  .
    //    .  .  .  .  #  .  #  .  .  .  .

    // Refinement patch:
    //    .  .  #  .  .
    //    #  #  #  #  #
    //    .  #  #  #  .
    //    #  #  #  #  #
    //    .  .  #  .  .

    Mat sobels[2] = { frame.getXSobelConvolution (), frame.getYSobelConvolution () };
    int i = 1;
    for (auto s : sobels)
    {
        matchingDescriptor[i * 0] = (s.at<uchar> (row - 5, col - 1));
        matchingDescriptor[i * 1] = (s.at<uchar> (row - 5, col + 1));

        matchingDescriptor[i * 2] = (s.at<uchar> (row - 3, col - 5));
        matchingDescriptor[i * 3] = (s.at<uchar> (row - 3, col - 3));
        matchingDescriptor[i * 4] = (s.at<uchar> (row - 3, col - 1));
        matchingDescriptor[i * 5] = (s.at<uchar> (row - 3, col + 1));
        matchingDescriptor[i * 6] = (s.at<uchar> (row - 3, col + 3));
        matchingDescriptor[i * 7] = (s.at<uchar> (row - 3, col + 5));

        matchingDescriptor[i * 8] = (s.at<uchar> (row - 1, col - 3));
        matchingDescriptor[i * 9] = (s.at<uchar> (row - 1, col - 1));
        matchingDescriptor[i * 10] = (s.at<uchar> (row - 1, col + 1));
        matchingDescriptor[i * 11] = (s.at<uchar> (row - 1, col + 3));

        matchingDescriptor[i * 12] = (s.at<uchar> (row + 1, col - 3));
        matchingDescriptor[i * 13] = (s.at<uchar> (row + 1, col - 1));
        matchingDescriptor[i * 14] = (s.at<uchar> (row + 1, col + 1));
        matchingDescriptor[i * 15] = (s.at<uchar> (row + 1, col + 3));

        matchingDescriptor[i * 16] = (s.at<uchar> (row + 3, col - 5));
        matchingDescriptor[i * 17] = (s.at<uchar> (row + 3, col - 3));
        matchingDescriptor[i * 18] = (s.at<uchar> (row + 3, col - 1));
        matchingDescriptor[i * 19] = (s.at<uchar> (row + 3, col + 1));
        matchingDescriptor[i * 20] = (s.at<uchar> (row + 3, col + 3));
        matchingDescriptor[i * 21] = (s.at<uchar> (row + 3, col + 5));

        matchingDescriptor[i * 22] = (s.at<uchar> (row + 5, col - 1));
        matchingDescriptor[i * 23] = (s.at<uchar> (row + 5, col + 1));


        refinementDescriptor[i * 0] = (s.at<uchar> (row - 2, col));

        refinementDescriptor[i * 1] = (s.at<uchar> (row - 1, col - 2));
        refinementDescriptor[i * 2] = (s.at<uchar> (row - 1, col - 1));
        refinementDescriptor[i * 3] = (s.at<uchar> (row - 1, col));
        refinementDescriptor[i * 4] = (s.at<uchar> (row - 1, col + 1));
        refinementDescriptor[i * 5] = (s.at<uchar> (row - 1, col + 2));

        refinementDescriptor[i * 6] = (s.at<uchar> (row, col - 1));
        refinementDescriptor[i * 7] = (s.at<uchar> (row, col));
        refinementDescriptor[i * 8] = (s.at<uchar> (row, col + 1));

        refinementDescriptor[i * 9] = (s.at<uchar> (row + 1, col - 2));
        refinementDescriptor[i * 10] = (s.at<uchar> (row + 1, col - 1));
        refinementDescriptor[i * 11] = (s.at<uchar> (row + 1, col));
        refinementDescriptor[i * 12] = (s.at<uchar> (row + 1, col + 1));
        refinementDescriptor[i * 13] = (s.at<uchar> (row + 1, col + 2));

        refinementDescriptor[i * 14] = (s.at<uchar> (row + 2, col));

        i++;
    }
}
