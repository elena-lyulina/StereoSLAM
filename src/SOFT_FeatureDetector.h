#pragma once
#include <iostream>
#include <opencv2/imgproc.hpp>
#include "FeaturePoint.h"

using namespace std;
using namespace cv;

typedef tuple<const FeaturePoint*, const FeaturePoint*, const FeaturePoint*,const  FeaturePoint*> fp_tuple;

class SOFT_FeatureDetector {
 public:
  SOFT_FeatureDetector(pair<Frame, Frame> &prevImages, pair<Frame, Frame> &succImages);
  vector<fp_tuple> getMatchingPoints();

  vector<fp_tuple> getBlobMaxMatchingPoints();
  vector<fp_tuple> getBlobMinMatchingPoints();
  vector<fp_tuple> getCornerMaxMatchingPoints();
  vector<fp_tuple> getCornerMinMatchingPoints();

  void showMP();


 private:
  pair<Frame, Frame> &prevImages;
  pair<Frame, Frame> &succImages;
  static void doMatchingCircle (const vector<FeaturePoint> &leftPred,
                                const vector<FeaturePoint> &rightPred,
                                const vector<FeaturePoint> &rightSucc,
                                const vector<FeaturePoint> &leftSucc,
                                vector<fp_tuple> &matched);


  vector<fp_tuple> blobMaxMatchingPoints;
  vector<fp_tuple> blobMinMatchingPoints;
  vector<fp_tuple> cornerMaxMatchingPoints;
  vector<fp_tuple> cornerMinMatchingPoints;



};


