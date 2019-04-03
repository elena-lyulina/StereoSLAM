#include "Frame.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;


Frame::Frame (Mat &img)
: image (img), integralImage (Mat::zeros (image.rows + 1, image.cols + 1, CV_32S)),
  xSobel (Mat::zeros (image.rows, image.cols, CV_8U)), ySobel (Mat::zeros (image.rows, image.cols, CV_8U))
{
    fillSum ();
    getXSobelConvolution (image, xSobel);
    getYSobelConvolution (image, ySobel);
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

Mat &Frame::getBlobConvolution (Mat &result)
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

Mat &Frame::getCornerConvolution (Mat &result)
{
    //  (-1, -1,  0,  1,  1,   (1,
    //   -1, -1,  0,  1,  1,    1,
    //    0,  0,  0,  0,  0, =  0, * (-1, -1,  0,  1,  1)
    //    1,  1,  0, -1, -1,   -1,
    //    1,  1,  0, -1, -1 )  -1)


    Mat rowMult (result.rows, result.cols, CV_32S, Scalar::all (0));
    Mat colMult (result.rows, result.cols, CV_32S, Scalar::all (0));

    const int w = image.cols, h = image.rows;
    rowMult(Rect(0, 2, w, h - 4))
      = image(Rect(0, 0, image.cols, image.rows - 4))
  +image(Rect(0, 1, image.cols, image.rows - 4))
  -image(Rect(0, 3, image.cols, image.rows - 4))
  -image(Rect(0, 4, image.cols, image.rows - 4));

    colMult(Rect(2,2,image.cols-4, image.rows -4))
    =
        -rowMult(Rect(0, 2, image.cols-4, image.rows - 4))
            -rowMult(Rect(1, 2, image.cols-4, image.rows - 4))
            +rowMult(Rect(3, 2, image.cols-4, image.rows - 4))
            +rowMult(Rect(4, 2, image.cols-4, image.rows - 4));
    colMult.convertTo(result, CV_8U);
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


bool cmp (uchar a, uchar b, int cmp)
{
    return (cmp == 1) ? a > b : b > a;
}


Mat &Frame::getIntegralImage ()
{
    return integralImage;
}

Mat &Frame::getXSobelConvolution ()
{
    return xSobel;
}

Mat &Frame::getYSobelConvolution ()
{
    return ySobel;
}

Mat &Frame::getXSobelConvolution (Mat image, Mat &result)
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

Mat &Frame::getYSobelConvolution (Mat image, Mat &result)
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
void Frame::suppression2D (int n, Mat image, vector<pair<int, int>> &result, Comparator comp)
{
    for (int i = n; i < image.rows - n; i += n + 1)
    {
        for (int j = n; j < image.cols - n; j += n + 1)
        {

            int mi = i;
            int mj = j;
            bool equalFound = false;

            for (int i2 = i; i2 <= i + n; i2++)
            {
                for (int j2 = j; j2 <= j + n; j2++)
                {

                    if (comp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj)))
                    {

                        equalFound = false;
                        mi = i2;
                        mj = j2;
                    }
                    else if (image.at<uchar> (i2, j2) == image.at<uchar> (mi, mj) && (i2 != mi || j2 != mj))
                    {

                        equalFound = true;
                    }
                }
            }

            if (!equalFound && (mi < image.rows - n) && (mj < image.cols - n))
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

                if (geNotFound)
                {
                    result.emplace_back (pair<int, int> (mi, mj));
                }
            }
        }
    }
}

template void Frame::suppression2D (int n, Mat image, vector<pair<int, int>> &result, greater<int>);
template void Frame::suppression2D (int n, Mat image, vector<pair<int, int>> &result, less<int>);
