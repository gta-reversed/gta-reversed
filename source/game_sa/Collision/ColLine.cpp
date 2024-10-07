#include "StdInc.h"

#include "ColLine.h"
#include "Lines.h"

void CColLine::InjectHooks() {
    RH_ScopedClass(CColLine);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(Set, 0x40EF10);
}

// 0x40EF50
CColLine::CColLine(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// 0x40EF10
void CColLine::Set(const CVector& start, const CVector& end) {
    m_vecStart = start;
    m_vecEnd = end;
}

// NOTSA
float CColLine::DistTo(CVector pt) const {
    return CCollision::DistToLine(m_vecStart, m_vecEnd, pt);
}

// NOTSA
void CColLine::DrawWireFrame(CRGBA color, const CMatrix& transform) const {
    const auto colorARGB = color.ToInt();
    CLines::RenderLineNoClipping(transform.TransformPoint(m_vecStart), transform.TransformPoint(m_vecEnd), colorARGB, colorARGB);
}
