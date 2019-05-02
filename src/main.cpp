#include <DepthGetter.h>
#include <FeatureTracker.h>
#include <Frame.h>
#include <glog/logging.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main (int argc, char *argv[])
{
    google::InitGoogleLogging (argv[0]);

    // we need the path name to 2010_03_09_drive_0019 as input argument
    if (argc < 2)
    {
        cerr << "Usage: ./viso2 path/to/sequence/2010_03_09_drive_0019" << endl;
        return 1;
    }

    string dir = argv[1];


    // load 2 pair of stereo-images
    const int n = 4;
    vector<pair<Frame, Frame>> frames;

    int start_index = 57;

    for (int i = 0; i < n / 2; i++)
    {
        cout << i << "\n";
        // input file names
        char base_name[256];
        sprintf (base_name, "%06d.png", start_index + i);
        string left_file_name = dir + "/I1_" + base_name;
        string right_file_name = dir + "/I2_" + base_name;

        const char *left = &left_file_name[0];
        const char *right = &right_file_name[0];

        try
        {
            // load left and right input image
            frames.emplace_back (Frame (left), Frame (right));
        }
        // catch image read/write errors here
        catch (...)
        {
            cerr << "ERROR: Couldn't read input files!" << endl;
            return 1;
        }
    }

    FeatureTracker ft (frames[0], frames[1]);

    parameters param;
    param.f = 645.24;
    param.cu = 635.96;
    param.cv = 194.13;
    param.base = 0.5707;

    DepthGetter dg (param);


    pointType types[TYPE_AMOUNT] = { BLOB_MAX, BLOB_MIN, CORNER_MAX, CORNER_MIN };
    ft.showMP (TYPE_AMOUNT, types);
    ft.showNMP (TYPE_AMOUNT, types);

    cv::waitKey ();
}
