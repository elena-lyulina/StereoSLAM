#include "../src/FeatureDetection.h"
#include <benchmark/benchmark.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <memory>


class SuppressionFixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        const char *filename = "/home/elena/workspaces/c++/StereoSLAM/res/lp.png";
        Mat image = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
        Mat result = Mat (image.rows, image.cols, CV_8UC3, Scalar::all (0));
        Frame frame = Frame (image);

        mat = frame.doBlobConvolution(result);
        suppressed = vector<pair<int, int>> ();
    }
    int loc = 20;
    Mat mat;
    vector<pair<int, int>> suppressed;


    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (SuppressionFixture, BM_Suppression2D) (benchmark::State &state)
{
    for (auto _ : state)
    {
        Frame::suppression2D (loc, mat, suppressed, greater<int> ());
    }
}

BENCHMARK_REGISTER_F (SuppressionFixture, BM_Suppression2D);


class ConvolutionFixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        const char *filename = "/home/elena/workspaces/c++/StereoSLAM/res/lp.png";
        image = imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
        result = Mat (image.rows, image.cols, CV_8UC3, Scalar::all (0));
        frame = std::make_unique<Frame>(image);
    }
    Mat image;
    Mat result;


    std::unique_ptr<Frame> frame;

    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (ConvolutionFixture, BM_BlobConvolution) (benchmark::State &state)
{
    for (auto _ : state)
    {
        Frame frame = Frame (image);
      frame.doBlobConvolution(result);
    }
}

BENCHMARK_REGISTER_F (ConvolutionFixture, BM_BlobConvolution);


BENCHMARK_MAIN ();
