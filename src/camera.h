#pragma once

#include "vec3.h"

struct Camera {
    Vec3 position;
    Vec3 forward;
    Vec3 right;
    Vec3 up;
};

inline std::string to_string(const Camera& camera) {
    // return std::format(
    //     "Camera(\n"
    //         "\tposition = {},\n"
    //         "\tforward  = {},\n"
    //         "\tright    = {},\n"
    //         "\tup       = {}\n"
    //     ")",
    //     camera.position, camera.forward, camera.right, camera.up
    // );
    return std::format("{}", "camera");
}