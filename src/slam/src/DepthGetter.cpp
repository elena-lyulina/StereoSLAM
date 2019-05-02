#include "DepthGetter.h"
#include <fstream>
#include <opencv2/highgui.hpp>

DepthGetter::DepthGetter (parameters param) : param (param)
{
}

void DepthGetter::getDepths (std::vector<fp_tuple> *matchedPoints, std::string file, cv::Mat rgbImage)
{
    std::ofstream out;
    out.open (file);

    if (out.is_open ())
    {
        out << "ply" << '\n';
        out << "format ascii 1.0"
            << "\n";
        out << "element vertex "
            << matchedPoints[BLOB_MAX].size () + matchedPoints[BLOB_MIN].size () +
               matchedPoints[CORNER_MAX].size () + matchedPoints[CORNER_MIN].size ()
            << "\n";

        out << "property float x"
            << "\n";
        out << "property float y"
            << "\n";
        out << "property float z"
            << "\n";

        out << "property uchar red"
            << "\n";
        out << "property uchar green"
            << "\n";
        out << "property uchar blue"
            << "\n";


        out << "end_header"
            << "\n";

        for (int i = 0; i < TYPE_AMOUNT; i++)
        {
            for (int j = 0; j < matchedPoints[i].size (); j++)
            {
                int u1 = std::get<LEFT_PRED> (matchedPoints[i][j])->col;
                int v1 = std::get<LEFT_PRED> (matchedPoints[i][j])->row;
                int u2 = std::get<RIGHT_PRED> (matchedPoints[i][j])->col;

                int disparity = std::max (u1 - u2, 1);

                double z = param.f * param.base / disparity;
                double x = ((double)u1 - param.cu) * param.base / disparity;
                double y = ((double)v1 - param.cv) * param.base / disparity;

                out << x << " " << y << " " << z << " ";


                if (rgbImage.channels () == 3)
                {
                    cv::Vec3b bgr = rgbImage.at<cv::Vec3b> (u1, v1);
                    out << +bgr[2] << " " << +bgr[1] << " " << +bgr[0] << "\n";
                }
                else if (rgbImage.channels () == 1)
                {
                    int intenc = rgbImage.at<char> (u1, v1);
                    out << +intenc << " " << +intenc << " " << +intenc << "\n";
                }
            }
        }
    }
    out.close ();
}

void DepthGetter::showDisparity (std::vector<fp_tuple> *matchedPoints)
{
    Frame *frame = &std::get<LEFT_PRED> (matchedPoints[0][0])->frame;
    cv::Mat imageRGB =
    cv::Mat (frame->getImage ().rows, frame->getImage ().cols, CV_8UC3, cv::Scalar::all (0));
    cvtColor (frame->getImage (), imageRGB, cv::COLOR_GRAY2BGR);


    for (int i = 0; i < TYPE_AMOUNT; i++)
    {
        for (int j = 0; j < matchedPoints[i].size (); j++)
        {
            int u1 = std::get<LEFT_PRED> (matchedPoints[i][j])->col;
            int v1 = std::get<LEFT_PRED> (matchedPoints[i][j])->row;

            int u2 = std::get<RIGHT_PRED> (matchedPoints[i][j])->col;
            int v2 = std::get<RIGHT_PRED> (matchedPoints[i][j])->row;

            int disparity = std::max (u1 - u2, 1);

            cv::Scalar color = cv::Scalar (0, disparity, 0);

            circle (imageRGB, cv::Point (u1, v1), 3, color, -1);
        }

        namedWindow ("depth", cv::WINDOW_AUTOSIZE);
        imshow ("depth", imageRGB);
    }
}
