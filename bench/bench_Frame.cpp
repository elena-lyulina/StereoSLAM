#include <Frame.h>
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
        Frame frame = Frame (image);

        blob = frame.getBlobConvolution ();
        suppressedNonMax = std::vector<std::pair<int, int>> ();
        suppressedNonMin = std::vector<std::pair<int, int>> ();
    }

    int loc = 20;
    cv::Mat blob;
    std::vector<std::pair<int, int>> suppressedNonMax;
    std::vector<std::pair<int, int>> suppressedNonMin;


    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (SuppressionFixture, BM_Suppression2D) (benchmark::State &state)
{
    for (auto _ : state)
    {
        Frame::suppression2D (loc, blob, suppressedNonMax, suppressedNonMin);
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
        frame->doBlobConvolution (result);
    }
}

BENCHMARK_REGISTER_F (ConvolutionFixture, BM_BlobConvolution);


BENCHMARK_MAIN ();
