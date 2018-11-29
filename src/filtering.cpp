#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Mat& squared_sum(Mat &image, Mat &result);
Mat& filtered(Mat &image, int n, int coefficient, Mat &result);



int main( int argc, char* argv[])
{
    const char* filename;
    if (argc >= 2) {
        filename = argv[1];
    } else {
        cerr << "No file path found" << endl;
        return 1;
    }

//   Mat image;
//    image = imread(filename, IMREAD_COLOR);
//
//    if (image.empty())
//    {
//        cerr << "Can't open image ["  << filename << "]" << endl;
//        return -1;
//    }

    Mat image(100, 100, CV_8UC3, Scalar(0, 0, 255));
    namedWindow("Input", WINDOW_AUTOSIZE);
    namedWindow("My filtering", WINDOW_AUTOSIZE);
    namedWindow("Reliable filtering", WINDOW_AUTOSIZE);
    imshow("Input", image);


    Mat result(image.rows, image.cols, CV_16UC3, Scalar::all(0));
    filtered(image, 1, 1, result);
    filtered(result, 0, -9, result);

    imshow("My filtering", result);

    Mat filter = (Mat_<char>(3, 3) <<  1,  1,  1,
                                       1, -8,  1,
                                       1,  1,  1);



    Mat filtered;
    filter2D(image, filtered, image.depth(), filter);
    imshow("Reliable filtering", filtered);

//    namedWindow("Input", WINDOW_AUTOSIZE);
//    namedWindow("Blobs", WINDOW_AUTOSIZE);
//    namedWindow("Corners", WINDOW_AUTOSIZE);


// first trying this way:
//    Mat blob_detector = (Mat_<char>(5,5) << -1, -1, -1, -1, -1,
//                                            -1,  1,  1,  1, -1,
//                                            -1,  1,  8,  1, -1,
//                                            -1,  1,  1,  1, -1,
//                                            -1, -1, -1, -1, -1);
//
//    Mat blob_filtered;
//    filter2D(image, blob_filtered, image.depth(), blob_detector);
//
//
//    Mat corner_detector = (Mat_<char>(5,5) << -1, -1,  0,  1,  1,
//                                              -1, -1,  0,  1,  1,
//                                               0,  0,  0,  0,  0,
//                                               1,  1,  0, -1, -1,
//                                               1,  1,  0, -1, -1);
//
//    Mat corner_filtered;
//    filter2D(image, corner_filtered, image.depth(), corner_detector);
//
//
//    imshow("Input", image);
//    imshow("Blobs", blob_filtered);
//    imshow("Corners", corner_filtered);


    waitKey();
    return 0;
}

/*
 * Counting sum of squared pixels intensities, starting from the top left corner (taken as (0,0) coordinate);
 * The sum of pixels intensities, placed between (0,0) and (i,j) is stored at (i,j) coordinates in Result Mat,
 * assuming that Result mat has the same size as image
 * todo: check size of result mat? find better solution?
 * todo: find out comment style in c++
 */
Mat& squared_sum(Mat &image, Mat &result) {
    //CV_Assert(image.depth() == CV_8U);  // accept only uchar images
    CV_Assert(result.size() == image.size()); // sums should be stored in the same size mat

    Mat_<Vec3b> image_ = image;
    Mat_<Vec3i> result_ = result;

    Vec3i current_line_sum = 0;

    // fill the first row of sum matrix
    for (int j = 0; j < image_.cols; j++) {
        current_line_sum[0] += image_(0,j)[0];
        current_line_sum[1] += image_(0,j)[1];
        current_line_sum[2] += image_(0,j)[2];

        result_[0][j][0] = current_line_sum[0];
        result_[0][j][1] = current_line_sum[1];
        result_[0][j][2] = current_line_sum[2];
    }

    // fill other rows, using sum of previous rows
    for (int i = 1; i < image_.rows; i++) {
        current_line_sum = 0;

        for (int j = 0; j < image_.cols; j++) {
            current_line_sum[0] += image_(i,j)[0];
            current_line_sum[1] += image_(i,j)[1];
            current_line_sum[2] += image_(i,j)[2];

            result_[i][j][0] = current_line_sum[0] + result_[i-1][j][0];
            result_[i][j][1] = current_line_sum[1] + result_[i-1][j][1];
            result_[i][j][2] = current_line_sum[2] + result_[i-1][j][2];
        }
    }

    result = result_;
    return result;
}

/*
 * Filtering image by using sum of pixel in squares;
 *
 * image - image to filter
 * squared_sum - mat with sum of pixel in squares (the same format as result of Mat& squared_sum(Mat &image, Mat &result) function)
 * n - half of kernel matrix size; assuming that kernel matrix has [2n+1]x[2n+1] size
 * coefficient - coefficient to apply
 * result - to store filtered mat
 */
Mat& filtered(Mat &image, int n, int coefficient, Mat &result) {
    Mat_<Vec3b> image_ = image;
    Mat_<Vec3i> result_ = result;

    Mat sum(image.rows, image.cols, CV_16UC3, Scalar::all(0));
    squared_sum(image, sum);
    Mat_<Vec3i> squared_sum_ = sum;

    for (int i = n + 1; i <= image.rows - n - 1; i++) {
        for (int j = n + 1; j <= image.cols - n - 1; j++) {
            result_[i][j][0] = (squared_sum_[i+n][j+n][0] - squared_sum_[i+n][j-n-1][0] - squared_sum_[i-n-1][j+n][0] + squared_sum_[i-n-1][j-n-1][0]) * coefficient;
            result_[i][j][1] = (squared_sum_[i+n][j+n][1] - squared_sum_[i+n][j-n-1][1] - squared_sum_[i-n-1][j+n][1] + squared_sum_[i-n-1][j-n-1][1]) * coefficient;
            result_[i][j][2] = (squared_sum_[i+n][j+n][2] - squared_sum_[i+n][j-n-1][2] - squared_sum_[i-n-1][j+n][2] + squared_sum_[i-n-1][j-n-1][2]) * coefficient;
        }
    }
    result = result_;
    return result;
}
