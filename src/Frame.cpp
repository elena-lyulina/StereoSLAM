#include "Frame.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <vector> //for std::vector

using namespace std;
using namespace cv;


Frame::Frame (Mat &img)
: image (img), integralImage (Mat::zeros (image.rows + 1, image.cols + 1, CV_32SC1))
{
    fillSum ();
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

uchar saturation (int a)
{
    if (a > 255) return 255;
    if (a < 0) return 0;

    return (uchar)a;
}


Mat &Frame::getBlobConvolution(Mat &result)
{
    //  (-1, -1, -1, -1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1,  1,  8,  1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1, -1, -1, -1, -1 )
    Mat_<uchar> result_ = result;
    int n = 2;
    for (int i = n; i < image.rows - n; i++)
    {
        for (int j = n; j < image.cols - n; j++)
        {
            result_[i][j] = saturation (-rectSum (j - n, i - n, 2 * n + 1, 2 * n + 1) +
                                        2 * rectSum (j - n + 1, i - n + 1, 2 * n - 1, 2 * n - 1) +
                                        7 * image.at<uchar> (i, j));
        }
    }
    result = result_;
    return result;
}

Mat &Frame::getCornerConvolution(Mat &result)
{
    //  (-1, -1,  0,  1,  1,
    //   -1, -1,  0,  1,  1,
    //    0,  0,  0,  0,  0,
    //    1,  1,  0, -1, -1,
    //    1,  1,  0, -1, -1 )

    Mat_<uchar> result_ = result;
    int n = 2;
    for (int i = n; i < image.rows - n; i++)
    {
        for (int j = n; j < image.cols - n; j++)
        {
            result_[i][j] = saturation (-rectSum (j - n, i - n, n, n) + rectSum (j - n, i + 1, n, n) +
                                        rectSum (j + 1, i - n, n, n) - rectSum (j + 1, i + 1, n, n));
        }
    }
    result = result_;
    return result;
}


int Frame::compPartialMax (int from, int to, int *I, int *pmax)
{
    pmax[to] = I[to];
    int best = to;
    while (to > from)
    {
        to--;
        if (I[best] >= I[to])
        {
            pmax[to] = I[best];
        }
        else
        {
            pmax[to] = I[to];
            best = to;
        }
    }
    return best;
}


// currently unused
void Frame::suppression1D (int n, int *I, int size, vector<int> &maximum)
{
    int pmax[size];
    int i = n;
    compPartialMax (0, i - 1, I, pmax);
    int chkpt = -1;
    while (i < size - 2 * n)
    {
        cout << "hello" << endl;
        int j = compPartialMax (i, i + n, I, pmax);
        int k = compPartialMax (i + n + 1, j + n, I, pmax);
        if (i == j || I[j] >= I[k])
        {
            if ((chkpt <= j - n || I[j] >= pmax[chkpt]) && (j - n == i || I[j] >= pmax[j - n]))
            {
                maximum.push_back (j);
            }
            if (i < j)
            {
                chkpt = i + n + 1;
            }
            i = j + n + 1;
        }
        else
        {
            i = k;
            chkpt = j + n + 1;
            while (i < size - n)
            {
                j = compPartialMax (chkpt, i + n, I, pmax);
                if (I[i] >= I[j])
                {
                    maximum.push_back (i);
                    i = i + n - 1;
                    break;
                }
                else
                {
                    chkpt = i + n - 1;
                    i = j;
                }
            }
        }
    }
}

bool cmp (uchar a, uchar b, int cmp)
{
    return (cmp == 1) ? a > b : b > a;
}


// founded on (2n + 1) × (2n + 1)-Block Algorithm from "Efficient Non-Maximum Suppression",
// Alexander Neubeck, Luc Van Gool;
// if comp == 1, then it found local maximum, else - local minimum
void Frame::suppression2D (int n, Mat image, vector<pair<int, int>> &maximum, int comp)
{
    for (int i = n; i <= image.rows - n; i += n + 1)
    {
        for (int j = n; j <= image.cols - n; j += n + 1)
        {
            int mi = i;
            int mj = j;
            for (int i2 = i; i2 <= i + n; i2++)
            {
                for (int j2 = j; j2 <= j + n; j2++)
                {
                    if (cmp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj), comp))
                    {
                        mi = i2;
                        mj = j2;
                    }
                }
            }
            int flag = 0;


            for (int i2 = mi - n; i2 <= mi + n && i2 < image.rows; i2++)
            {
                for (int j2 = mj - n; j2 <= j && j2 < image.cols; j2++)
                {
                    if (cmp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj), comp))
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) break;
                for (int j2 = j + n; j2 <= mj + n && j2 < image.cols; j2++)
                {
                    if (cmp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj), comp))
                    {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) break;
            }
            if (flag == 0)
            {
                for (int j2 = j; j2 <= j + n && j2 < image.cols; j2++)
                {
                    for (int i2 = mi - n; i2 <= i && i2 < image.rows; i2++)
                    {
                        if (cmp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj), comp))
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) break;
                    for (int i2 = i + n; i2 <= mi + n && i2 < image.rows; i2++)
                    {
                        if (cmp (image.at<uchar> (i2, j2), image.at<uchar> (mi, mj), comp))
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) break;
                }
            }
            if (flag == 0)
            {
                maximum.emplace_back (pair<int, int> (mi, mj));
            }
        }
    }
}

Mat &Frame::getIntegralImage ()
{
    return integralImage;
}

Mat &Frame::getXSobelConvolution(Mat image, Mat &result) {
    // x Sobel kernel
    // -1 0 1     [1]
    // -2 0 2  =  [2] * [-1, 0, 1]
    // -1 0 1     [1]

   Mat rowMult(result.rows, result.cols, CV_16S, Scalar::all (0));
   for (int i = 1; i < image.cols - 1; i++) {
       subtract(image.col(i + 1), image.col(i - 1), rowMult.col(i));
   }

    Mat temp(1, result.cols, CV_16S, Scalar::all (0));
   for (int i = 1; i < rowMult.rows - 1;  i++) {
       add(2 * rowMult.row(i), rowMult.row(i - 1), temp);
       add(temp, rowMult.row(i + 1), result.row(i));
   }

   return result;
}

Mat &Frame::getYSobelConvolution(Mat image, Mat &result) {
    // y Sobel kernel
    //  1  2  1     [1]
    //  0  0  0  =  [0] * [1, 2, 1]
    // -1 -2 -1     [-1]

    Mat rowMult(result.rows, result.cols, CV_16S, Scalar::all (0));
    Mat temp(result.rows, 1, CV_16S, Scalar::all (0));

    for (int i = 1; i < image.rows - 1;  i++) {
        subtract(image.row(i - 1), image.row(i + 1), rowMult.row(i));
    }

    for (int i = 1; i < rowMult.cols - 1; i++) {
        add(2 * rowMult.col(i), rowMult.col(i - 1), temp);
        add(temp, rowMult.col(i + 1), result.col(i));
    }

    return result;
}