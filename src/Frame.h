#pragma once

#include <iostream>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

class Frame
{
 public:
  Frame (Mat &img);
  Frame (const char *filename);
  const Mat &getImage();
  const Mat &getIntegralImage ();
  const Mat &getXSobelConvolution ();
  const Mat &getYSobelConvolution ();
  const Mat &getBlobConvolution ();
  const Mat &getCornerConvolution ();

  static Mat &doXSobelConvolution(Mat image, Mat &result);
  static Mat &doYSobelConvolution(Mat image, Mat &result);
  int rectSum (int x, int y, int width, int height);

  Mat &doBlobConvolution(Mat &result);
  Mat &doCornerConvolution(Mat &result);


  /*
   Founded on (2n + 1) × (2n + 1)-Block Algorithm from "Efficient Non-Maximum Suppression",
   Alexander Neubeck, Luc Van Gool;
   if comp == 1, then it found local maximum, else - local minimum
   fills vector of pairs(i, j), where i stands for row's coordinate, j - for col's coordinate
   */
  template <typename Comparator>
  static void suppression2D (int n, Mat image, vector<pair<int, int>> &result, Comparator comp);


 private:
  Mat image;
  Mat integralImage;
  Mat xSobel;
  Mat ySobel;
  Mat blobConvolution;
  Mat cornerConvolution;

  /*
    Counting sum of squared pixels intensities, starting from the top left corner (taken as (0,0)
    coordinate); The sum of pixels intensities, placed between (0,0) and (i,j) is stored at (i,j)
    coordinates in integralImage mat, assuming that integralImage mat has the same size as image
   */
  void fillSum ();



  Mat &rectSumMat (int sideLength, Mat &result);
};

