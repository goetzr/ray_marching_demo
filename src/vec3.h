#pragma once

struct Vec3 {
    constexpr double length_squared() const noexcept {
        return x * x + y * y + z * z;
    }

    double length() const noexcept {
        return std::sqrt(length_squared());
    }

    Vec3 normalize() const noexcept {
        double len = length();
        if (len == 0) {
            return { 0, 0, 0 };
        }
        return { x / len, y / len, z / len };
    }

    double x;
    double y;
    double z;
};

constexpr Vec3 operator-(const Vec3& v) noexcept {
    return { -v.x, -v.y, -v.z };
}

constexpr Vec3 operator+(const Vec3& a, const Vec3& b) noexcept {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

constexpr Vec3 operator-(const Vec3& a, const Vec3& b) noexcept {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

constexpr Vec3 operator*(const Vec3& v, double d) {
    return { v.x * d, v.y * d, v.z * d };
}

constexpr Vec3 operator*(double d, const Vec3& v) {
    return v * d;
}

constexpr double dot(const Vec3& a, const Vec3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}