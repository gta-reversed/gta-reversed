#pragma once

#include "FxInfo.h"
#include "FxInterpInfo255.h"

class NOTSA_EXPORT_VTABLE FxInfoHeatHaze_c : public FxInfo_c {
protected:
    FxInterpInfo255_c m_InterpInfo;

public:
    FxInfoHeatHaze_c();
    ~FxInfoHeatHaze_c() override = default;

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
};
