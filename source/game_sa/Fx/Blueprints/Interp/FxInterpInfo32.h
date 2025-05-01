#pragma once

#include "FxInterpInfo.h"

class NOTSA_EXPORT_VTABLE FxInterpInfo32_c : public FxInterpInfo_c {
protected:
    int16** m_Keys;

public:
    FxInterpInfo32_c();
    ~FxInterpInfo32_c() = default; // 0x4A89B0

    void Load(FILESTREAM file) override;

    void Allocate(int32 count);
    void GetVal(float* outValues, float delta);
};
