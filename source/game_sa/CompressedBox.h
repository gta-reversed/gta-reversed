#pragma once

#include "CompressedVector.h"

class CompressedBox {
public:
    CompressedVector m_vecMin;
    CompressedVector m_vecMax;

    void DrawWireFrame(CRGBA color, const CMatrix& transform) const;

    // NOTSA
    operator CBox() const;
};
VALIDATE_SIZE(CompressedBox, 0xC);
