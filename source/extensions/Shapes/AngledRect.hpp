#pragma once

#include <array>

#include <Lines.h>
#include <Vector2D.h>
#include <Matrix.h>
#include <RGBA.h>


namespace notsa {
namespace shapes {
/*!
* Represents an angled rectangle (basically a quad with all right angles):
* /[B]-----------[C]\
* |                 |
* |                 |
* |                 |
* |                 |
* \[A]--<width>--[D]/ 
* `Width` may be negative (in which case `C` on the __above__ rectangle would be on the left instead of the right)
* just a note: `A` might be on the bottom too (so `B` may be in the top).
* But `C` is always calculated based on `A`'s position, and it's always either on the left or right of it.
*/
class AngledRect {
public:
    /*!
    * @param a           The A corner
    * @param b           The B corner
    * @param widthAndDir Width and direction of the C point relative to A.
    *                    If positive the C point will be to the left of the AB segment, otherwise to the right.
    */
    AngledRect(CVector2D a, CVector2D b, float widthAndDir) :
        m_a{ a },
        m_dirAB{ (b - a).Normalized(&m_height) },
        m_dirAD{ widthAndDir >= 0 ? m_dirAB.GetPerpLeft() : m_dirAB.GetPerpRight() },
        m_width{ std::abs(widthAndDir) }
    {
        DoDebugChecks();
    }

    /*!
    * @param a      The A corner
    * @param dirAB  Unit vector a -> b
    * @param lenAB  Distance beetween corners a <-> b
    * @param dirAD  -||-
    * @param lenAD  -||-
    */
    AngledRect(CVector2D a, CVector2D dirAB, float lenAB, CVector2D dirAD, float lenAD) :
        m_a{a},
        m_dirAB{dirAB},
        m_dirAD{dirAD},
        m_width{lenAD},
        m_height{lenAB}
    {
        DoDebugChecks();
    }

    auto GetCornerA() const { return m_a; }
    auto GetCornerB() const { return GetCornerA() + m_dirAB * m_height; }
    auto GetCornerC() const { return GetCornerB() + m_dirAD * m_width; }
    auto GetCornerD() const { return GetCornerA() + m_dirAD * m_width; }
    auto GetCorners() const { return std::to_array({ GetCornerA(), GetCornerB(), GetCornerC(), GetCornerD() }); }

    //! Check if a point is within this quad
    //! @param pos       The point
    //! @param tolerance Makes the rect larger/smaller in each direction [This way the point might be considered to be within even if it wouldn't be otherwise]
    bool IsPointWithin(const CVector2D& pt, float tolerance = 0.f) const;

    //! Draw wireframe of this quad (Must set-up render states beforehands!)
    void DrawWireFrame(CRGBA color, float z, const CMatrix& transform = CMatrix::Unity()) const;

    //! Highlight this rect with markers in each corner
    void HighlightWithMarkers(const CMatrix& transform = CMatrix::Unity()) const;

private:
    void DoDebugChecks() {
        assert(m_width > 0.f);
        assert(m_height > 0.f);
        assert(m_dirAB.IsUnitVector());
        assert(m_dirAD.IsUnitVector());
    }
private:
    CVector2D m_a;               //< Corner A and B 
    CVector2D m_dirAB, m_dirAD;  //< Unit vectors - directions to/from corners `C -> A` is the same as `B -> D`
    float     m_width, m_height; //< Width and height (both always positive)
};
};
}; // namespace notsa
