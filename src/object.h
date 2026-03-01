#pragma once

#include "vec3.h"
#include "sphere.h"

enum class ObjectType {
    Unused,
    Sphere
};

class Object {
public:
    Object() noexcept : type_{ObjectType::Unused}, unused_{} {}
    Object(Sphere&& sphere) noexcept : type_{ObjectType::Sphere}, sphere_{sphere} {}
    double sdf(const Vec3& p) const noexcept;

private:
    ObjectType type_;
    union {
        char unused_;
        Sphere sphere_;
    };
};