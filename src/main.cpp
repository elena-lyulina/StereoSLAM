#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <Frame.h>
#include <FeatureTracker.h>

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

    //    int k = 5;
    //    Frame frame (filename1);
    //
    //    vector<pair<int, int>> max, min;
    //    Frame::suppression2D (k, frame.getCornerConvolution (), max, min);
    //    cout << endl << "size: " << max.size () << " " << min.size () << endl;


    pair<Frame, Frame> prevImages = { Frame (filename1), Frame (filename2) };
    pair<Frame, Frame> succImage = { Frame (filename3), Frame (filename4) };

    FeatureTracker ft (prevImages, succImage);

    cout << ft.getCyclicallyMatchedPoints ()->size () << endl;

    pointType types[TYPE_AMOUNT] = { BLOB_MAX, BLOB_MIN, CORNER_MAX, CORNER_MIN };
    ft.showMP (TYPE_AMOUNT, types);
}
