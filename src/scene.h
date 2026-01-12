#pragma once

#include "vec3.h"

class Renderable {
public:
    virtual ~Renderable() = 0;
    virtual double sdf(const Vec3& p) const noexcept = 0;
};

inline Renderable::~Renderable() {}

class Sphere : public Renderable {
public:
    Sphere(const Vec3& center, double radius)
        : center_{center}, radius_{radius}
    {}
    ~Sphere() = default;

    double sdf(const Vec3& p) const noexcept override {
        return (p - center_).length() - radius_;
    }

private:
    Vec3 center_;
    double radius_;
};

struct ClosestElement {
    int index;
    double distance;
};

class Scene {
public:
    void add_element(std::unique_ptr<Renderable> r) {
        elements_.push_back(std::move(r));
    }

    ClosestElement closest_element(const Vec3& p) const noexcept;

private:
    std::vector<std::unique_ptr<Renderable>> elements_;
};