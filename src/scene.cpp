#include "pch.h"

#include "scene.h"

Result<ClosestObject, void> Scene::closest_object(const Vec3& p) const noexcept {
    if (num_objects_ == 0 ) {
        // No objects in the scene.
        return Result<ClosestObject, void>::err();
    }

    ClosestObject closest_elem {
        .index = 0,
        .distance = objects_[0].sdf(p)
    };

    for (int i = 1; i != num_objects_; ++i) {
        double dist = objects_[i].sdf(p);
        if (dist < closest_elem.distance) {
            closest_elem.index = i;
            closest_elem.distance = dist;
        }
    }

    return Result<ClosestObject, void>::ok(closest_elem);
}