#pragma once

#include "vec3.h"
#include "common_defs.h"
#include "mat3.h"

class Cube {
public:
    Cube(Vec3 center, double side_len, EulerAngles rotation) noexcept;
    double sdf(const Vec3& p) const noexcept;

private:
    // The cube's center, in world coordinates.
    Vec3 center_;
    // The side length of a face of the cube.
    double side_len_;
    // The cube's basis vectors, in world coordinates.
    Mat3 basis_;
    // The 4x4 world to local matrix.
    CoordTransform world_to_local_;
};