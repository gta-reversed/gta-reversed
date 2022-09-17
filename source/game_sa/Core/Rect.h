/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector2D.h"

/* CRect class describes a rectangle.

    A(left;top)_____________________→
    |                               |
    |                               |
    |                               |
    |_________________B(right;bottom)
    ↓

*/

class CRect {
public:
    float left      =  1000000.0F; // x1
    float top       = -1000000.0F; // y2
    float right     = -1000000.0F; // x2
    float bottom    =  1000000.0F; // y1

public:
    static void InjectHooks();

    CRect() = default; // 0x4041C0
    constexpr CRect(float fLeft, float fTop, float fRight, float fBottom) { // 0x4041C0
       left   = fLeft;
       bottom    = fTop;
       right  = fRight;
       top = fBottom;
       assert(!IsFlipped());
    }
    constexpr CRect(const CVector2D& top, const CVector2D& bottom) :
        CRect{top.x, top.y, bottom.x, bottom.y}
    {
    }

    [[nodiscard]] constexpr inline bool IsFlipped() const { // 0x404190
        return left > right || bottom > top;
    }

    void Restrict(const CRect& restriction);
    void Resize(float resizeX, float resizeY);
    [[nodiscard]] bool IsPointInside(const CVector2D& point) const;
    [[nodiscard]] bool IsPointInside(const CVector2D& point, float tolerance) const;
    void SetFromCenter(float x, float y, float size);
    void GetCenter(float* x, float* y) const;
    [[nodiscard]] inline CVector2D GetCenter() const { return { (right + left) * 0.5F, (bottom + top) * 0.5F }; }
    void StretchToPoint(float x, float y);

};

VALIDATE_SIZE(CRect, 0x10);
