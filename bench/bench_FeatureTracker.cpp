#include <FeatureTracker.h>
#include <Frame.h>
#include <benchmark/benchmark.h>

static void BM_all (benchmark::State &state)
{
    for (auto _ : state)
    {
        std::pair<Frame, Frame> prevImages = { Frame (PATH_TO_IMG), Frame (PATH_TO_IMG) };
        std::pair<Frame, Frame> succImage = { Frame (PATH_TO_IMG), Frame (PATH_TO_IMG) };

        FeatureTracker ft (prevImages, succImage);
    }
}


BENCHMARK (BM_all);

BENCHMARK_MAIN ();