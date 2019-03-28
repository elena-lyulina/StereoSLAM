#include "../src/FeatureDetection.h"
#include <benchmark/benchmark.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>

// static void CustomArguments(benchmark::internal::Benchmark* b) {
//  const char *filename = "/home/elena/workspaces/c++/StereoSLAM/res/1big.png";
//  Mat image = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
//  Mat result = Mat(image.rows, image.cols, CV_8UC3, Scalar::all (0));
//  Frame frame = Frame(image);
//}
//
// void BM_FeatureMatching_args(benchmark::State& state, vector<FeaturePoint> candidates1,
//                             vector<FeaturePoint> candidates2,
//                             vector<pair<FeaturePoint, FeaturePoint>> &matched,
//                             int error) {
//  for (auto _ : state) {
//    FeatureDetection::findMatches (candidates1, candidates2, matched, error);
//  }
//}
//
// BENCHMARK_CAPTURE(BM_FeatureMatching_args, vector<FeaturePoint>(), vector<FeaturePoint>(), vector<pair<FeaturePoint, FeaturePoint>>(), 10);

static void BM_FeatureMatching (benchmark::State &state)
{
    for (auto _ : state)
    {
        int loc = 20;
        int error = 100;
        const char *filename = "/home/elena/workspaces/c++/StereoSLAM/res/lp.png";
        Mat image = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
        Mat result = Mat (image.rows, image.cols, CV_8UC3, Scalar::all (0));
        Frame frame = Frame (image);
        vector<pair<int, int>> blobMaxCoord;
        Frame::suppression2D (loc, frame.getBlobConvolution (result), blobMaxCoord, greater<int> ());
        vector<FeaturePoint> blobMaxPoints;
        for (auto p : blobMaxCoord)
        {
            blobMaxPoints.emplace_back (FeaturePoint (frame, p.first, p.second));
        }

        vector<tuple<FeaturePoint, FeaturePoint, FeaturePoint, FeaturePoint>> matchesBMax;


        FeatureDetection::doMatchingCircle (blobMaxPoints, blobMaxPoints, blobMaxPoints, blobMaxPoints, matchesBMax);
    }
}

BENCHMARK (BM_FeatureMatching);


BENCHMARK_MAIN ();
