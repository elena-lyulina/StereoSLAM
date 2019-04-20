#include <FeaturePoint.h>

FeaturePoint::FeaturePoint (Frame &frame, int row, int col)
: row (row), col (col), frame (frame), w (frame.getImage ().cols), h (frame.getImage ().rows)
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


    const int size = 2;
    cv::Mat sobels[size] = { frame.getXSobelConvolution (), frame.getYSobelConvolution () };
    matchingDescriptor[0] = (sobels[0].at<int> (row - 5, col - 1));
    int j = 0;
    int k = 0;
    for (int i = 0; i < size; i++)
    {
        matchingDescriptor[j] = (sobels[i].at<int> (row - 5, col - 1));
        matchingDescriptor[j + 1] = (sobels[i].at<int> (row - 5, col + 1));

        matchingDescriptor[j + 2] = (sobels[i].at<int> (row - 3, col - 5));
        matchingDescriptor[j + 3] = (sobels[i].at<int> (row - 3, col - 3));
        matchingDescriptor[j + 4] = (sobels[i].at<int> (row - 3, col - 1));
        matchingDescriptor[j + 5] = (sobels[i].at<int> (row - 3, col + 1));
        matchingDescriptor[j + 6] = (sobels[i].at<int> (row - 3, col + 3));
        matchingDescriptor[j + 7] = (sobels[i].at<int> (row - 3, col + 5));

        matchingDescriptor[j + 8] = (sobels[i].at<int> (row - 1, col - 3));
        matchingDescriptor[j + 9] = (sobels[i].at<int> (row - 1, col - 1));
        matchingDescriptor[j + 10] = (sobels[i].at<int> (row - 1, col + 1));
        matchingDescriptor[j + 11] = (sobels[i].at<int> (row - 1, col + 3));

        matchingDescriptor[j + 12] = (sobels[i].at<int> (row + 1, col - 3));
        matchingDescriptor[j + 13] = (sobels[i].at<int> (row + 1, col - 1));
        matchingDescriptor[j + 14] = (sobels[i].at<int> (row + 1, col + 1));
        matchingDescriptor[j + 15] = (sobels[i].at<int> (row + 1, col + 3));

        matchingDescriptor[j + 16] = (sobels[i].at<int> (row + 3, col - 5));
        matchingDescriptor[j + 17] = (sobels[i].at<int> (row + 3, col - 3));
        matchingDescriptor[j + 18] = (sobels[i].at<int> (row + 3, col - 1));
        matchingDescriptor[j + 19] = (sobels[i].at<int> (row + 3, col + 1));
        matchingDescriptor[j + 20] = (sobels[i].at<int> (row + 3, col + 3));
        matchingDescriptor[j + 21] = (sobels[i].at<int> (row + 3, col + 5));

        matchingDescriptor[j + 22] = (sobels[i].at<int> (row + 5, col - 1));
        matchingDescriptor[j + 23] = (sobels[i].at<int> (row + 5, col + 1));

        j += 24;


        refinementDescriptor[k] = (sobels[i].at<int> (row - 2, col));

        refinementDescriptor[k + 1] = (sobels[i].at<int> (row - 1, col - 2));
        refinementDescriptor[k + 2] = (sobels[i].at<int> (row - 1, col - 1));
        refinementDescriptor[k + 3] = (sobels[i].at<int> (row - 1, col));
        refinementDescriptor[k + 4] = (sobels[i].at<int> (row - 1, col + 1));
        refinementDescriptor[k + 5] = (sobels[i].at<int> (row - 1, col + 2));

        refinementDescriptor[k + 6] = (sobels[i].at<int> (row, col - 1));
        refinementDescriptor[k + 7] = (sobels[i].at<int> (row, col));
        refinementDescriptor[k + 8] = (sobels[i].at<int> (row, col + 1));

        refinementDescriptor[k + 9] = (sobels[i].at<int> (row + 1, col - 2));
        refinementDescriptor[k + 10] = (sobels[i].at<int> (row + 1, col - 1));
        refinementDescriptor[k + 11] = (sobels[i].at<int> (row + 1, col));
        refinementDescriptor[k + 12] = (sobels[i].at<int> (row + 1, col + 1));
        refinementDescriptor[k + 13] = (sobels[i].at<int> (row + 1, col + 2));

        refinementDescriptor[k + 14] = (sobels[i].at<int> (row + 2, col));

        k += 15;
    }
}
