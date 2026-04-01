#pragma once

#include "vec3.h"
#include "common_defs.h"

class Box2D  {
public:
    Box2D(const Vec3& center, RectF size, EulerAngles rotation) noexcept;
    double sdf(const Vec3& p) const noexcept;

private:
    // The box's center, in world coordinates.
    Vec3 center_;
    RectF size_;
    // The box's basis vectors, in world coordinates.
    Mat3 basis_;
    // The 4x4 world to local matrix.
    CoordTransform world_to_local_;
};