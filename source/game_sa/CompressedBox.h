#pragma once

#include "Box.h"
#include "CompressedVector.h"

class CompressedBox {
public:
    CompressedLargeVector m_vecMin, m_vecMax;

    void DrawWireFrame(CRGBA color, const CMatrix& transform) const;

    // NOTSA
    operator CBox() const { return CBox{ m_vecMin, m_vecMax }; }
};
VALIDATE_SIZE(CompressedBox, 0xC);
