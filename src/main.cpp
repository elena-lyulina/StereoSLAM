#include "Frame.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


int main (int argc, char *argv[])
{
    const char *filename;
    if (argc >= 2)
    {
        filename = argv[1];
    }
    else
    {
        cerr << "No file path found" << endl;
        return 1;
    }
    //
    //    Mat image;
    //    image = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
    //
    //    if (image.empty ())
    //    {
    //        cerr << "Can't open image [" << filename << "]" << endl;
    //        return -1;
    //    }

    Mat image (100, 100, CV_8UC1, Scalar (30, 255, 2));
    namedWindow ("Input", WINDOW_AUTOSIZE);
    namedWindow ("My filtering", WINDOW_AUTOSIZE);


    Mat result (image.rows, image.cols, CV_16UC1, Scalar::all (0));

    Frame frame (image);
    frame.getBlobConvolution(result);


    // checking 2d suppression
    // first of all, convert image to 2d array
    auto **array_ = new uchar *[result.rows];
    for (int i = 0; i < result.rows; i++)
        array_[i] = new uchar[result.cols];

    for (int i = 0; i < result.cols; i++)
        array_[i] = result.ptr<uchar> (i);


    // now i need to transonse it; todo: find better way to get 2d data!

    auto **array = new uchar *[result.cols];
    for (int i = 0; i < result.cols; i++)
        array[i] = new uchar[result.rows];

    for (int i = 0; i < result.rows; i++)
    {
        for (int j = 0; j < result.cols; j++)
        {
            array[j][i] = array_[i][j];
        }
    }


    vector<pair<int, int>> vect2dMax;
    vector<pair<int, int>> vect2dMin;
    Mat image3 = Mat::zeros (result.rows, result.cols, CV_16UC3);
    cvtColor (image, image3, COLOR_GRAY2BGR);
    frame.suppression2D (20, array, result.rows, result.cols, vect2dMax, 1);
    frame.suppression2D (20, array, result.rows, result.cols, vect2dMin, -1);
    //    for (auto x : vect2dMax)
    //    {
    //        std::cout << x.first << " " << x.second << "\n";
    //        circle (image3, Point (x.first, x.second), 2, Scalar (66, 244, 78), -1);
    //    }

    for (auto x : vect2dMin)
    {
        std::cout << x.first << " " << x.second << "\n";
        circle (image3, Point (x.first, x.second), 3, Scalar (65, 65, 245), -1);
    }


    imshow ("My filtering", result);
    imshow ("Input", image3);


    // Free each sub-array
    //    for(int i = 0; i < result.cols; i++) {
    //        delete[] array_[i];
    //    }
    ////    //Free the array of pointers
    ////    delete[] array_;
    //
    //    //Free each sub-array
    //    for(int i = 0; i < result.cols; i++) {
    //        delete[] array[i];
    //    }
    //    //Free the array of pointers
    //    delete[] array;


    //    imshow("Reliable filtering", filtered);


    waitKey ();
    return 0;
}
