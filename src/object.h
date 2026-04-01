#pragma once

#include "vec3.h"
#include "sphere.h"
#include "cube.h"
#include "box2d.h"

enum class ObjectType {
    Unused,
    Sphere,
    Cube,
    Box2D
};

class Object {
public:
    Object() noexcept : type_{ObjectType::Unused}, unused_{} {}
    Object(Sphere&& sphere) noexcept : type_{ObjectType::Sphere}, sphere_{sphere} {}
    Object(Cube&& cube) noexcept : type_{ObjectType::Cube}, cube_{cube} {}
    Object(Box2D&& box2d) noexcept : type_{ObjectType::Box2D}, box2d_{box2d} {}
    double sdf(const Vec3& p) const noexcept;

private:
    ObjectType type_;
    union {
        char unused_;
        Sphere sphere_;
        Cube cube_;
        Box2D box2d_;
    };
};