#pragma once

#include "vec3.h"

struct Camera {
    Vec3 position;
    Vec3 forward;
    Vec3 right;
    Vec3 up;
    double fov_x;
    double fov_y;
};

inline std::string to_string(const Camera& c) {
    return std::format(
        "Camera(\n"
            "  position = {},\n"
            "  forward  = {},\n"
            "  right    = {},\n"
            "  up       = {}\n"
        ")",
        to_string(c.position), to_string(c.forward), to_string(c.right), to_string(c.up)
    );
}

inline std::ostream& operator<<(std::ostream& os, const Camera& c) {
    return os << to_string(c);
}