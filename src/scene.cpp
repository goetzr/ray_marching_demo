#include "pch.h"

#include "scene.h"

ClosestElement Scene::closest_element(const Vec3& p) const noexcept {
    auto it = elements_.begin();
    double dist = (*it)->sdf(p);

    ClosestElement closest_elem {
        .index = 0,
        .distance = dist
    };

    ++it;
    for (int index = 1; it != elements_.end(); ++it, ++index) {
        double dist = (*it)->sdf(p);
        if (dist < closest_elem.distance) {
            closest_elem.index = index;
            closest_elem.distance = dist;
        }
    }

    return closest_elem;
}