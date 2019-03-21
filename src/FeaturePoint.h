#include "Frame.h"

#ifndef STEREOSLAM_FEATUREPOINT_H
#define STEREOSLAM_FEATUREPOINT_H

#endif //STEREOSLAM_FEATUREPOINT_H

class FeaturePoint {
public:
    FeaturePoint(Frame frame, int x, int y);
    vector<int> descriptor;
    int uniqueIdentifier;
    int age;
    int currentPosition;
    int featureStrength;
    int clazz;
};