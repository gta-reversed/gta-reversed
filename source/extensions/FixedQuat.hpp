#pragma once

#include "FixedFloat.hpp"
#include "Quaternion.h"

template<typename T, float CompressValue>
struct FixedQuat {
    constexpr FixedQuat() = default;
    constexpr FixedQuat(CQuaternion q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
    constexpr FixedQuat(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}

    constexpr operator CQuaternion() const { return CQuaternion{ x, y, z, w }; }

public: // Public, because in some cases it might be useful to only access 1
    FixedFloat<T, CompressValue> x{}, y{}, z{}, w{};
};
