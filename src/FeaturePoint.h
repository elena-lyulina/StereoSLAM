#pragma once

#include "Frame.h"

class FeaturePoint
{
 public:
  FeaturePoint (Frame &frame, int row, int col);
  int uniqueIdentifier;
  int age;
  int currentPosition;
  int featureStrength;
  int clazz;
  int row;
  int col;
  Frame& frame;

  static const int matchingDescriptorSize = 48;
  int matchingDescriptor[matchingDescriptorSize];

  static const int refinementDescriptorSize = 30;
  int refinementDescriptor[refinementDescriptorSize];



};