#include "FeatureDetection.h"
#include "FeaturePoint.h"
#include "Frame.h"
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


int main (int argc, char *argv[])
{
    const char *filename1;
    const char *filename2;
    if (argc >= 2)
    {
        filename1 = argv[1];
        filename2 = argv[2];
    }
    else
    {
        cerr << "No file path found" << endl;
        return 1;
    }

    vector<const char *> filenames{ filename1, filename2 };
    vector<Mat> images;
    vector<Mat> imagesRGB;
    vector<Mat> results;
    vector<Frame> frames;
    vector<vector<pair<int, int>>> blobMaxCoord;
    vector<vector<FeaturePoint>> blobMaxPoints;

    // checking 'blob max' class of feature points
    int n = 10;
    for (int i = 0; i < 2; i++)
    {
        images.emplace_back (imread (filenames[i], CV_LOAD_IMAGE_GRAYSCALE));
        imagesRGB.emplace_back(Mat(images[i].rows, images[i].cols, CV_8UC3, Scalar::all (0)));
        cvtColor(images[i], imagesRGB[i], CV_GRAY2RGB);

        frames.emplace_back (Frame (images[i]));
        results.emplace_back (Mat (images[i].rows, images[i].cols, CV_8U, Scalar::all (0)));

        blobMaxCoord.emplace_back (vector<pair<int, int>> ());
        frames[i].suppression2D (n, frames[i].getBlobConvolution (results[i]), blobMaxCoord[i], 1);

        blobMaxPoints.emplace_back (vector<FeaturePoint> ());
        for (auto p : blobMaxCoord[i])
        {
            blobMaxPoints[i].emplace_back (FeaturePoint (frames[i], p.first, p.second));
        }
    }


    vector<pair<FeaturePoint, FeaturePoint>> matches;
    int error = 1;

    cout << "match size " << matches.size() << endl;
    FeatureDetection::findMatches (blobMaxPoints[0], blobMaxPoints[1], matches, error);
    cout << "match size " << matches.size() << endl;


    RNG rng(12345);
    Scalar color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));

    for (auto m : matches) {
        cout << m.first.row << " " << m.first.col << endl;
        cout << m.second.row << " " << m.second.col << endl;

        circle (imagesRGB[0], Point (m.first.row, m.first.col), 5, color, -1);
        circle (imagesRGB[1], Point (m.second.row, m.second.col), 5, color, -1);
        color = Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
    }

    namedWindow ("image1", WINDOW_AUTOSIZE);
    namedWindow ("image2", WINDOW_AUTOSIZE);

//    namedWindow ("result1", WINDOW_AUTOSIZE);
//    namedWindow ("result2", WINDOW_AUTOSIZE);

    imshow ("image1", imagesRGB[0]);
    imshow ("image2", imagesRGB[1]);

//    imshow ("result1", results[0]);
//    imshow ("result2", results[1]);

    waitKey ();
    return 0;
}
