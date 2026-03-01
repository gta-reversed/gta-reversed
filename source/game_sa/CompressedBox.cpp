#include "StdInc.h"
#include "CompressedBox.h"
#include "Lines.h"

void CompressedBox::DrawWireFrame(CRGBA color, const CMatrix& transform) const
{
    CBox(*this).DrawWireFrame(color, transform);
}
