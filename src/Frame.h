#include <iostream>
#include <opencv2/imgproc.hpp>
#ifndef STEREOSLAM_FRAME_H
#define STEREOSLAM_FRAME_H

#pragma once

using namespace std;
using namespace cv;

class Frame
{
public:
    Frame (Mat &img);
    Mat &getIntegralImage ();
    Mat &getBlobConvolution(Mat &result);
    Mat &getCornerConvolution(Mat &result);
    Mat &getXSobelConvolution(Mat image, Mat &result);
    Mat &getYSobelConvolution(Mat image, Mat &result);
    int rectSum (int x, int y, int width, int height);
    int compPartialMax (int from, int to, int *I, int *pmax);
    void suppression1D (int n, int *I, int size, vector<int> &maximum);
    void suppression2D (int n, Mat image, vector<pair<int, int>> &maximum, int comp);


private:
    Mat image;
    Mat integralImage;

    /*
     * Counting sum of squared pixels intensities, starting from the top left corner (taken as (0,0)
     * coordinate); The sum of pixels intensities, placed between (0,0) and (i,j) is stored at (i,j)
     * coordinates in integralImage mat, assuming that integralImage mat has the same size as image
     * todo: check size of result mat? find better solution?
     * todo: find out comment style in c++
     */
    void fillSum ();
};

#endif // STEREOSLAM_FRAME_H
