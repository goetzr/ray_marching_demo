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
            len = 1;
        }
        return { x / len, y / len, z / len };
    }

    Vec3 abs() const noexcept {
        return { std::abs(x), std::abs(y), std::abs(z) };
    }

    double x;
    double y;
    double z;
};

constexpr Vec3 operator-(const Vec3& v) noexcept {
    return { -v.x, - v.y, -v.z };
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

constexpr Vec3 vmin(const Vec3& a, const Vec3& b) noexcept {
    return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
}

constexpr Vec3 vmax(const Vec3& a, const Vec3& b) noexcept {
    return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
}

constexpr Vec3 vzero() noexcept {
    return { 0, 0, 0 };
}

constexpr bool equal(const Vec3& lhs, const Vec3& rhs, double threshold=0.000001) noexcept {
    return std::abs(lhs.x - rhs.x) < threshold &&
           std::abs(lhs.y - rhs.y) < threshold &&
           std::abs(lhs.z - rhs.z) < threshold;
}