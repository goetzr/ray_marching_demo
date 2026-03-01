#pragma once

#include "vec3.h"

class Sphere  {
public:
    Sphere(const Vec3& center, double radius) noexcept
        : center_{center}, radius_{radius}
    {}

    double sdf(const Vec3& p) const noexcept {
        return (p - center_).length() - radius_;
    }

private:
    Vec3 center_;
    double radius_;
};