#include "StdInc.h"

#include "Sphere.h"
#include "Lines.h"

// 0x40FCF0
void CSphere::Set(float radius, const CVector& center) {
    m_fRadius = radius;
    m_vecCenter = center;
}

// NOTSA
bool CSphere::IsPointWithin(const CVector& p) const {
    return (p - m_vecCenter).SquaredMagnitude() <= sq(m_fRadius);
}

// NOTSA
void CSphere::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    const CVector center = transform.TransformPoint(m_vecCenter);
 
    CVector v13 = center;
    v13.z += m_fRadius;

    CVector v21 = center;
    v21.z -= m_fRadius;

    CVector v32 = center;
    v32.x += m_fRadius;

    CVector v41 = center;
    v41.x -= m_fRadius;

    CVector v52 = center;
    v52.y += m_fRadius;

    CVector v61 = center;
    v61.y -= m_fRadius;

    const auto colorARGB = color.ToInt();
    CLines::RenderLineNoClipping(v13, v32, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v41, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v32, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v41, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v52, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v13, v61, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v52, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(v21, v61, colorARGB, colorARGB);
}

auto CSphere::GetTransformed(const CMatrix& transform) const -> CSphere {
    return { transform.TransformPoint(m_vecCenter), m_fRadius };
}

auto TransformObject(const CSphere& sp, const CMatrix& transform) -> CSphere {
    return { transform.TransformPoint(sp.m_vecCenter), sp.m_fRadius };
}
