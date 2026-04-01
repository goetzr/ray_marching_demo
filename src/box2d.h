#pragma once

#include "vec3.h"
#include "common_defs.h"

class Box2D  {
public:
    Box2D(const Vec3& center, RectF size, EulerAngles rotation) noexcept;
    double sdf(const Vec3& p) const noexcept;
    Mat3 basis() const noexcept { return basis_; }

private:
    Vec3 center_;
    RectF size_;
    // The cube's basis vectors, in world coordinates.
    Mat3 basis_;
    // The 4x4 world to local matrix.
    CoordTransform world_to_local_;
};