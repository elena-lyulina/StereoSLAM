#pragma once
#include <Frame.h>
#include <util.h>

class FeaturePoint
{
    public:
    FeaturePoint (Frame &frame, int row, int col);
    int uniqueIdentifier;
    int age;
    int currentPosition;
    int featureStrength;
    pointType clazz;
    int row;
    int col;
    Frame &frame;

    static const int mdSize = 48;
    alignas (32) uint8_t matchingDescriptor[mdSize];

    static const int rdSize = 30;
    alignas (32) uint8_t refinementDescriptor[rdSize];

    const int w;
    const int h;
};