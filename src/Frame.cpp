#include "Frame.h"
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;


Frame::Frame (Mat &img)
: image (img), integralImage (Mat::zeros (image.rows + 1, image.cols + 1, CV_32S)),
  xSobel (Mat::zeros (image.rows, image.cols, CV_8U)),
  ySobel (Mat::zeros (image.rows, image.cols, CV_8U)),
  blobConvolution (Mat::zeros (image.rows, image.cols, CV_8U)),
  cornerConvolution (Mat::zeros (image.rows, image.cols, CV_8U))
{
    fillSum ();
    doXSobelConvolution (image, xSobel);
    doYSobelConvolution (image, ySobel);
    doBlobConvolution (blobConvolution);
    doCornerConvolution (cornerConvolution);
}

Frame::Frame (const char *filename)
{
    Mat img = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
    *this = Frame (img);
}

void Frame::fillSum ()
{
    uint current_line_sum = 0;

    // assuming that the first column and raw are filled with zeros
    // fill other rows, using sum of previous rows
    for (int i = 0; i < image.rows; i++)
    {
        current_line_sum = 0;
        for (int j = 0; j < image.cols; j++)
        {
            current_line_sum += image.at<uchar> (i, j);
            integralImage.at<uint> (i + 1, j + 1) = current_line_sum + integralImage.at<uint> (i, j + 1);
        }
    }
}


int Frame::rectSum (int x, int y, int width, int height)
{
    return integralImage.at<uint> (y + height, x + width) - integralImage.at<uint> (y + height, x) -
           integralImage.at<uint> (y, x + width) + integralImage.at<uint> (y, x);
}


Mat &Frame::rectSumMat (int sideLength, Mat &result)
{

    int width = integralImage.cols - sideLength;
    int height = integralImage.rows - sideLength;
    int n = sideLength / 2;

    result = integralImage (Rect (sideLength, sideLength, width, height)) -
             integralImage (Rect (0, sideLength, width, height)) +
             integralImage (Rect (0, 0, width, height)) -
             integralImage (Rect (sideLength, 0, width, height));

    copyMakeBorder (result, result, n, n, n, n, BORDER_CONSTANT);


    return result;
}

Mat &Frame::doBlobConvolution (Mat &result)
{
    //  (-1, -1, -1, -1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1,  1,  8,  1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1, -1, -1, -1, -1 )

    int n = 2;

    Mat temp, temp1, temp2;
    image.convertTo (temp2, CV_32S);
    result = 2 * rectSumMat (n + 1, temp) - rectSumMat (2 * n + 1, temp1) + 7 * temp2;
    cv::rectangle (result, cv::Rect (cv::Point (0, 0), result.size ()), Scalar::all (0), n);
    result.convertTo (result, CV_8U);

    return result;
}

Mat &Frame::doCornerConvolution (Mat &result)
{
    //  (-1, -1,  0,  1,  1,   (1,
    //   -1, -1,  0,  1,  1,    1,
    //    0,  0,  0,  0,  0, =  0, * (-1, -1,  0,  1,  1)
    //    1,  1,  0, -1, -1,   -1,
    //    1,  1,  0, -1, -1 )  -1)


    Mat rowMult (result.rows, result.cols, CV_32S, Scalar::all (0));
    Mat colMult (result.rows, result.cols, CV_32S, Scalar::all (0));

    const int w = image.cols, h = image.rows;
    rowMult (Rect (0, 2, w, h - 4)) =
    image (Rect (0, 0, image.cols, image.rows - 4)) + image (Rect (0, 1, image.cols, image.rows - 4)) -
    image (Rect (0, 3, image.cols, image.rows - 4)) - image (Rect (0, 4, image.cols, image.rows - 4));

    colMult (Rect (2, 2, image.cols - 4, image.rows - 4)) =
    -rowMult (Rect (0, 2, image.cols - 4, image.rows - 4)) -
    rowMult (Rect (1, 2, image.cols - 4, image.rows - 4)) +
    rowMult (Rect (3, 2, image.cols - 4, image.rows - 4)) +
    rowMult (Rect (4, 2, image.cols - 4, image.rows - 4));
    colMult.convertTo (result, CV_8U);
#if 0
    for (int i = 2; i < image.rows - 2; i++)
    {
        add (image.row (i - 2), image.row (i - 1), rowMult.row (i));
        subtract (rowMult.row (i), image.row (i + 1), rowMult.row (i));
        subtract (rowMult.row (i), image.row (i + 2), rowMult.row (i));
    }

    for (int i = 2; i < image.cols - 2; i++)
    {
        add (rowMult.col (i + 2), rowMult.col (i + 1), colMult.col (i));
        subtract (colMult.col (i), rowMult.col (i - 1), colMult.col (i));
        subtract (colMult.col (i), rowMult.col (i - 2), result.col (i));
    }
#endif
    return result;
}


const Mat &Frame::getImage ()
{
    return image;
}

const Mat &Frame::getIntegralImage ()
{
    return integralImage;
}

const Mat &Frame::getXSobelConvolution ()
{
    return xSobel;
}

const Mat &Frame::getYSobelConvolution ()
{
    return ySobel;
}

const Mat &Frame::getBlobConvolution ()
{
    return blobConvolution;
}

const Mat &Frame::getCornerConvolution ()
{
    return cornerConvolution;
}

Mat &Frame::doXSobelConvolution (Mat image, Mat &result)
{
    // x Sobel kernel
    // -1 0 1     [1]
    // -2 0 2  =  [2] * [-1, 0, 1]
    // -1 0 1     [1]

    Mat rowMult (result.rows, result.cols, CV_16S, Scalar::all (0));
    for (int i = 1; i < image.cols - 1; i++)
    {
        subtract (image.col (i + 1), image.col (i - 1), rowMult.col (i));
    }

    Mat temp (1, result.cols, CV_16S, Scalar::all (0));
    for (int i = 1; i < rowMult.rows - 1; i++)
    {
        add (2 * rowMult.row (i), rowMult.row (i - 1), temp);
        add (temp, rowMult.row (i + 1), result.row (i));
    }

    return result;
}

Mat &Frame::doYSobelConvolution (Mat image, Mat &result)
{
    // y Sobel kernel
    //  1  2  1     [1]
    //  0  0  0  =  [0] * [1, 2, 1]
    // -1 -2 -1     [-1]

    Mat rowMult (result.rows, result.cols, CV_16S, Scalar::all (0));
    Mat temp (result.rows, 1, CV_16S, Scalar::all (0));

    for (int i = 1; i < image.rows - 1; i++)
    {
        subtract (image.row (i - 1), image.row (i + 1), rowMult.row (i));
    }

    for (int i = 1; i < rowMult.cols - 1; i++)
    {
        add (2 * rowMult.col (i), rowMult.col (i - 1), temp);
        add (temp, rowMult.col (i + 1), result.col (i));
    }

    return result;
}

template <typename Comparator>
bool equalOrCompNotFound (int n, const Mat &image, int mi, int mj, int i, int j, Comparator comp)
{
    bool geNotFound = true;

    for (int i2 = mi - n; geNotFound && i2 <= mi + n && i2 < image.rows; i2++)
    {
        for (int j2 = mj - n; geNotFound && j2 < j && j2 < image.cols; j2++)
        {

            if (comp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj)) ||
                image.at<uchar> (i2, j2) == image.at<uchar> (mi, mj))
            {
                geNotFound = false;
            }
        }
        for (int j2 = j + n + 1; geNotFound && j2 <= mj + n && j2 < image.cols; j2++)
        {

            if (comp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj)) ||
                image.at<uchar> (i2, j2) == image.at<uchar> (mi, mj))
            {
                geNotFound = false;
            }
        }
    }

    for (int j2 = j; geNotFound && j2 <= j + n && j2 < image.cols; j2++)
    {
        for (int i2 = mi - n; geNotFound && i2 < i && i2 < image.rows; i2++)
        {

            if (comp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj)) ||
                image.at<uchar> (i2, j2) == image.at<uchar> (mi, mj))
            {
                geNotFound = false;
            }
        }

        for (int i2 = i + n + 1; geNotFound && i2 <= mi + n && i2 < image.rows; i2++)
        {

            if (comp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj)) ||
                image.at<uchar> (i2, j2) == image.at<uchar> (mi, mj))
            {
                geNotFound = false;
            }
        }
    }

    return geNotFound;
}

void Frame::suppression2D (int n, const Mat &image, vector<pair<int, int>> &maxResult, vector<pair<int, int>> &minResult)
{


    // looking for max/min value into [n x n] image areas, which top left corner is represented by (i, j) coord
    for (int i = n; i < image.rows - n; i += n + 1)
    {
        for (int j = n; j < image.cols - n; j += n + 1)
        {
            int maxi = i;
            int maxj = j;

            int mini = i;
            int minj = j;

            bool equalMaxFound = false;
            bool equalMinFound = false;


            for (int i2 = i; i2 <= i + n; i2++)
            {
                for (int j2 = j; j2 <= j + n; j2++)
                {

                    if (image.at<uchar> (i2, j2) > image.at<uchar> (maxi, maxj))
                    {

                        equalMaxFound = false;
                        maxi = i2;
                        maxj = j2;
                    }
                    else if (image.at<uchar> (i2, j2) == image.at<uchar> (maxi, maxj) &&
                             (i2 != maxi || j2 != maxj))
                    {
                        equalMaxFound = true;
                    }

                    if (image.at<uchar> (i2, j2) < image.at<uchar> (mini, minj))
                    {
                        equalMinFound = false;
                        mini = i2;
                        minj = j2;
                    }
                    else if (image.at<uchar> (i2, j2) == image.at<uchar> (mini, minj) &&
                             (i2 != mini || j2 != minj))
                    {
                        equalMinFound = true;
                    }
                }
            }

            // if max was found, check if there is no another max or equal values outside of
            // observed [n x n] area and only then it declares as local max
            if ((!equalMaxFound && (maxi < image.rows - n) && (maxj < image.cols - n)))
            {
                if (equalOrCompNotFound (n, image, maxi, maxj, i, j, greater<int> ()))
                {
                    maxResult.emplace_back (pair<int, int> (maxi, maxj));
                }
            }

            // if min was found, check if there is no another min or equal values outside of
            // observed [n x n] area and only then it declares as local min
            if ((!equalMinFound && (mini < image.rows - n) && (minj < image.cols - n)))
            {
                if (equalOrCompNotFound (n, image, mini, minj, i, j, less<int> ()))
                {
                    minResult.emplace_back (pair<int, int> (mini, minj));
                }
            }
        }
    }
}