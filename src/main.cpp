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
    const char *filename3;
    const char *filename4;
    if (argc >= 4)
    {
        filename1 = argv[1];
        filename2 = argv[2];
        filename3 = argv[3];
        filename4 = argv[4];
    }
    else
    {
        cerr << "No file path found" << endl;
        return 1;
    }

    vector<const char *> filenames{ filename1, filename2, filename3, filename4 };
    vector<Mat> images;
    vector<Mat> imagesRGB;
    vector<Frame> frames;

    vector<Mat> results;

    vector<vector<pair<int, int>>> blobMaxCoord;
    vector<vector<pair<int, int>>> blobMinCoord;
    vector<vector<pair<int, int>>> cornerMaxCoord;
    vector<vector<pair<int, int>>> cornerMinCoord;

    vector<vector<FeaturePoint>> blobMaxPoints;
    vector<vector<FeaturePoint>> blobMinPoints;
    vector<vector<FeaturePoint>> cornerMaxPoints;
    vector<vector<FeaturePoint>> cornerMinPoints;

    int n = 10;
    for (int i = 0; i < filenames.size(); i++)
    {
        images.emplace_back (imread (filenames[i], CV_LOAD_IMAGE_GRAYSCALE));
        imagesRGB.emplace_back(Mat(images[i].rows, images[i].cols, CV_8UC3, Scalar::all (0)));
        cvtColor(images[i], imagesRGB[i], CV_GRAY2RGB);
        frames.emplace_back (Frame (images[i]));
        results.emplace_back (Mat (images[i].rows, images[i].cols, CV_8U, Scalar::all (0)));


        blobMaxCoord.emplace_back (vector<pair<int, int>> ());
        blobMinCoord.emplace_back (vector<pair<int, int>> ());
        cornerMaxCoord.emplace_back (vector<pair<int, int>> ());
        cornerMinCoord.emplace_back (vector<pair<int, int>> ());

        frames[i].suppression2D (n, frames[i].getBlobConvolution (results[i]), blobMaxCoord[i], greater<int>());
        frames[i].suppression2D (n, frames[i].getBlobConvolution (results[i]), blobMinCoord[i], less<int>());
        frames[i].suppression2D (n, frames[i].getCornerConvolution (results[i]), cornerMaxCoord[i], greater<int>());
        frames[i].suppression2D (n, frames[i].getCornerConvolution (results[i]), cornerMinCoord[i], less<int>());


        blobMaxPoints.emplace_back (vector<FeaturePoint> ());
        blobMinPoints.emplace_back (vector<FeaturePoint> ());
        cornerMaxPoints.emplace_back (vector<FeaturePoint> ());
        cornerMinPoints.emplace_back (vector<FeaturePoint> ());

        for (auto p : blobMaxCoord[i])
        {
            blobMaxPoints[i].emplace_back (FeaturePoint (frames[i], p.first, p.second));
        }
        for (auto p : blobMinCoord[i])
        {
            blobMinPoints[i].emplace_back (FeaturePoint (frames[i], p.first, p.second));
        }
        for (auto p : cornerMaxCoord[i])
        {
            cornerMaxPoints[i].emplace_back (FeaturePoint (frames[i], p.first, p.second));
        }
        for (auto p : cornerMinCoord[i])
        {
            cornerMinPoints[i].emplace_back (FeaturePoint (frames[i], p.first, p.second));
        }
    }


    vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> matchesBMax;
    vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> matchesBMin;
    vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> matchesCMax;
    vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> matchesCMin;

    int error = 100;

    FeatureDetection::doMatchingCircle (blobMaxPoints[0], blobMaxPoints[1], blobMaxPoints[2], blobMaxPoints[3], matchesBMax);
    cout << "matchBMax size " << matchesBMax.size() << endl;
    cout << "BMax size1 " << blobMaxPoints[0].size() << endl;
    cout << "BMax size2 " << blobMaxPoints[1].size() << endl;

    FeatureDetection::doMatchingCircle (blobMinPoints[0], blobMinPoints[1], blobMinPoints[2], blobMinPoints[3], matchesBMin);
    cout << "matchBMin size " << matchesBMin.size() << endl;

    FeatureDetection::doMatchingCircle (cornerMaxPoints[0], cornerMaxPoints[1], cornerMaxPoints[2], cornerMaxPoints[3], matchesCMax);
    cout << "matchCMax size " << matchesCMax.size() << endl;

    FeatureDetection::doMatchingCircle (cornerMinPoints[0], cornerMinPoints[1], cornerMinPoints[2], cornerMinPoints[3], matchesCMin);
    cout << "matchCMin size " << matchesCMin.size() << endl;


    RNG rng(12345);
    Scalar color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));

   // Scalar color = Scalar(0, 255, 0);
    for (auto m : matchesBMax) {
        circle (imagesRGB[0], Point (get<0>(m).col, get<0>(m).row), 3, color, -1);
        circle (imagesRGB[1], Point (get<1>(m).col, get<1>(m).row), 3, color, -1);
        circle (imagesRGB[2], Point (get<2>(m).col, get<2>(m).row), 3, color, -1);
        circle (imagesRGB[3], Point (get<3>(m).col, get<3>(m).row), 3, color, -1);

        color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));
    }
//    color = Scalar(255, 0, 0);
//    for (auto m : matchesBMin) {
//        circle (imagesRGB[0], Point (m.first.col, m.first.row), 3, color, -1);
//        circle (imagesRGB[1], Point (m.second.col, m.second.row), 3, color, -1);
//        color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));
//    }
//    color = Scalar(0, 0, 255);
//    for (auto m : matchesCMax) {
//        circle (imagesRGB[0], Point (m.first.col, m.first.row), 3, color, -1);
//        circle (imagesRGB[1], Point (m.second.col, m.second.row), 3, color, -1);
//        color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));
//    }
//    color = Scalar(0, 255, 255);
//    for (auto m : matchesCMin) {
//        circle (imagesRGB[0], Point (m.first.col, m.first.row), 3, color, -1);
//        circle (imagesRGB[1], Point (m.second.col, m.second.row), 3, color, -1);
//        color = Scalar(rng.uniform(10 ,255), rng.uniform(10, 255), rng.uniform(10, 255));
//    }


  for (int i = 0; i < filenames.size(); i++) {
    namedWindow ("image" + to_string(i), WINDOW_AUTOSIZE);
    imshow ("image" + to_string(i), imagesRGB[i]);
  }


    waitKey ();
    return 0;
}
