#include <gtest/gtest.h>
using namespace std;

TEST(FeatureDetection, abs) {
    vector<int> SAD {-1, 3, -45, 0, 2};
    int (*absolute)(int) = &abs;
    transform(SAD.begin(), SAD.end(), SAD.begin(), absolute);
    for (auto e : SAD) cout << e << " ";
}


int main (int argc, char **argv)
{
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}