#include "pch.h"

#include "scene.h"

std::optional<ClosestObject> Scene::closest_object(const Vec3& p) const noexcept {
    if (objects_.empty()) {
        return std::nullopt;
    }

    ClosestObject closest_elem {
        .index = 0,
        .distance = objects_[0].sdf(p)
    };

    for (int i = 1; i != objects_.size(); ++i) {
        double dist = objects_[i].sdf(p);
        if (dist < closest_elem.distance) {
            closest_elem.index = i;
            closest_elem.distance = dist;
        }
    }

    return std::make_optional<ClosestObject>(closest_elem);
}