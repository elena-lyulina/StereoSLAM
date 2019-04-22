#include <FeatureMatcher.h>
#include <FeaturePoint.h>
#include <Frame.h>
#include <benchmark/benchmark.h>
#include <immintrin.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <random>


class SAD_32_Fixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        std::random_device rd;
        std::mt19937 gen (rd ());
        std::uniform_int_distribution<> dis (-84 * 255, 84 * 255);

        for (int i = 0; i < n; i++)
        {
            arr1[i] = dis (gen);
            arr2[i] = dis (gen);
        }
    }

    static const int n = FeaturePoint::mdSize;
    alignas (32) int32_t arr1[n];
    alignas (32) int32_t arr2[n];


    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (SAD_32_Fixture, BM_SAD_32) (benchmark::State &state)
{
    for (auto _ : state)
    {
        FeatureMatcher::SAD_32x48 (arr1, arr2);
    }
}

BENCHMARK_REGISTER_F (SAD_32_Fixture, BM_SAD_32);


class SAD_8_Fixture : public benchmark::Fixture
{
    public:
    void SetUp (const ::benchmark::State &state)
    {
        std::random_device rd;
        std::mt19937 gen (rd ());
        std::uniform_int_distribution<> dis (0, 255);

        for (int i = 0; i < n; i++)
        {
            arr1[i] = dis (gen);
            arr2[i] = dis (gen);
        }
    }

    static const int n = FeaturePoint::mdSize;
    alignas (32) uint8_t arr1[n];
    alignas (32) uint8_t arr2[n];


    void TearDown (const ::benchmark::State &state)
    {
    }
};


BENCHMARK_DEFINE_F (SAD_8_Fixture, BM_SAD_8) (benchmark::State &state)
{
    for (auto _ : state)
    {
        FeatureMatcher::SAD_8x48 (arr1, arr2);
    }
}

BENCHMARK_REGISTER_F (SAD_8_Fixture, BM_SAD_8);

BENCHMARK_MAIN ();
