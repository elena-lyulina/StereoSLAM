#include "src/slam/Frame.h"
#include <benchmark/benchmark.h>
#include <memory>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>


class SuppressionFixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        const char *filename = PATH_TO_IMG;
        cv::Mat image = cv::imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
        cv::Mat result = cv::Mat (image.rows, image.cols, CV_8UC3, cv::Scalar::all (0));
        Frame frame = Frame (image);

        Mat = frame.doBlobConvolution (result);
        suppressed = std::vector<std::pair<int, int>> ();
    }
    int loc = 20;
    cv::Mat Mat;
    std::vector<std::pair<int, int>> suppressed;


    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (SuppressionFixture, BM_Suppression2D) (benchmark::State &state)
{
    for (auto _ : state)
    {
        Frame::suppression2D (loc, cv::Mat, suppressed, std::greater<int> ());
    }
}

BENCHMARK_REGISTER_F (SuppressionFixture, BM_Suppression2D);


class ConvolutionFixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        const char *filename = PATH_TO_IMG;
        image = cv::imread (filename, CV_LOAD_IMAGE_GRAYSCALE);
        result = cv::Mat (image.rows, image.cols, CV_8UC3, cv::Scalar::all (0));
        frame = std::make_unique<Frame> (image);
    }
    cv::Mat image;
    cv::Mat result;


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
        frame.doBlobConvolution (result);
    }
}

BENCHMARK_REGISTER_F (ConvolutionFixture, BM_BlobConvolution);


BENCHMARK_MAIN ();
