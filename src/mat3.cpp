#include "pch.h"

#include "mat3.h"

Mat3 Mat3::inverse() const noexcept {
    Mat3 cofactor = {
        Vec3 {
            c[1].y * c[2].z - c[2].y * c[1].z,
            -(c[1].x * c[2].z - c[2].x * c[1].z),
            c[1].x * c[2].y - c[2].x * c[1].y },
        Vec3 {
            -(c[0].y * c[2].z - c[2].y * c[0].z),
            c[0].x * c[2].z - c[2].x * c[0].z,
            -(c[0].x * c[2].y - c[2].x * c[0].y),
        },
        Vec3 {
            c[0].y * c[1].z - c[1].y * c[0].z
            -(c[0].x * c[1].z - c[1].x * c[0].z),
            c[0].x * c[1].y - c[1].x * c[0].y
        }
    };
    Mat3 adjugate = cofactor.transpose();
    return (1 / determinant()) * adjugate;
}