#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoColourBright_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    FxInfoColourBright_c();
    ~FxInfoColourBright_c() override = default; // 0x4A7A40

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
};
