#include <StdInc.h>

#include "AngledRect.hpp"


namespace notsa {
namespace shapes {
bool AngledRect::IsPointWithin(const CVector2D& pos) const {
    // Check if `pos` lies on the segment defined by `A` and `dir * mag`
    const auto PointLiesOnSegment = [
        &,
            posRelativeToA = pos - m_a
    ](const CVector2D& dir, float mag) {
        const auto dot = dir.Dot(posRelativeToA);
        return dot >= 0.f && dot <= mag;
    };

        return PointLiesOnSegment(m_dirBtoA, m_height) && PointLiesOnSegment(m_dirDtoA, m_width);
}

void AngledRect::DrawWireFrame(CRGBA color, float z, const CMatrix& transform) const {
    const auto colorARGB = color.ToInt();

    // Calculate corners
    CVector corners[4];
    rng::transform(GetCorners(), corners, [&](auto& c) {
        return transform.TransformPoint(CVector{ c.x, c.y, z });
    });

    // Draw lines going from one corner to another
    for (auto i = 0u; i < 4; i++) {
        CLines::RenderLineNoClipping(corners[i], corners[(i + 1) % 4], colorARGB, colorARGB);
    }

    // Draw diagonal line
    CLines::RenderLineNoClipping(corners[0], corners[2], colorARGB, colorARGB);
}

void AngledRect::HighlightWithMarkers(const CMatrix& transform) const {
    for (const auto& corner : GetCorners()) {
        auto corner3D = transform.TransformPoint(CVector{ corner, CWorld::FindGroundZForCoord(corner.x, corner.y) });
        C3dMarkers::PlaceMarkerSet(
            reinterpret_cast<uint32>(this) + rand(),
            MARKER3D_CYLINDER,
            corner3D,
            0.2f,
            255, 0, 255, 150,
            2048,
            0.2f,
            0
        );
    }
}
}; // namespace shapes
}; // namespace notsa
