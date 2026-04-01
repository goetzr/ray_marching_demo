#pragma once

struct Vec2 {
    constexpr double length_squared() const noexcept {
        return x * x + y * y;
    }

    double length() const noexcept {
        return std::sqrt(length_squared());
    }

    Vec2 normalize() const noexcept {
        double len = length();
        if (len == 0) {
            len = 1;
        }
        return { x / len, y / len };
    }

    Vec2 abs() const noexcept {
        return { std::abs(x), std::abs(y) };
    }

    double x;
    double y;
};

constexpr Vec2 operator-(const Vec2& v) noexcept {
    return { -v.x, - v.y };
}

constexpr Vec2 operator+(const Vec2& a, const Vec2& b) noexcept {
    return { a.x + b.x, a.y + b.y };
}

constexpr Vec2 operator-(const Vec2& a, const Vec2& b) noexcept {
    return { a.x - b.x, a.y - b.y };
}

constexpr Vec2 operator*(const Vec2& v, double d) {
    return { v.x * d, v.y * d };
}

constexpr Vec2 operator*(double d, const Vec2& v) {
    return v * d;
}

constexpr double dot(const Vec2& a, const Vec2& b) noexcept {
    return a.x * b.x + a.y * b.y;
}

constexpr Vec2 vmin(const Vec2& a, const Vec2& b) noexcept {
    return { std::min(a.x, b.x), std::min(a.y, b.y) };
}

constexpr Vec2 vmax(const Vec2& a, const Vec2& b) noexcept {
    return { std::max(a.x, b.x), std::max(a.y, b.y) };
}

constexpr Vec2 vzero_2d() noexcept {
    return { 0, 0 };
}