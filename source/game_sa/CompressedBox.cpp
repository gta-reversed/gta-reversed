#include "StdInc.h"
#include "CompressedBox.h"
#include "Lines.h"

void CompressedBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const
{
    CBox(*this).DrawWireFrame(color, transform);
}
CompressedBox::operator CBox() const {
    return CBox{UncompressLargeVector(m_vecMin), UncompressLargeVector(m_vecMax)};
}
