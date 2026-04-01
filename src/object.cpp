#include "pch.h"

#include "object.h"

double Object::sdf(const Vec3& p) const noexcept {
    static constexpr double kMaxDistance = 10000;

    switch (type_) {
        case ObjectType::Unused:
            return kMaxDistance;
        case ObjectType::Sphere:
            return sphere_.sdf(p);
        case ObjectType::Cube:
            return cube_.sdf(p);
        case ObjectType::Box2D:
            return box2d_.sdf(p);
        default:
            std::cerr << "Object::sdf: unknown object type\n";
            std::terminate();
    }
}