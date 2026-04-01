#pragma once

#include "vec3.h"
#include "mat3.h"
#include "common_defs.h"

constexpr double deg2rad(double d) {
    return d * std::numbers::pi / 180;
}

constexpr double rad2deg(double r) {
    return r * 180 / std::numbers::pi;
}

Mat3 generate_basis(EulerAngles rotation) noexcept;
CoordTransform inverse_coord_transform(Mat3 rotation, Vec3 translation) noexcept;