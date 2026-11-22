#include "StdInc.h"
#include "ColTriangle.h"
#include "Lines.h"

// NOTSA
void CColTriangle::DrawWireFrame(CRGBA color, const CompressedVector* vertices, const CMatrix& transform) const {
    const CVector a = transform.TransformPoint(vertices[vA]);
    const CVector b = transform.TransformPoint(vertices[vB]);
    const CVector c = transform.TransformPoint(vertices[vC]);

    const auto colorARGB = color.ToIntARGB();
    CLines::RenderLineNoClipping(a, b, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(a, c, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(b, c, colorARGB, colorARGB);
}

auto CColTriangle::GetPlane(const CompressedVector* vertices) const -> CColTrianglePlane {
    return { *this, vertices };
}

auto CColTriangle::GetBoundingRect(const CVector& a, const CVector& b, const CVector& c) -> CRect {
    const auto [left, right] = std::minmax({ a.x, b.x, c.x });
    const auto [bottom, top] = std::minmax({ a.y, b.y, c.y });
    return CRect{ left, bottom, right, top };
}

auto CColTriangle::GetPoly(const CompressedVector* verts) const -> CStoredCollPoly {
    return CStoredCollPoly{
        .verts    = { verts[vA], verts[vB], verts[vC] },
        .valid    = true,
        .ligthing = m_nLight,
    };
}
