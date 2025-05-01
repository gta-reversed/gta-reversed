#pragma once

#include "FixedFloat.hpp"
#include "Rect.h"

// MinMaxOrientation: When it's true, elements are oriented as MinX, MinY, MaxX, MaxY. (CRect ctor style) 
// instead of top-left, bottom-right (CRect structure style)...
// Retarded I know.
template<typename T, float CompressValue, bool MinMaxOrientation = false>
struct FixedRect {
    constexpr FixedRect() = default;

    //v~~~ Use this after fixing the retarded CRect ctor args
    //constexpr FixedRect(T left, T top, T right, T bottom) :
    constexpr FixedRect(T left, T bottom, T right, T top) :
        x1(left),
        x2(right),
        y1(top),
        y2(bottom)
    {
        if constexpr (MinMaxOrientation) {
            std::swap(y1, y2);
        }
        assert(left <= right && bottom <= top); // !CRect::IsFlipped
    }

    constexpr FixedRect(CRect rt) :
        x1(rt.left),
        x2(rt.right),
        y1(rt.top),
        y2(rt.bottom)
    {
        if constexpr (MinMaxOrientation) {
            std::swap(y1, y2);
        }
    }

    constexpr operator CRect() const { return CRect{ x1, MinMaxOrientation ? y1 : y2, x2, MinMaxOrientation ? y2 : y1 }; }

    // Top-left (or bottom-left if MinMaxOrientation is true)
    constexpr CVector2D GetX1Y1() const { return { x1, y1 }; }

    // Bottom-right (or top-right if MinMaxOrientation is true)
    constexpr CVector2D GetY1Y2() const { return { x2, y2 }; }

public: // Public, because in some cases it might be useful to only access 1
    // MinMaxOrientation == false: left, top, right, bottom
    // == true: left, bottom, right, top
    FixedFloat<T, CompressValue> x1{}, y1{}, x2{}, y2{};
};
