#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoSize_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    FxInfoSize_c();
    ~FxInfoSize_c() override = default; // 0x4A7650

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float delta, float totalTime, float length, bool useConst, void* info) override;
};
