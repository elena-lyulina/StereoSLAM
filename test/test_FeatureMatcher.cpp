#include <FeatureMatcher.h>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

TEST (FeatureMatcher, SAD_8)
{
    const int n = 48;
    alignas (32) uint8_t arr1[n];
    alignas (32) uint8_t arr2[n];

    int times = 10000;

    for (int j = 0; j < times; j++)
    {
        std::random_device rd;
        std::mt19937 gen (rd ());
        std::uniform_int_distribution<> dis (0, 255);
        int expexted_res = 0;

        for (int i = 0; i < n; i++)
        {
            arr1[i] = dis (gen);
            arr2[i] = dis (gen);
            expexted_res += abs (arr1[i] - arr2[i]);
        }

        int res = FeatureMatcher::SAD_8x48 (arr1, arr2);

        EXPECT_EQ (expexted_res, res);
    }
}

TEST (FeatureMatcher, SAD_32x8)
{
    const int n = 8;
    alignas (32) int32_t arr1[n];
    alignas (32) int32_t arr2[n];

    int times = 10000;

    for (int j = 0; j < times; j++)
    {
        std::random_device rd;
        std::mt19937 gen (rd ());
        std::uniform_int_distribution<> dis (-84 * 255, 84 * 255);
        int expexted_res = 0;

        for (int i = 0; i < n; i++)
        {
            arr1[i] = dis (gen);
            arr2[i] = dis (gen);
            expexted_res += abs ((int)(arr1[i] - arr2[i]));
        }

        __m256i a1 = _mm256_load_si256 ((__m256i *)arr1);
        __m256i a2 = _mm256_load_si256 ((__m256i *)arr2);

        int res = FeatureMatcher::SAD_32x8 (a1, a2);

        EXPECT_EQ (expexted_res, res);
    }
}


TEST (FeatureMatcher, SAD_32)
{
    const int n = FeaturePoint::mdSize;
    alignas (32) int32_t arr1[n];
    alignas (32) int32_t arr2[n];

    int times = 10000;

    for (int j = 0; j < times; j++)
    {
        std::random_device rd;
        std::mt19937 gen (rd ());
        std::uniform_int_distribution<> dis (-84 * 255, 84 * 255);
        int expexted_res = 0;

        for (int i = 0; i < n; i++)
        {
            arr1[i] = dis (gen);
            arr2[i] = dis (gen);
            expexted_res += abs ((int)(arr1[i] - arr2[i]));
        }

        int res = FeatureMatcher::SAD_32x48 (arr1, arr2);

        EXPECT_EQ (expexted_res, res);
    }
}

int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}
