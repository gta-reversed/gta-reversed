#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoAttractLine_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    FxInfoAttractLine_c();
    ~FxInfoAttractLine_c() override = default; // 0x4A7040

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
};
