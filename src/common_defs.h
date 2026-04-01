#pragma once

#include "vec3.h"
#include "mat3.h"

constexpr double kPi      = std::numbers::pi;
constexpr double kPiOver2 = std::numbers::pi / 2;
constexpr double kPiOver4 = std::numbers::pi / 4;

struct RectF {
    double width;
    double height;
};

struct RectI {
    int width;
    int height;
};

struct EulerAngles {
    double x;
    double y;
    double z;
};

// A 4x4 coordinate transformation matrix, split out into its rotation matrix and translation vector.
struct CoordTransform {
    Mat3 rotation;
    Vec3 translation;
};