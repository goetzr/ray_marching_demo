#pragma once

inline double deg2rad(double d) {
    return d * std::numbers::pi / 180;
}

inline double rad2deg(double r) {
    return r * 180 / std::numbers::pi;
}