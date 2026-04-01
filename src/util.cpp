#include "pch.h"

#include "util.h"

Mat3 generate_basis(EulerAngles rotation) {
    // Point your thumb along the axis away from the origin.
    // Then curl your fingers in the direction of the rotation.
    Mat3 rot_x {
        Vec3 { 1, 0, 0 },
        Vec3 { 0, std::cos(rotation.x), std::sin(rotation.x) },
        Vec3 { 0, -std::sin(rotation.x), std::cos(rotation.x) }
    };

    Mat3 rot_y {
        Vec3 { std::cos(rotation.y), 0, -std::sin(rotation.y) },
        Vec3 { 0, 1, 0 },
        Vec3 { std::sin(rotation.y), 0, std::cos(rotation.y) }
    };

    Mat3 rot_z {
        Vec3 { std::cos(rotation.z), std::sin(rotation.z), 0 },
        Vec3 { -std::sin(rotation.z), std::cos(rotation.z), 0 },
        Vec3 { 0, 0, 1 }
    };

    // Standard basis.
    Mat3 basis  = {
        Vec3 { 1, 0, 0 },
        Vec3 { 0, 1, 0 },
        Vec3 { 0, 0, 1 }
    };

    // XYZ Euler.
    return rot_z * rot_y * rot_x * basis;
}

CoordTransform inverse_coord_transform(Mat3 rotation, Vec3 translation) noexcept {
    // Because the rotation matrix is orthonormal, its inverse is its transpose.
    Mat3 inv_rotation = rotation.transpose();

    // Invert the translation using the inverted rotation matrix.
    Vec3 inv_translation = -(inv_rotation * translation);

    return { inv_rotation, inv_translation };
}