#include "StdInc.h"

#include "Lines.h"

void CBox::InjectHooks()
{
    RH_ScopedClass(CBox);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EDE0);
    RH_ScopedInstall(Recalc, 0x40EE20);
}

void CBox::Set(const CVector& vecMin, const CVector& vecMax)
{
    m_vecMin = vecMin;
    m_vecMax = vecMax;
}

void CBox::Recalc()
{
    if (m_vecMin.x > m_vecMax.x)
        std::swap(m_vecMin.x, m_vecMax.x);

    if (m_vecMin.y > m_vecMax.y)
        std::swap(m_vecMin.y, m_vecMax.y);

    if (m_vecMin.z > m_vecMax.z)
        std::swap(m_vecMin.z, m_vecMax.z);
}

// NOTSA - TODO(OPT): Refactor code (meaningful names, etc) and possibly use std::optional for `CMatrix` (In cases where a unity matrix would be used otherwise)
void CBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    auto workVec = m_vecMin;
    CVector v1 = transform.TransformPoint(workVec);

    workVec.z = m_vecMax.z;
    CVector v2 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    CVector v3 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.y = m_vecMax.y;
    CVector v4 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.y = m_vecMax.y;
    workVec.z = m_vecMax.z;
    CVector v5 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    workVec.z = m_vecMax.z;
    CVector v6 = transform.TransformPoint(workVec);

    workVec = m_vecMin;
    workVec.x = m_vecMax.x;
    workVec.y = m_vecMax.y;
    CVector v7 = transform.TransformPoint(workVec);

    workVec = m_vecMax;
    CVector v8 = transform.TransformPoint(workVec);

    const auto colorRGBA = color.ToInt();
    CLines::RenderLineNoClipping(v1, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v1, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v1, v4, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v5, v4, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v2, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v6, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v8, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v3, colorRGBA, colorRGBA);
    CLines::RenderLineNoClipping(v7, v4, colorRGBA, colorRGBA);
}

bool CBox::IsPointInside(const CVector& point) const {
    return point.x >= m_vecMin.x && point.x <= m_vecMax.x
        && point.y >= m_vecMin.y && point.y <= m_vecMax.y
        && point.z >= m_vecMin.z && point.z <= m_vecMax.z;
}

void CBox::StretchToPoint(const CVector& pt) {
    for (size_t i = 0; i < 3; i++) {
        m_vecMin[i] = std::min(m_vecMin[i], pt[i]);
        m_vecMax[i] = std::max(m_vecMax[i], pt[i]);
    }
}
