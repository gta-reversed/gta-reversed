#include "StdInc.h"

bool CPointList::Empty() const noexcept {
    return m_Count == 0;
}

void CPointList::AddPoint(CVector pt) noexcept {
    if (m_Count >= m_Points.size()) {
        return;
    }
    m_Points[m_Count++] = pt;
}
