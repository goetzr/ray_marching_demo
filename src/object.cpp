#include "pch.h"

#include "object.h"

double Object::sdf(const Vec3& p) const noexcept {
    static constexpr double kMaxDistance = 10000;

    switch (type_) {
        case ObjectType::Unused:
            return kMaxDistance;
        case ObjectType::Sphere:
            return sphere_.sdf(p);
        default:
            std::terminate();
    }
}