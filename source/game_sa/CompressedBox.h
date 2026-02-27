#pragma once

#include "extensions/FixedVector.hpp"

class CompressedBox {
public:
    FixedVector<int16, 8.0f> m_vecMin, m_vecMax;

    void DrawWireFrame(CRGBA color, const CMatrix& transform) const;

    // NOTSA
    operator CBox() const { return CBox{ m_vecMin, m_vecMax }; }
};
VALIDATE_SIZE(CompressedBox, 0xC);
