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
        : objects_{objects}, num_objects_{objects.size()}
    {}
    Result<ClosestObject, void> closest_object(const Vec3& p) const noexcept;

private:
    std::array<Object, MaxObjects> objects_;
    int num_objects_;
};