#pragma once

class Vec3 {
public:
    constexpr Vec3() noexcept : x{0}, y{0}, z{0}
    {}

    constexpr Vec3(double x, double y, double z) noexcept
        : x{x}, y{y}, z{z}
    {}

    constexpr double length_squared() const noexcept {
        return x * x + y * y + z * z;
    }

    double length() const noexcept {
        return std::sqrt(length_squared());
    }

    Vec3 normalize() const {
        double len = length();
        return Vec3(x / len, y / len, z / len);
    }

    double x;
    double y;
    double z;
};

constexpr Vec3 operator+(const Vec3& a, const Vec3& b) noexcept {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

constexpr Vec3 operator-(const Vec3& a, const Vec3& b) noexcept {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

constexpr double dot(const Vec3& a, const Vec3& b) noexcept {
    return a.x* b.x + a.y * b.y + a.z * b.z;
}

inline std::string to_string(const Vec3& v) {
    return std::format("Vec3({}, {}, {})", v.x, v.y, v.z);
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << to_string(v);
}