#include <gtest/gtest.h>
#include <Frame.h>

TEST (FeatureDetection, abs)
{
    std::vector<int> SAD{ -1, 3, -45, 0, 2 };
    int (*absolute) (int) = &abs;
    transform (SAD.begin (), SAD.end (), SAD.begin (), absolute);
    for (auto e : SAD)
        std::cout << e << " ";
}


int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}