#include "pch.h"

#include "box2d.h"
#include "util.h"
#include "vec2.h"

Box2D::Box2D(const Vec3& center, RectF size, EulerAngles rotation) noexcept
        : center_{center}, size_{size}, basis_{}, world_to_local_{}
{
    // Calculate the basis vectors from the rotation.
    rotation.x = deg2rad(rotation.x);
    rotation.y = deg2rad(rotation.y);
    rotation.z = deg2rad(rotation.z);
    basis_ = generate_basis(rotation);

    std::cout << "Box2D basis x = { " << basis_.c[0].x << ", " << basis_.c[0].y << ", " << basis_.c[0].z << " }\n";
    std::cout << "Box2D basis y = { " << basis_.c[1].x << ", " << basis_.c[1].y << ", " << basis_.c[1].z << " }\n";
    std::cout << "Box2D basis z = { " << basis_.c[2].x << ", " << basis_.c[2].y << ", " << basis_.c[2].z << " }\n";
    // std::cout << "Box2D basis lengths: x = " << basis_.c[0].length() << ", y = " << basis_.c[1].length() << ", z = " << basis_.c[2].length() << "\n";
    // std::cout << "Box2D basis x * y = " << dot(basis_.c[0], basis_.c[1]) << "\n";
    // std::cout << "Box2D basis x * z = " << dot(basis_.c[0], basis_.c[2]) << "\n";
    // std::cout << "Box2D basis y * z = " << dot(basis_.c[1], basis_.c[2]) << "\n";
    // std::cout << "Box2D basis determinant = " << basis_.determinant() << "\n";

    double w = size.width / 2.0;
    double h = size.height / 2.0;
    Vec3 ul_world = basis_ * Vec3 { -w, h, 0 } + center;
    Vec3 ur_world = basis_ * Vec3 { w, h, 0 } + center;
    Vec3 ll_world = basis_ * Vec3 { -w, -h, 0 } + center;
    Vec3 lr_world = basis_ * Vec3 { w, -h, 0 } + center;
    std::cout << "World coordinates of corners of box:\n";
    std::cout << "\tUL = { " << ul_world.x << ", " << ul_world.y << ", " << ul_world.z << " }\n";
    std::cout << "\tUR = { " << ur_world.x << ", " << ur_world.y << ", " << ur_world.z << " }\n";
    std::cout << "\tLL = { " << ll_world.x << ", " << ll_world.y << ", " << ll_world.z << " }\n";
    std::cout << "\tLR = { " << lr_world.x << ", " << lr_world.y << ", " << lr_world.z << " }\n";

    // Calculate the world to local matrix from the basis vectors and the center position.
    world_to_local_ = inverse_coord_transform(basis_, center_);
}

double Box2D::sdf(const Vec3& p) const noexcept {
    Vec3 p2 = world_to_local_.rotation * p + world_to_local_.translation;
    // std::cout << "Box2D::sdf: p_local = { " << p2.x << ", " << p2.y << ", " << p2.z << " }\n";

    // 1. Calculate 2D half-extents
    double bx = size_.width * 0.5f;
    double by = size_.height * 0.5f;

    // 2. Component-wise distance in the 2D plane (XY)
    double dx = std::abs(p2.x) - bx;
    double dy = std::abs(p2.y) - by;
    // std::cout << "dx = " << dx << "\n";
    // std::cout << "dy = " << dy << "\n";

    // 3. Distance to the 2D rectangle boundary within the plane
    // (This is the 2D SDF logic)
    double d2d_outside = std::sqrt(
        std::max(dx, 0.0) * std::max(dx, 0.0) +
        std::max(dy, 0.0) * std::max(dy, 0.0)
    );
    double d2d_inside = std::min(std::max(dx, dy), 0.0);
    double d2d = d2d_outside + d2d_inside;
    // std::cout << "d2d_outside = " << d2d_outside << "\n";
    // std::cout << "d2d_inside = " << d2d_inside << "\n";
    // std::cout << "d2d = " << d2d << "\n";

    // 4. Combine with the Z distance (distance to the plane)
    // For a 0-thickness object, we use the Pythagorean theorem 
    // between the 2D distance and the Z offset.
    if (d2d > 0.0f) {
        // Point is outside the rectangle's footprint
        return std::sqrt(d2d * d2d + p2.z * p2.z);
    } else {
        // Point is directly above/below the rectangle's area
        return std::abs(p2.z);
    }
}