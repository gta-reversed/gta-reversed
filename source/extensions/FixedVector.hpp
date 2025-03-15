#pragma once
#include "FixedFloat.hpp"
#include "Vector.h"

template<typename T, float CompressValue>
struct FixedVector {
    constexpr FixedVector() = default;
    constexpr FixedVector(CVector v3d) : x(v3d.x), y(v3d.y), z(v3d.z) {}
    //constexpr FixedVector(float X, float Y, float Z) : x(X), y(Y), z(Z) {} // doesn't work because of `SunFlareDef` shit
    constexpr FixedVector(T X, T Y, T Z) : x(X), y(Y), z(Z) {}

    constexpr operator CVector() const { return CVector{ x, y, z }; }

public: // Public, because in some cases it might be useful to only access 1
    FixedFloat<T, CompressValue> x{}, y{}, z{};
};

template<typename T, float CompressValue>
struct FixedVector2D {
    constexpr FixedVector2D() = default;
    constexpr FixedVector2D(float X, float Y) : x(X), y(Y) {}
    constexpr FixedVector2D(CVector2D v2d) : FixedVector2D{v2d.x, v2d.y} {}
    constexpr FixedVector2D(T X, T Y) : FixedVector2D{ x, y }  {}

    constexpr operator CVector2D() const { return { x, y }; }

public: // Public, because in some cases it might be useful to only access 1
    FixedFloat<T, CompressValue> x{}, y{};
};
