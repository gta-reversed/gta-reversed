#include "StdInc.h"

#include "AngledBox.hpp"

namespace notsa {
namespace shapes {
AngledBox::AngledBox(
    AngledRect rect,
    float bottomZ,
    float ceilingZ
) :
    m_rect{ rect },
    m_bottomZ{ bottomZ },
    m_ceilingZ{ ceilingZ }
{
    assert(ceilingZ >= bottomZ);
}

AngledBox::AngledBox(
    CVector2D a,
    CVector2D dirAB, float lenAB,
    CVector2D dirAD, float lenAD,
    float bottomZ,
    float ceilingZ
) :
    AngledBox(
        AngledRect{ a, dirAB, lenAB, dirAD, lenAD },
        bottomZ,
        ceilingZ
    )
{
}

bool AngledBox::IsPointWithin(CVector pt, float tolerance) const {
    return pt.z >= m_bottomZ - tolerance
        && pt.z <= m_ceilingZ + tolerance
        && m_rect.IsPointWithin(pt, tolerance);
}

void AngledBox::DrawWireFrame(CRGBA color, float z, const CMatrix& transform) const {
    constexpr uint8 indices[][2]{
        // Bottom rect
        {0, 1},
        {1, 2},
        {2, 3},
        {3, 0},

        // Top rect
        {4, 5},
        {5, 6},
        {6, 7},
        {7, 4},

        // Diagonals bottom, top
        {0, 3},
        {4, 7},

        // Diagonals on the 4 sides
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7},
    };

    // Get the base rect's corners
    const auto rectCorners = m_rect.GetCorners();

    // All 8 corners (first the bottom 4, then top 4)
    CVector corners[8];
    for (auto i = 0; i < 2; i++) {
        const auto z = i == 0 ? m_bottomZ : m_ceilingZ;
        rng::transform(rectCorners, corners + i * 4, [&](auto& c) {
            return transform * CVector{ c.x, c.y, z };
        });
    }

    // Draw all lines
    const auto colorARGB = color.ToInt();
    for (auto [a, b] : indices) {
        CLines::RenderLineNoClipping(corners[a], corners[b], colorARGB, colorARGB);
    }
}

}; // namespace shapes
}; // namespace notsa
