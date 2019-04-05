#include "../src/Frame.h"
#include <gtest/gtest.h>


TEST (Frame, fillSumTest)
{
    cv::Mat image = (cv::Mat_<uchar> (5, 5) << 0, 1, 2, 3, 4, 1, 5, 6, 7, 8, 2, 6, 9, 10, 11, 3, 7,
                     10, 12, 13, 4, 8, 11, 13, 14);

    Frame frame (image);

    const cv::Mat expectedIntegralImage =
    (cv::Mat_<int> (6, 6) << 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 6, 10, 0, 1, 7, 15, 25, 37, 0, 3, 15, 32,
     52, 75, 0, 6, 25, 52, 84, 120, 0, 10, 37, 75, 120, 170);

    EXPECT_EQ (image.rows, expectedIntegralImage.rows - 1);
    EXPECT_EQ (image.cols, expectedIntegralImage.cols - 1);
    EXPECT_TRUE (countNonZero (frame.getIntegralImage () != expectedIntegralImage) == 0);
}


TEST (Frame, rectSumTest)
{
    cv::Mat image = (cv::Mat_<uchar> (5, 5) << 0, 1, 2, 3, 4, 1, 5, 6, 7, 8, 2, 6, 9, 10, 11, 3, 7,
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
    cv::Mat blob_detector = (cv::Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, -1, -1, -1, -1, 1, 1,
                             1, -1, -1, 1, 8, 1, -1, -1, 1, 1, 1, -1, -1, -1, -1, -1, -1);

    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        cv::Mat image (rows, cols, CV_8U);
        randu (image, cv::Scalar (0), cv::Scalar (255));

        Frame frame (image);

        // filter2D doesnt let CV_32S images, so convert to CV_32F
        cv::Mat temp, expected_result_blob;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result_blob, -1, blob_detector);
        cv::rectangle (expected_result_blob, cv::Rect (cv::Point (0, 0), expected_result_blob.size ()),
                       cv::Scalar::all (0), n);
        expected_result_blob.convertTo (expected_result_blob, CV_32S);


        EXPECT_TRUE (countNonZero (frame.getBlobConvolution () != expected_result_blob) == 0)
        << image << std::endl
        << frame.getBlobConvolution () << std::endl
        << std::endl
        << expected_result_blob << "\n\n";
    }
}

// todo: fix it
TEST (Frame, getConvolutionTest)
{
    int n = 2;

    cv::Mat corner_detector = (cv::Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, 0, 1, 1, -1, -1, 0,
                               1, 1, 0, 0, 0, 0, 0, 1, 1, 0, -1, -1, 1, 1, 0, -1, -1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        cv::Mat image (rows, cols, CV_8UC1);
        randu (image, cv::Scalar (0), cv::Scalar (255));
        Frame frame (image);


        cv::Mat temp, expected_result_corner;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result_corner, -1, corner_detector);
        cv::rectangle (expected_result_corner, cv::Rect (cv::Point (0, 0), expected_result_corner.size ()),
                       cv::Scalar::all (0), n);
        expected_result_corner.convertTo (expected_result_corner, CV_32S);


        EXPECT_TRUE (countNonZero (frame.getCornerConvolution () != expected_result_corner) == 0)
        << image << std::endl
        << std::endl
        << frame.getCornerConvolution () << std::endl
        << std::endl
        << expected_result_corner;
    }
}


TEST (Frame, suppression2DTest)
{
    int minSize = 50;
    int maxSize = 1000;
    int times = 100;
    int rows, cols;
    int minK = 1;
    int maxK = minSize;
    int k;

    for (int l = 0; l < times; l++)
    {

        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        k = rand () % (maxK - minK + 1) + minK;
        cv::Mat image (rows, cols, CV_8UC1);
        randu (image, cv::Scalar (0), cv::Scalar (10));

        std::vector<std::pair<int, int>> max;
        std::vector<std::pair<int, int>> min;


        Frame::suppression2D (k, image, max, min);

        std::vector<std::pair<int, int>> expected_max;
        std::vector<std::pair<int, int>> expected_min;
        bool maxFlag;
        bool minFlag;
        for (int i = k; i < image.rows - k; i++)
        {
            for (int j = k; j < image.cols - k; j++)
            {
                maxFlag = true;
                minFlag = true;

                for (int ik = i - k; (maxFlag || minFlag) && ik <= i + k; ik++)
                {
                    for (int jk = j - k; (maxFlag || minFlag) && jk <= j + k; jk++)
                    {

                        if (image.at<uchar> (i, j) < image.at<uchar> (ik, jk))
                        {
                            maxFlag = false;
                        }
                        else if (image.at<uchar> (i, j) > image.at<uchar> (ik, jk))
                        {
                            minFlag = false;
                        }

                        else if (image.at<uchar> (i, j) == image.at<uchar> (ik, jk) && (i != ik || j != jk))
                        {
                            maxFlag = false;
                            minFlag = false;
                        }
                    }
                }

                if (maxFlag)
                {
                    expected_max.emplace_back (std::make_pair (i, j));
                }
                if (minFlag)
                {
                    expected_min.emplace_back (std::make_pair (i, j));
                }
            }
        }

        sort (max.begin (), max.end ());
        sort (min.begin (), min.end ());

        EXPECT_TRUE (expected_max == max)
        << "expected size " << expected_max.size () << ", actual size " << max.size () << std::endl;
        EXPECT_TRUE (expected_min == min)
        << "expected size " << expected_min.size () << ", actual size " << min.size () << std::endl;
    }
}

TEST (Frame, getXSobelConvolutionTest)
{
    int n = 1;
    cv::Mat sobel = (cv::Mat_<char> (2 * n + 1, 2 * n + 1) << -1, 0, 1, -2, 0, 2, -1, 0, 1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        cv::Mat image (rows, cols, CV_8UC1);
        randu (image, cv::Scalar (0), cv::Scalar (255));
        Frame frame (image);

        cv::Mat temp, expected_result;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result, -1, sobel);
        cv::rectangle (expected_result, cv::Rect (cv::Point (0, 0), expected_result.size ()),
                       cv::Scalar::all (0), n);
        expected_result.convertTo (expected_result, CV_32S);

        EXPECT_TRUE (countNonZero (frame.getXSobelConvolution () != expected_result) == 0)
        << "image" << std::endl
        << image << "\n\n"
        << "result" << std::endl
        << frame.getXSobelConvolution () << "\n\n"
        << "expected" << std::endl
        << expected_result << "\n\n";
    }
}


TEST (Frame, getYSobelConvolutionTest)
{
    int n = 1;
    cv::Mat sobel = (cv::Mat_<char> (2 * n + 1, 2 * n + 1) << 1, 2, 1, 0, 0, 0, -1, -2, -1);


    int minSize = 5;
    int maxSize = 1000;
    int times = 1000;
    int rows, cols;

    for (int i = 0; i < times; i++)
    {
        rows = rand () % (maxSize - minSize + 1) + minSize;
        cols = rand () % (maxSize - minSize + 1) + minSize;
        cv::Mat image (rows, cols, CV_8UC1);
        randu (image, cv::Scalar (0), cv::Scalar (255));
        Frame frame (image);

        cv::Mat temp, expected_result;
        image.convertTo (temp, CV_32F);
        filter2D (temp, expected_result, -1, sobel);
        cv::rectangle (expected_result, cv::Rect (cv::Point (0, 0), expected_result.size ()),
                       cv::Scalar::all (0), n);
        expected_result.convertTo (expected_result, CV_32S);

        EXPECT_TRUE (countNonZero (frame.getYSobelConvolution () != expected_result) == 0)
        << "image" << std::endl
        << image << "\n\n"
        << "result" << std::endl
        << frame.getXSobelConvolution () << "\n\n"
        << "expected" << std::endl
        << expected_result << "\n\n";
    }
}


int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}
