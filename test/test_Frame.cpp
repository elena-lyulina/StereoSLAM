#include <gtest/gtest.h>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <src/slam/Frame.h>

using namespace std;
using namespace cv;

TEST (Frame, fillSumTest)
{
    Mat image = (Mat_<uchar> (5, 5) << 0, 1, 2, 3, 4, 1, 5, 6, 7, 8, 2, 6, 9, 10, 11, 3, 7,
        10, 12, 13, 4, 8, 11, 13, 14);
    Frame frame (image);

    const Mat expectedIntegralImage =
        (Mat_<int> (6, 6) << 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 6, 10, 0, 1, 7, 15, 25, 37, 0, 3, 15, 32,
            52, 75, 0, 6, 25, 52, 84, 120, 0, 10, 37, 75, 120, 170);

    EXPECT_EQ (image.rows, expectedIntegralImage.rows - 1);
    EXPECT_EQ (image.cols, expectedIntegralImage.cols - 1);
    EXPECT_TRUE (countNonZero (frame.getIntegralImage () != expectedIntegralImage) == 0);
}


TEST (Frame, rectSumTest)
{
    Mat image = (Mat_<uchar> (5, 5) << 0, 1, 2, 3, 4, 1, 5, 6, 7, 8, 2, 6, 9, 10, 11, 3, 7,
        10, 12, 13, 4, 8, 11, 13, 14);

    Frame frame (image);

    EXPECT_EQ (frame.rectSum (0, 0, 0, 0), 0);
    EXPECT_EQ (frame.rectSum (0, 0, 1, 1), 0);
    EXPECT_EQ (frame.rectSum (0, 0, 1, 5), 10);
    EXPECT_EQ (frame.rectSum (1, 2, 2, 3), 51);
    EXPECT_EQ (frame.rectSum (2, 3, 3, 2), 73);
}

TEST (Frame, blobConvolutionTest)
{
    int n = 2;
    Mat blob_detector = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, -1, -1, -1, -1, 1, 1,
        1, -1, -1, 1, 8, 1, -1, -1, 1, 1, 1, -1, -1, -1, -1, -1, -1);

    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        Mat image (rows, cols, CV_8U);
        randu (image, Scalar (0), Scalar (255));

        Frame frame (image);

        // filter2D doesnt let CV_32S images, so convert to CV_32F
        Mat temp, expected_result_blob;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result_blob, -1, blob_detector);
        rectangle (expected_result_blob, Rect (Point (0, 0), expected_result_blob.size ()),
                   Scalar::all (0), n);
        expected_result_blob.convertTo (expected_result_blob, CV_32S);


        EXPECT_TRUE (countNonZero (frame.getBlobConvolution () != expected_result_blob) == 0)
                        << image << endl
                        << frame.getBlobConvolution () << endl
                        << endl
                        << expected_result_blob << "\n\n";
    }
}

// todo: fix it
TEST (Frame, cornerConvolutionTest)
{
    int n = 2;

    Mat corner_detector = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, 0, 1, 1, -1, -1, 0,
        1, 1, 0, 0, 0, 0, 0, 1, 1, 0, -1, -1, 1, 1, 0, -1, -1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        Mat image (rows, cols, CV_8UC1);
        randu (image, Scalar (0), Scalar (255));
        Frame frame (image);


        Mat temp, expected_result_corner;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result_corner, -1, corner_detector);
        rectangle (expected_result_corner, Rect (Point (0, 0), expected_result_corner.size ()),
                   Scalar::all (0), n);
        expected_result_corner.convertTo (expected_result_corner, CV_32S);


        EXPECT_TRUE (countNonZero (frame.getCornerConvolution () != expected_result_corner) == 0)
                        << image << endl
                        << endl
                        << frame.getCornerConvolution () << endl
                        << endl
                        << expected_result_corner;
    }
}

TEST (Frame, withVSwithoutSaturation) {
    int n = 2;
    Mat blob_detector = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, -1, -1, -1, -1, 1, 1,
        1, -1, -1, 1, 8, 1, -1, -1, 1, 1, 1, -1, -1, -1, -1, -1, -1);

    int area = 5;
    int minSize = 500;
    int maxSize = 1000;
    int times = 10;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        Mat image (rows, cols, CV_8U);
        randu (image, Scalar (0), Scalar (255));

        Frame frame (image);

        Mat temp, withoutSaturation_result, withSaturation_result;
        vector<pair<int, int>> minWithSat, maxWithSat, minWithoutSat, maxWithoutSat;

        // filter2D doesnt let CV_32S images, so convert to CV_32F
        image.convertTo (temp, CV_32F);
        filter2D (temp, withoutSaturation_result, -1, blob_detector);
        rectangle (withoutSaturation_result, Rect (Point (0, 0), withoutSaturation_result.size ()),
                   Scalar::all (0), n);
        withoutSaturation_result.convertTo (withoutSaturation_result, CV_32S);

        filter2D (image, withSaturation_result, -1, blob_detector);
        rectangle (withSaturation_result, Rect (Point (0, 0), withSaturation_result.size ()),
                   Scalar::all (0), n);

        Frame::suppression2D(area, withoutSaturation_result, maxWithoutSat, minWithoutSat);
        Frame::suppression2D(area, withSaturation_result, maxWithSat, minWithSat);

        cout << "max diff: " << maxWithoutSat.size() << ", " << maxWithSat.size() << endl;
        cout << "min diff: " << minWithoutSat.size() << ", " << minWithSat.size() << endl;

    }
}


TEST (Frame, suppression2DTest)
{
    int minSize = 100;
    int maxSize = 10000;
    int times = 100;
    int rows, cols;
    int minK = 2;
    int maxK = 20;
    int k;

    for (int l = 0; l < times; l++)
    {

        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        k = rand () % (maxK - minK + 1) + minK;
        Mat image (rows, cols, CV_32S);
        randu (image, Scalar (0), Scalar (255));

        vector<pair<int, int>> max;
        vector<pair<int, int>> min;


        Frame::suppression2D (k, image, max, min);

        vector<pair<int, int>> expected_max;
        vector<pair<int, int>> expected_min;
        bool maxFlag;
        bool minFlag;

        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = k; j < image.cols - k; j++)
            {
                int value = image.at<int> (i, j);
                maxFlag = true;
                minFlag = true;

                for (int ik = i - k; (maxFlag || minFlag) && ik <= i + k; ik++)
                {
                    for (int jk = j - k; (maxFlag || minFlag) && jk <= j + k; jk++)
                    {
                        int checkValue = image.at<int> (ik, jk);

                        if (image.at<int> (i, j) < image.at<int> (ik, jk))
                        {
                            maxFlag = false;
                        }
                        else if (image.at<int> (i, j) > image.at<int> (ik, jk))
                        {
                            minFlag = false;
                        }

                        else if (image.at<int> (i, j) == image.at<int> (ik, jk) && (i != ik || j != jk))
                        {
                            maxFlag = false;
                            minFlag = false;
                        }
                    }
                }

                if (maxFlag)
                {
                    expected_max.emplace_back (make_pair (i, j));
                }
                if (minFlag)
                {
                    expected_min.emplace_back (make_pair (i, j));
                }
            }
        }

        sort (max.begin (), max.end ());
        sort (min.begin (), min.end ());

        EXPECT_TRUE (expected_max == max)
                        << "expected size " << expected_max.size () << ", actual size " << max.size () << endl;
        EXPECT_TRUE (expected_min == min)
                        << "expected size " << expected_min.size () << ", actual size " << min.size () << endl;
    }
}

TEST (Frame, getXSobelConvolutionTest)
{
    int n = 1;
    Mat sobel = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, 0, 1, -2, 0, 2, -1, 0, 1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        Mat image (rows, cols, CV_8UC1);
        randu (image, Scalar (0), Scalar (255));
        Frame frame (image);

        Mat temp, expected_result;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result, -1, sobel);
        rectangle (expected_result, Rect (Point (0, 0), expected_result.size ()),
                   Scalar::all (0), n);
        expected_result.convertTo (expected_result, CV_32S);

        EXPECT_TRUE (countNonZero (frame.getXSobelConvolution () != expected_result) == 0)
                        << "image" << endl
                        << image << "\n\n"
                        << "result" << endl
                        << frame.getXSobelConvolution () << "\n\n"
                        << "expected" << endl
                        << expected_result << "\n\n";
    }
}


TEST (Frame, getYSobelConvolutionTest)
{
    int n = 1;
    Mat sobel = (Mat_<char> (2 * n + 1, 2 * n + 1) << 1, 2, 1, 0, 0, 0, -1, -2, -1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        Mat image (rows, cols, CV_8UC1);
        randu (image, Scalar (0), Scalar (255));
        Frame frame (image);

        Mat temp, expected_result;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result, -1, sobel);
        rectangle (expected_result, Rect (Point (0, 0), expected_result.size ()),
                   Scalar::all (0), n);
        expected_result.convertTo (expected_result, CV_32S);

        EXPECT_TRUE (countNonZero (frame.getYSobelConvolution () != expected_result) == 0)
                        << "image" << endl
                        << image << "\n\n"
                        << "result" << endl
                        << frame.getXSobelConvolution () << "\n\n"
                        << "expected" << endl
                        << expected_result << "\n\n";
    }
}


int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}
