#pragma once

#include "FxInterpInfo.h"

class NOTSA_EXPORT_VTABLE FxInterpInfoFloat_c : public FxInterpInfo_c {
protected:
    float** m_Keys;

public:
    FxInterpInfoFloat_c();
    ~FxInterpInfoFloat_c() = default; // 0x4A8460

    void Load(FILESTREAM file) override;

    void Allocate(int32 count);
    float GetVal(int32 attrib, float time, float deltaTime);
    void GetVal(float* outValues, float delta);
};
