#pragma once

#include "vec3.h"

struct Mat3 {
    Vec3 c[3];

    Mat3 transpose() const noexcept {
        return {
            Vec3 { c[0].x, c[0].y, c[0].z },
            Vec3 { c[1].x, c[1].y, c[1].z },
            Vec3 { c[2].x, c[2].y, c[2].z }
        };
    }

    double determinant() const noexcept {
        return c[0].x * (c[1].y * c[2].z - c[2].y * c[1].z) -
               c[1].x * (c[0].y * c[2].z - c[2].y * c[0].z) +
               c[2].x * (c[0].y * c[1].z - c[1].y * c[0].z);
    }

    Mat3 inverse() const noexcept;
};

constexpr Vec3 operator*(const Mat3& m, const Vec3& v) noexcept {
    return v.x * m.c[0] + v.y * m.c[1] +  v.z * m.c[2];
}

constexpr Mat3 operator*(const Mat3& lhs, const Mat3& rhs) noexcept {
    return { lhs * rhs.c[0], lhs * rhs.c[1], lhs * rhs.c[2] };
}

constexpr Mat3 operator*(double s, const Mat3& m) noexcept {
    return { s * m.c[0], s * m.c[1], s * m.c[2] };
}

constexpr Mat3 operator*(const Mat3& m, double s) noexcept {
    return s * m;
}