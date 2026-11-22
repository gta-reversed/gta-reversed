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
        m_b{ b },
        m_dirBtoA{ (b - a).Normalized(&m_height) },
        m_dirDtoA{ widthAndDir >= 0 ? m_dirBtoA.GetPerpLeft() : m_dirBtoA.GetPerpRight() },
        m_width{ std::abs(widthAndDir) }
    {
    }

    auto GetCornerA() const { return m_a; }
    auto GetCornerB() const { return m_b; }
    auto GetCornerC() const { return m_b + m_dirDtoA * m_width; }
    auto GetCornerD() const { return m_a + m_dirDtoA * m_width; }
    auto GetCorners() const { return std::to_array({ GetCornerA(), GetCornerB(), GetCornerC(), GetCornerD() }); }

    //! Check if a point is within this quad
    bool IsPointWithin(const CVector2D& pos) const;

    //! Draw wireframe of this quad (Must set-up render states beforehands!)
    void DrawWireFrame(CRGBA color, float z, const CMatrix& transform = CMatrix::Unity()) const;

    //! Highlight this rect with markers in each corner
    void HighlightWithMarkers(const CMatrix& transform = CMatrix::Unity()) const;
private:
    CVector2D m_a{}, m_b{};             //< Corner A and B 
    CVector2D m_dirBtoA{}, m_dirDtoA{}; //< Directions to/from corners `C -> A` is the same as `B -> D`
    float     m_width{}, m_height{};    //< Width and height (both always positive)
};
};
}; // namespace notsa
