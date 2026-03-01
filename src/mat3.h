#pragma once

#include "vec3.h"

struct Mat3 {
    Vec3 c[3];
};

Vec3 operator*(const Mat3& m, const Vec3& v) {
    return v.x * m.c[0] + v.y * m.c[1] +  v.z * m.c[2];
}

Mat3 operator*(const Mat3& lhs, const Mat3& rhs) {
    return { lhs * rhs.c[0], lhs * rhs.c[1], lhs * rhs.c[2] };
}