#include "pch.h"

#include "cube.h"

Cube::Cube(Vec3 center, double side_len, EulerAngles rotation) noexcept
        : center_{center}, side_len_{side_len}, basis_{}, world_to_local_{}
{
    // Calculate the basis vectors from the rotation.
    rotation.x = deg2rad(rotation.x);
    rotation.y = deg2rad(rotation.y);
    rotation.z = deg2rad(rotation.z);
    basis_ = generate_basis(rotation);

    std::cout << "Cube basis x = { " << basis_.c[0].x << ", " << basis_.c[0].y << ", " << basis_.c[0].z << " }\n";
    std::cout << "Cube basis y = { " << basis_.c[1].x << ", " << basis_.c[1].y << ", " << basis_.c[1].z << " }\n";
    std::cout << "Cube basis z = { " << basis_.c[2].x << ", " << basis_.c[2].y << ", " << basis_.c[2].z << " }\n";

    Mat3 bt = basis_.transpose();
    std::cout << "Cube basis transpose x = { " << bt.c[0].x << ", " << bt.c[0].y << ", " << bt.c[0].z << " }\n";
    std::cout << "Cube basis transpose y = { " << bt.c[1].x << ", " << bt.c[1].y << ", " << bt.c[1].z << " }\n";
    std::cout << "Cube basis transpose z = { " << bt.c[2].x << ", " << bt.c[2].y << ", " << bt.c[2].z << " }\n";

    // Calculate the world to local matrix from the basis vectors and the center position.
    world_to_local_ = inverse_coord_transform(basis_, center_);

    std::cout << "World to local rotation x = { " << world_to_local_.rotation.c[0].x << ", " << world_to_local_.rotation.c[0].y << ", " << world_to_local_.rotation.c[0].z << "\n";
    std::cout << "World to local rotation y = { " << world_to_local_.rotation.c[1].x << ", " << world_to_local_.rotation.c[1].y << ", " << world_to_local_.rotation.c[1].z << "\n";
    std::cout << "World to local rotation z = { " << world_to_local_.rotation.c[2].x << ", " << world_to_local_.rotation.c[2].y << ", " << world_to_local_.rotation.c[2].z << "\n";
    std::cout << "World to local translation = { " << world_to_local_.translation.x << ", " << world_to_local_.translation.y << ", " << world_to_local_.translation.z << " }\n";

    Vec3 lorg = world_to_local_.rotation * center_ + world_to_local_.translation;
    std::cout << "Local origin = { " << lorg.x << ", " << lorg.y << ", " << lorg.z << " }\n";
}

double Cube::sdf(const Vec3& p) const noexcept {
    // Transform the point from world coordinates to local coordinates.
    Vec3 p2 = world_to_local_.rotation * p + world_to_local_.translation;
    // std::cout << "Cube::sdf: p_local = { " << p2.x << ", " << p2.y << ", " << p2.z << " }\n";

    // 1. Calculate half-extent
    double b = side_len_ * 0.5f;

    // 2. Component-wise distance to the faces
    double dx = std::abs(p2.x) - b;
    double dy = std::abs(p2.y) - b;
    double dz = std::abs(p2.z) - b;
    // std::cout << "dx = " << dx << "\n";
    // std::cout << "dy = " << dy << "\n";
    // std::cout << "dz = " << dz << "\n";

    // 3. Distance to the outside of the cube
    double d2d_outside = std::sqrt(
        std::max(dx, 0.0) * std::max(dx, 0.0) +
        std::max(dy, 0.0) * std::max(dy, 0.0) +
        std::max(dz, 0.0) * std::max(dz, 0.0)
    );
    // std::cout << "d2d_outside = " << d2d_outside << "\n";

    // Distance when inside the box (nearest face)
    double d2d_inside = std::min(std::max(dx, std::max(dy, dz)), 0.0);
    // std::cout << "d2d_inside = " << d2d_inside << "\n";

    return d2d_outside + d2d_inside;
}