#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

class Frame
{
    public:
    Frame (cv::Mat &img);
    Frame (const char *filename);
    const cv::Mat &getImage ();
    const cv::Mat &getIntegralImage ();
    const cv::Mat &getXSobelConvolution ();
    const cv::Mat &getYSobelConvolution ();
    const cv::Mat &getBlobConvolution ();
    const cv::Mat &getCornerConvolution ();

    enum sobelSize
    {
        SS_3 = 3,
        SS_5 = 5
    };

    static cv::Mat &doXSobelConvolution (cv::Mat image, cv::Mat &result, sobelSize size);
    static cv::Mat &doYSobelConvolution (cv::Mat image, cv::Mat &result, sobelSize size);


    int rectSum (int x, int y, int width, int height);

    cv::Mat &doBlobConvolution (cv::Mat &result);
    cv::Mat &doCornerConvolution (cv::Mat &result);


    /*
     Founded on (2n + 1) × (2n + 1)-Block Algorithm from "Efficient Non-Maximum Suppression",
     Alexander Neubeck, Luc Van Gool;
     if comp == 1, then it found local maximum, else - local minimum
     fills vector of pairs(i, j), where i stands for row's coordinate, j - for col's coordinate
     */

    static void suppression2D (int n,
                               const cv::Mat &image,
                               std::vector<std::pair<int, int>> &maxResult,
                               std::vector<std::pair<int, int>> &minResult);


    private:
    cv::Mat image;
    cv::Mat integralImage;
    cv::Mat xSobel;
    cv::Mat ySobel;
    cv::Mat blobConvolution;
    cv::Mat cornerConvolution;

    /*
      Counting sum of squared pixels intensities, starting from the top left corner (taken as (0,0)
      coordinate); The sum of pixels intensities, placed between (0,0) and (i,j) is stored at (i,j)
      coordinates in integralImage cv::Mat, assuming that integralImage cv::Mat has the same size as image
     */
    void fillSum ();


    cv::Mat &rectSumMat (int sideLength, cv::Mat &result);
};
