#include "pch.h"

#include "camera.h"
#include "mat3.h"

Camera::Camera(double sensor_aspect_ratio, double fov_horiz, SensorFit sensor_fit, double clip_near, double clip_far,
           Vec3 pos, double rot_x, double rot_y, double rot_z) noexcept
        : sensor_aspect_ratio_{sensor_aspect_ratio}, fov_{}, sensor_fit_{sensor_fit},
          clip_near_{clip_near}, clip_far_{clip_far}, pos_{pos}, i_hat_{}, j_hat_{}, k_hat_{},
          world_to_camera_{}
{
    fov_.horiz = deg2rad(fov_horiz);
    fov_.vert = deg2rad(fov_horiz / sensor_aspect_ratio);

    rot_x = deg2rad(rot_x);
    rot_y = deg2rad(rot_y);
    rot_z = deg2rad(rot_z);

    // The camera starts with the same basis vectors as the world coordinate system.
    Mat3 basis {
        Vec3 { 1, 0, 0 },    // i_hat (right)
        Vec3 { 0, 1, 0 },    // j_hat (up)
        Vec3 { 0, 0, 1 }     // k_hat (backward)
    };

    // Point thumb in the direction of the positive axis. Fingers curl in the direction of the rotation.
    Mat3 r_x {
        Vec3 { 1, 0, 0 },
        Vec3 { 0, std::cos(rot_x), std::sin(rot_x) },
        Vec3 { 0, -std::sin(rot_x), std::cos(rot_x) }
    };

    Mat3 r_y {
        Vec3 { std::cos(rot_y), 0, -std::sin(rot_y) },
        Vec3 { 0, 1, 0 },
        Vec3 { std::sin(rot_y), 0, std::cos(rot_y) }
    };

    Mat3 r_z {
        Vec3 { std::cos(rot_z), std::sin(rot_z), 0 },
        Vec3 { -std::sin(rot_z), std::cos(rot_z), 0 },
        Vec3 { 0, 0, 1 }
    };

    // std::cout << "r_x, i_hat = { " << r_x.c[0].x << ", " << r_x.c[0].y << ", " << r_x.c[0].z << " }\n";
    // std::cout << "r_x, j_hat = { " << r_x.c[1].x << ", " << r_x.c[1].y << ", " << r_x.c[1].z << " }\n";
    // std::cout << "r_x, k_hat = { " << r_x.c[2].x << ", " << r_x.c[2].y << ", " << r_x.c[2].z << " }\n";

    // std::cout << "r_y, i_hat = { " << r_y.c[0].x << ", " << r_y.c[0].y << ", " << r_y.c[0].z << " }\n";
    // std::cout << "r_y, j_hat = { " << r_y.c[1].x << ", " << r_y.c[1].y << ", " << r_y.c[1].z << " }\n";
    // std::cout << "r_y, k_hat = { " << r_y.c[2].x << ", " << r_y.c[2].y << ", " << r_y.c[2].z << " }\n";

    // std::cout << "r_z, i_hat = { " << r_z.c[0].x << ", " << r_z.c[0].y << ", " << r_z.c[0].z << " }\n";
    // std::cout << "r_z, j_hat = { " << r_z.c[1].x << ", " << r_z.c[1].y << ", " << r_z.c[1].z << " }\n";
    // std::cout << "r_z, k_hat = { " << r_z.c[2].x << ", " << r_z.c[2].y << ", " << r_z.c[2].z << " }\n";

    // XYZ Euler.
    Mat3 new_basis = r_z * r_y * r_x * basis;

    i_hat_ = new_basis.c[0];
    j_hat_ = new_basis.c[1];
    k_hat_ = new_basis.c[2];

    // std::cout << "i_hat = { " << i_hat_.x << ", " << i_hat_.y << ", " << i_hat_.z << " }\n";
    // std::cout << "j_hat = { " << j_hat_.x << ", " << j_hat_.y << ", " << j_hat_.z << " }\n";
    // std::cout << "k_hat = { " << k_hat_.x << ", " << k_hat_.y << ", " << k_hat_.z << " }\n";

    // NOTE: This is unnecessary for a raymarcher. The desired FOV is specified directly.
    // fov_.horiz = 2 * std::atan2(sensor_sz_.width  * kInchesToMm / 2,  focal_len_);
    // fov_.vert  = 2 * std::atan2(sensor_sz_.height * kInchesToMm / 2, focal_len_);

    // NOTE: Unnecessary for a raymarcher.
    // canvas_sz_.width  = 2 * std::tan(fov_horiz_ / 2) * clip_near_;
    // canvas_sz_.height = 2 * std::tan(fov_vert_  / 2) * clip_near_;

    // Calculate the world to camera matrix from the basis vectors and the position.
    calc_world_to_camera();
}

void Camera::calc_world_to_camera() noexcept {
    // Because the rotation matrix is orthonormal, its inverse is its transpose.
    Mat3 rot = { i_hat_, j_hat_, k_hat_ };
    world_to_camera_.rotation = rot.transpose();

    // Invert the translation (position) using the inverted rotation matrix.
    world_to_camera_.translation = -(world_to_camera_.rotation * pos_);
}