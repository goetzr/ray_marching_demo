#include "pch.h"

#include "cube.h"
#include "util.h"

Cube::Cube(Vec3 center, double side_len, EulerAngles rotation) noexcept
        : center_{center}, side_len_{side_len}, basis_{}, world_to_local_{}
{
    // Calculate the basis vectors from the rotation.
    rotation.x = deg2rad(rotation.x);
    rotation.y = deg2rad(rotation.y);
    rotation.z = deg2rad(rotation.z);
    basis_ = generate_basis(rotation);

    // Calculate the world to local matrix from the basis vectors and the center position.
    world_to_local_ = inverse_coord_transform(basis_, center_);
}

double Cube::sdf(const Vec3& p) const noexcept {
    // Transform the point from world coordinates to local coordinates.
    Vec3 p2 = world_to_local_.rotation * p + world_to_local_.translation;

    // 1. Calculate half-extent.
    double b = side_len_ * 0.5f;

    // 2. Component-wise distance to the faces.
    double dx = std::abs(p2.x) - b;
    double dy = std::abs(p2.y) - b;
    double dz = std::abs(p2.z) - b;

    // 3. Distance to the outside of the cube.
    double d2d_outside = std::sqrt(
        std::max(dx, 0.0) * std::max(dx, 0.0) +
        std::max(dy, 0.0) * std::max(dy, 0.0) +
        std::max(dz, 0.0) * std::max(dz, 0.0)
    );

    // Distance when inside the box (nearest face).
    double d2d_inside = std::min(std::max(dx, std::max(dy, dz)), 0.0);

    return d2d_outside + d2d_inside;
}