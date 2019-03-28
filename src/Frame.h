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
  explicit Frame (Mat &img);
  Mat &getIntegralImage ();
  Mat &getXSobelConvolution ();
  Mat &getYSobelConvolution ();
  Mat &getBlobConvolution (Mat &result);
  Mat &getCornerConvolution (Mat &result);
  static Mat &getXSobelConvolution (Mat image, Mat &result);
  static Mat &getYSobelConvolution (Mat image, Mat &result);
  int rectSum (int x, int y, int width, int height);


  /*
   Founded on (2n + 1) × (2n + 1)-Block Algorithm from "Efficient Non-Maximum Suppression",
   Alexander Neubeck, Luc Van Gool;
   if comp == 1, then it found local maximum, else - local minimum
   fills vector of pairs(i, j), where i stands for row coordinate, j - for col coordinate
   */
  template <typename Comparator>
  static void suppression2D (int n, Mat image, vector<pair<int, int>> &result, Comparator comp);



 private:
  Mat image;
  Mat integralImage;
  Mat xSobel;
  Mat ySobel;

  /*
    Counting sum of squared pixels intensities, starting from the top left corner (taken as (0,0)
    coordinate); The sum of pixels intensities, placed between (0,0) and (i,j) is stored at (i,j)
    coordinates in integralImage mat, assuming that integralImage mat has the same size as image
   */
  void fillSum ();

  Mat &rectSumMat(int sideLength, Mat &result);
};

#endif // STEREOSLAM_FRAME_H
