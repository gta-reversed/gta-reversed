#pragma once

#include "FxInterpInfo.h"

class NOTSA_EXPORT_VTABLE FxInterpInfoU255_c : public FxInterpInfo_c {
protected:
    uint16** m_Keys;

public:
    FxInterpInfoU255_c();
    ~FxInterpInfoU255_c() = default; // 0x4A87F0

    void Load(FILESTREAM file) override;

    void Allocate(int32 count);
    void GetVal(float* outValues, float delta);
};
