#include "../src/Frame.h"
#include <gtest/gtest.h>



TEST (Frame, fillSumTest)
{
  Mat image = (Mat_<uchar> (5, 5) <<
      0, 1, 2, 3, 4,
      1, 5, 6, 7, 8,
      2, 6, 9, 10, 11,
      3, 7, 10, 12, 13,
      4, 8, 11, 13, 14);

  Frame frame (image);

  const Mat expectedIntegralImage =
      (Mat_<int> (6, 6) <<
          0, 0, 0, 0, 0, 0,
          0, 0, 1, 3, 6, 10,
          0, 1, 7, 15, 25, 37,
          0, 3, 15, 32, 52, 75,
          0, 6, 25, 52, 84, 120,
          0, 10, 37, 75, 120, 170);

  EXPECT_EQ (image.rows, expectedIntegralImage.rows - 1);
  EXPECT_EQ (image.cols, expectedIntegralImage.cols - 1);
  EXPECT_TRUE (countNonZero (frame.getIntegralImage () != expectedIntegralImage) == 0);
}


TEST (Frame, rectSumTest)
{
  Mat image = (Mat_<uchar> (5, 5) << 0, 1, 2, 3, 4, 1, 5, 6, 7, 8, 2, 6, 9, 10, 11, 3, 7, 10, 12,
      13, 4, 8, 11, 13, 14);

  Frame frame (image);

  EXPECT_EQ (frame.rectSum (0, 0, 0, 0), 0);
  EXPECT_EQ (frame.rectSum (0, 0, 1, 1), 0);
  EXPECT_EQ (frame.rectSum (0, 0, 1, 5), 10);
  EXPECT_EQ (frame.rectSum (1, 2, 2, 3), 51);
  EXPECT_EQ (frame.rectSum (2, 3, 3, 2), 73);
}


TEST (Frame, getConvolutionTest)
{
  int n = 2;
  Mat blob_detector = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, -1, -1, -1, -1, 1, 1, 1, -1,
      -1, 1, 8, 1, -1, -1, 1, 1, 1, -1, -1, -1, -1, -1, -1);

  Mat corner_detector = (Mat_<char> (2 * n + 1, 2 * n + 1) << -1, -1, 0, 1, 1, -1, -1, 0, 1, 1, 0,
      0, 0, 0, 0, 1, 1, 0, -1, -1, 1, 1, 0, -1, -1);


  int minSize = 5;
  int maxSize = 1000;
  int times = 100;
  int rows, cols;

  for (int i = 0; i < times; i++)
  {
    rows = rand () % (maxSize - minSize + 1) + minSize;
    cols = rand () % (maxSize - minSize + 1) + minSize;
    Mat image (rows, cols, CV_8UC1);
    randu (image, Scalar (0), Scalar (5));
    Frame frame (image);

    Mat result_blob (rows, cols, CV_8U, Scalar::all (0));
    Mat expected_result_blob (rows, cols, CV_8U, Scalar::all (0));
    filter2D (image, expected_result_blob, image.depth (), blob_detector);
    cv::rectangle (expected_result_blob,
                   cv::Rect (cv::Point (0, 0), expected_result_blob.size ()), Scalar::all (0), n);
    EXPECT_TRUE (countNonZero (frame.getBlobConvolution(result_blob) != expected_result_blob) == 0)
              << result_blob << endl
              << endl
              << expected_result_blob << "\n\n";

    Mat result_corner (rows, cols, CV_8U, Scalar::all (0));
    Mat expected_result_corner (rows, cols, CV_8U, Scalar::all (0));
    filter2D (image, expected_result_corner, image.depth (), corner_detector);
    cv::rectangle (expected_result_corner,
                   cv::Rect (cv::Point (0, 0), expected_result_corner.size ()), Scalar::all (0), n);

    frame.getCornerConvolution(result_corner);

    EXPECT_TRUE (countNonZero (frame.getCornerConvolution(result_corner) != expected_result_corner) == 0)
              << result_corner << endl
              << endl
              << expected_result_corner;
  }
}


TEST (Frame, suppression2DTest)
{
  int minSize = 25;
  int maxSize = 25;
  int times = 1;
  int rows, cols;

  for (int i = 0; i < times; i++) {

    rows = rand() % (maxSize - minSize + 1) + minSize;
    cols = rand() % (maxSize - minSize + 1) + minSize;
    Mat image(rows, cols, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Frame frame(image);
    Mat result_blob(rows, cols, CV_32SC1, Scalar::all(0));
    frame.getBlobConvolution(result_blob);
    vector<pair<int, int>> max;

    Mat white(rows, cols, CV_8UC1, Scalar(255, 255, 255));
    //   cout << endl << white << endl << endl;
    frame.suppression2D(2, white, max, 1);


  }

}

TEST (Frame, getXSobelConvolutionTest) {
  int n = 1;
  Mat sobel = (Mat_<char>(2 * n + 1, 2 * n + 1) <<
                                                -1, 0, 1,
      -2, 0, 2,
      -1, 0, 1);


  int minSize = 3;
  int maxSize = 1000;
  int times = 100;
  int rows, cols;

  for (int i = 0; i < times; i++) {
    rows = rand() % (maxSize - minSize + 1) + minSize;
    cols = rand() % (maxSize - minSize + 1) + minSize;
    Mat image(rows, cols, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Frame frame(image);

    Mat result(rows, cols, CV_8UC1, Scalar::all (0));
    Mat expected_result(rows, cols, CV_8UC1, Scalar::all(0));
    filter2D(image, expected_result, image.depth(), sobel);
    cv::rectangle(expected_result,
                  cv::Rect(cv::Point(0, 0), expected_result.size()), Scalar::all(0), n);
    EXPECT_TRUE (countNonZero(frame.getXSobelConvolution(image, result) != expected_result) == 0)
              << "image" << endl
              << image << "\n\n"
              << "result" << endl
              << result << "\n\n"
              << "expected" << endl
              << expected_result << "\n\n";

  }
}


TEST (Frame, getYSobelConvolutionTest) {
  int n = 1;
  Mat sobel = (Mat_<char>(2 * n + 1, 2 * n + 1) <<
                                                1,  2,  1,
      0,  0,  0,
      -1, -2, -1);


  int minSize = 3;
  int maxSize = 1000;
  int times = 100;
  int rows, cols;

  for (int i = 0; i < times; i++) {
    rows = rand() % (maxSize - minSize + 1) + minSize;
    cols = rand() % (maxSize - minSize + 1) + minSize;
    Mat image(rows, cols, CV_8UC1);
    randu(image, Scalar(0), Scalar(255));
    Frame frame(image);

    Mat result(rows, cols, CV_8UC1, Scalar::all (0));
    Mat expected_result(rows, cols, CV_8UC1, Scalar::all(0));
    filter2D(image, expected_result, image.depth(), sobel);
    cv::rectangle(expected_result,
                  cv::Rect(cv::Point(0, 0), expected_result.size()), Scalar::all(0), n);
    EXPECT_TRUE (countNonZero(frame.getYSobelConvolution(image, result) != expected_result) == 0)
              << "image" << endl
              << image << "\n\n"
              << "result" << endl
              << result << "\n\n"
              << "expected" << endl
              << expected_result << "\n\n";

  }
}


int main (int argc, char **argv)
{
  ::testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
