#include "FeaturePoint.h"
#include "Frame.h"
#include "SOFT_FeatureDetector.h"
#include "SOFT_FeatureTracker.h"
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

    pair<Frame, Frame> prevImages = { Frame (filename1), Frame (filename2) };
    pair<Frame, Frame> succImage = { Frame (filename3), Frame (filename4) };

    SOFT_FeatureTracker ft (prevImages, succImage);

    ft.showMP (BLOB_MAX);
}
