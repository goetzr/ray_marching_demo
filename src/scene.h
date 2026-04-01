#pragma once

#include "object.h"

constexpr int MaxObjects = 100;

struct ClosestObject {
    int index;
    double distance;
};

class Scene {
public:
    Scene(std::array<Object, MaxObjects> objects)
        : objects_{objects}
    {}
    std::optional<ClosestObject> closest_object(const Vec3& p) const noexcept;

private:
    std::array<Object, MaxObjects> objects_;
};