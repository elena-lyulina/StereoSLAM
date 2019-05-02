#pragma once

enum pointType
{
    BLOB_MAX,
    BLOB_MIN,
    CORNER_MAX,
    CORNER_MIN,

    TYPE_AMOUNT

};

enum frameNumber
{
    LEFT_PRED,
    RIGHT_PRED,
    RIGHT_SUCC,
    LEFT_SUCC,
    FRAME_AMOUNT

};

struct parameters
{
    double f; // focal length in pixels
    double cu; // principal point (u-coordinate) in pixels
    double cv; // principal point (v-coordinate) in pixels
    double base; // baseline in meters
};
