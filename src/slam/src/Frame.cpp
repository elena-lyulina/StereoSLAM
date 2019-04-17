#include <Frame.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/imgproc.hpp>

Frame::Frame (cv::Mat &img)
: image (img), integralImage (cv::Mat::zeros (image.rows + 1, image.cols + 1, CV_32S)),
  xSobel (cv::Mat::zeros (image.rows, image.cols, CV_32S)),
  ySobel (cv::Mat::zeros (image.rows, image.cols, CV_32S)),
  blobConvolution (cv::Mat::zeros (image.rows, image.cols, CV_32S)),
  cornerConvolution (cv::Mat::zeros (image.rows, image.cols, CV_32S))
{
    fillSum ();
    doXSobelConvolution (image, xSobel);
    doYSobelConvolution (image, ySobel);
    doBlobConvolution (blobConvolution);
    doCornerConvolution (cornerConvolution);
}

Frame::Frame (const char *filename)
{
    cv::Mat img = cv::imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
    *this = Frame (img);
}

void Frame::fillSum ()
{
    int current_line_sum = 0;

    // assuming that the first column and raw are filled with zeros
    // fill other rows, using sum of previous rows
    for (int i = 0; i < image.rows; i++)
    {
        current_line_sum = 0;
        for (int j = 0; j < image.cols; j++)
        {
            current_line_sum += image.at<uchar> (i, j);
            integralImage.at<int> (i + 1, j + 1) = current_line_sum + integralImage.at<int> (i, j + 1);
        }
    }
}


int Frame::rectSum (int x, int y, int width, int height)
{
    return integralImage.at<uint> (y + height, x + width) - integralImage.at<uint> (y + height, x) -
           integralImage.at<uint> (y, x + width) + integralImage.at<uint> (y, x);
}


cv::Mat &Frame::rectSumMat (int sideLength, cv::Mat &result)
{

    int width = integralImage.cols - sideLength;
    int height = integralImage.rows - sideLength;
    int n = sideLength / 2;

    result = integralImage (cv::Rect (sideLength, sideLength, width, height)) -
             integralImage (cv::Rect (0, sideLength, width, height)) +
             integralImage (cv::Rect (0, 0, width, height)) -
             integralImage (cv::Rect (sideLength, 0, width, height));

    copyMakeBorder (result, result, n, n, n, n, cv::BORDER_CONSTANT);


    return result;
}

cv::Mat &Frame::doBlobConvolution (cv::Mat &result)
{
    //  (-1, -1, -1, -1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1,  1,  8,  1, -1,
    //   -1,  1,  1,  1, -1,
    //   -1, -1, -1, -1, -1 )


    cv::Mat temp, temp1, temp2, temp3;
    image.convertTo (temp, CV_32S);

    const int w = image.cols - 4, h = image.rows - 4;

    result (cv::Rect (2, 2, w, h)) =
    2 * integralImage (cv::Rect (4, 4, w, h)) - 2 * integralImage (cv::Rect (1, 4, w, h)) +
    2 * integralImage (cv::Rect (1, 1, w, h)) - 2 * integralImage (cv::Rect (4, 1, w, h))

    - integralImage (cv::Rect (5, 5, w, h)) + integralImage (cv::Rect (0, 5, w, h)) -
    integralImage (cv::Rect (0, 0, w, h)) + integralImage (cv::Rect (5, 0, w, h))

    + 7 * temp (cv::Rect (2, 2, w, h));


    return result;
}

cv::Mat &Frame::doCornerConvolution (cv::Mat &result)
{
    //  (-1, -1,  0,  1,  1,   (1,
    //   -1, -1,  0,  1,  1,    1,
    //    0,  0,  0,  0,  0, =  0, * (-1, -1,  0,  1,  1)
    //    1,  1,  0, -1, -1,   -1,
    //    1,  1,  0, -1, -1 )  -1)


    cv::Mat rowMult (result.rows, result.cols, CV_32S, cv::Scalar::all (0));
    cv::Mat temp;
    image.convertTo (temp, CV_32S);

    const int w = temp.cols, h = temp.rows;

    rowMult (cv::Rect (0, 2, w, h - 4)) =
    temp (cv::Rect (0, 0, w, h - 4)) + temp (cv::Rect (0, 1, w, h - 4)) -
    temp (cv::Rect (0, 3, w, h - 4)) - temp (cv::Rect (0, 4, w, h - 4));

    result (cv::Rect (2, 2, w - 4, h - 4)) =
    -rowMult (cv::Rect (0, 2, w - 4, h - 4)) - rowMult (cv::Rect (1, 2, w - 4, h - 4)) +
    rowMult (cv::Rect (3, 2, w - 4, h - 4)) + rowMult (cv::Rect (4, 2, w - 4, h - 4));
    return result;
}


const cv::Mat &Frame::getImage ()
{
    return image;
}

const cv::Mat &Frame::getIntegralImage ()
{
    return integralImage;
}

const cv::Mat &Frame::getXSobelConvolution ()
{
    return xSobel;
}

const cv::Mat &Frame::getYSobelConvolution ()
{
    return ySobel;
}

const cv::Mat &Frame::getBlobConvolution ()
{
    return blobConvolution;
}

const cv::Mat &Frame::getCornerConvolution ()
{
    return cornerConvolution;
}

cv::Mat &Frame::doXSobelConvolution (cv::Mat image, cv::Mat &result)
{
    // x Sobel kernel
    // -1 0 1     [1]
    // -2 0 2  =  [2] * [-1, 0, 1]
    // -1 0 1     [1]


    cv::Mat rowMult (result.rows, result.cols, CV_32S, cv::Scalar::all (0));
    cv::Mat temp;
    image.convertTo (temp, CV_32S);

    const int w = temp.cols, h = temp.rows;

    rowMult (cv::Rect (0, 1, w, h - 2)) = temp (cv::Rect (0, 0, w, h - 2)) +
                                          2 * temp (cv::Rect (0, 1, w, h - 2)) +
                                          temp (cv::Rect (0, 2, w, h - 2));

    result (cv::Rect (1, 1, w - 2, h - 2)) =
    -rowMult (cv::Rect (0, 1, w - 2, h - 2)) + rowMult (cv::Rect (2, 1, w - 2, h - 2));

    return result;
}

cv::Mat &Frame::doYSobelConvolution (cv::Mat image, cv::Mat &result)
{
    // y Sobel kernel
    //  1  2  1     [1]
    //  0  0  0  =  [0] * [1, 2, 1]
    // -1 -2 -1     [-1]


    cv::Mat rowMult (result.rows, result.cols, CV_32S, cv::Scalar::all (0));
    cv::Mat temp;
    image.convertTo (temp, CV_32S);

    const int w = temp.cols, h = temp.rows;

    rowMult (cv::Rect (0, 1, w, h - 2)) =
    temp (cv::Rect (0, 0, w, h - 2)) - temp (cv::Rect (0, 2, w, h - 2));

    result (cv::Rect (1, 1, w - 2, h - 2)) = rowMult (cv::Rect (0, 1, w - 2, h - 2)) +
                                             2 * rowMult (cv::Rect (1, 1, w - 2, h - 2)) +
                                             rowMult (cv::Rect (2, 1, w - 2, h - 2));

    return result;
}

template <typename Comparator>
bool equalOrCompNotFound (int n, const cv::Mat &image, int mi, int mj, int i, int j, Comparator comp)
{
    // looking for max/min value into [n x n] image areas, which top left corner is represented by (i, j) coord
    int depth = image.type () & CV_MAT_DEPTH_MASK;
    assert (depth == CV_32S);

    bool geNotFound = true;

    for (int i2 = mi - n; geNotFound && i2 <= mi + n && i2 < image.rows; i2++)
    {
        for (int j2 = mj - n; geNotFound && j2 < j && j2 < image.cols; j2++)
        {

            if (comp (image.at<int> (i2, j2), image.at<int> (mi, mj)) ||
                image.at<int> (i2, j2) == image.at<int> (mi, mj))
            {
                geNotFound = false;
            }
        }
        for (int j2 = j + n + 1; geNotFound && j2 <= mj + n && j2 < image.cols; j2++)
        {

            if (comp (image.at<int> (i2, j2), image.at<int> (mi, mj)) ||
                image.at<int> (i2, j2) == image.at<int> (mi, mj))
            {
                geNotFound = false;
            }
        }
    }

    for (int j2 = j; geNotFound && j2 <= j + n && j2 < image.cols; j2++)
    {
        for (int i2 = mi - n; geNotFound && i2 < i && i2 < image.rows; i2++)
        {

            if (comp (image.at<int> (i2, j2), image.at<int> (mi, mj)) ||
                image.at<int> (i2, j2) == image.at<int> (mi, mj))
            {
                geNotFound = false;
            }
        }

        for (int i2 = i + n + 1; geNotFound && i2 <= mi + n && i2 < image.rows; i2++)
        {

            if (comp (image.at<int> (i2, j2), image.at<int> (mi, mj)) ||
                image.at<int> (i2, j2) == image.at<int> (mi, mj))
            {
                geNotFound = false;
            }
        }
    }

    return geNotFound;
}


// currently only for <int> images !!
void Frame::suppression2D (int n,
                           const cv::Mat &image,
                           std::vector<std::pair<int, int>> &maxResult,
                           std::vector<std::pair<int, int>> &minResult)
{
    // looking for max/min value into [n x n] image areas, which top left corner is represented by (i, j) coord
    int depth = image.type () & CV_MAT_DEPTH_MASK;
    assert (depth == CV_32S);

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

                    if (image.at<int> (i2, j2) > image.at<int> (maxi, maxj))
                    {

                        equalMaxFound = false;
                        maxi = i2;
                        maxj = j2;
                    }
                    else if (image.at<int> (i2, j2) == image.at<int> (maxi, maxj) && (i2 != maxi || j2 != maxj))
                    {
                        equalMaxFound = true;
                    }

                    if (image.at<int> (i2, j2) < image.at<int> (mini, minj))
                    {
                        equalMinFound = false;
                        mini = i2;
                        minj = j2;
                    }
                    else if (image.at<int> (i2, j2) == image.at<int> (mini, minj) && (i2 != mini || j2 != minj))
                    {
                        equalMinFound = true;
                    }
                }
            }

            // if max was found, check if there is no another max or equal values outside of
            // observed [n x n] area and only then it declares as local max
            if ((!equalMaxFound && (maxi < image.rows - n) && (maxj < image.cols - n)))
            {
                if (equalOrCompNotFound (n, image, maxi, maxj, i, j, std::greater<int> ()))
                {
                    maxResult.emplace_back (std::pair<int, int> (maxi, maxj));
                }
            }

            // if min was found, check if there is no another min or equal values outside of
            // observed [n x n] area and only then it declares as local min
            if ((!equalMinFound && (mini < image.rows - n) && (minj < image.cols - n)))
            {
                if (equalOrCompNotFound (n, image, mini, minj, i, j, std::less<int> ()))
                {
                    minResult.emplace_back (std::pair<int, int> (mini, minj));
                }
            }
        }
    }
}