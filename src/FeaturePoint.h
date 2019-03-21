#include "Frame.h"
#pragma once

#ifndef STEREOSLAM_FEATUREPOINT_H
#define STEREOSLAM_FEATUREPOINT_H

#endif // STEREOSLAM_FEATUREPOINT_H

class FeaturePoint
{
    public:
    FeaturePoint (Frame frame, int row, int col);
    vector<int> descriptor;
    int uniqueIdentifier;
    int age;
    int currentPosition;
    int featureStrength;
    int clazz;
    int row;
    int col;
};