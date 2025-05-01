#pragma once

#include "FxInterpInfo.h"

class NOTSA_EXPORT_VTABLE FxInterpInfo255_c : public FxInterpInfo_c {
protected:
    int16** m_Keys;

public:
    FxInterpInfo255_c();
    ~FxInterpInfo255_c() = default; // 0x4A8B70

    void Load(FILESTREAM file) override;

    void Allocate(int32 count);
    void GetVal(float* outValues, float delta);
};
