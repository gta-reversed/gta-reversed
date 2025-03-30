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
    // Init in flipped state
    float left      =  1000000.0F; // x1
    float top       = -1000000.0F; // y1
    float right     = -1000000.0F; // x2
    float bottom    =  1000000.0F; // y2

public:
    static void InjectHooks();

    CRect() = default; // 0x4041C0 - TODO: Fix retarded argument order to be: left, top, right, bottom

    constexpr CRect(float left, float bottom, float right, float top) : // minX, minY, maxX, maxY
        left{ left },
        bottom{ bottom },
        right{ right },
        top{ top }
    {
        assert(!IsFlipped());
    }

    constexpr CRect(const CVector2D& bottomLeft, const CVector2D& topRight) :
        CRect{bottomLeft.x, bottomLeft.y, topRight.x, topRight.y}
    {
    }

    /// A rect that can fit a circle of `radius` inside with `pos` being the center
    constexpr CRect(const CVector2D& pos, float radius) :
        CRect{ pos.x - radius, pos.y - radius, pos.x + radius, pos.y + radius }
    {
    }

    [[nodiscard]] constexpr inline bool IsFlipped() const { // 0x404190
        return left > right || bottom > top;
    }

    void Restrict(const CRect& restriction);
    void Resize(float resizeX, float resizeY);
    [[nodiscard]] bool IsPointInside(CVector2D const& point) const;
    [[nodiscard]] bool IsPointInside(CVector2D const& point, float tolerance) const;
    [[nodiscard]] bool IsRectInside(const CRect& rect) const; // NOTSA (or maybe it is?)
    void SetFromCenter(float x, float y, float size);
    void GetCenter(float* x, float* y) const;
    [[nodiscard]] inline CVector2D GetCenter() const { return { (right + left) * 0.5F, (bottom + top) * 0.5F }; }
    void StretchToPoint(float x, float y);

    CVector2D GetTopLeft() const { return { left, top }; }
    CVector2D GetBottomRight() const { return { right, bottom }; }

    /*!
    * @addr notsa
    * @brief Constrain a point into the rectangle.
    *
    * @param pt The point to constrain
    *
    * @return Whenever the point was constrained
    */
    bool DoConstrainPoint(CVector2D& pt) const;

    /*!
    * @addr notsa
    * @brief Get corners of this rect (Order: top left, top right, bottom right, bottom left)
    * 
    * @param z The Z position to be used for all corners
    */
    auto GetCorners3D(float z) const -> std::array<CVector, 4> {
        return {
            CVector{ left,  top, z},
            CVector{ right, top, z},
            CVector{ right, bottom, z},
            CVector{ left,  bottom, z}
        };
    }
    
    /*!
    * @notsa
    * @brief Check if this rect collides with another
    */
    bool OverlapsWith(const CRect& o) const;

    /*!
    * @notsa
    * @brief Check if this rectangle is inside another one
    */
    bool Contains(const CRect& o) const;

    bool operator==(const CRect&) const = default;
    bool operator!=(const CRect&) const = default;
};

VALIDATE_SIZE(CRect, 0x10);
