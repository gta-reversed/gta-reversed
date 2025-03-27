#include "StdInc.h"

#include "FxInfoEmRate.h"

// 0x4A6A00
FxInfoEmRate_c::FxInfoEmRate_c() : FxInfo_c() {
    m_nType = FX_INFO_EMRATE_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A4B40
void FxInfoEmRate_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4B60
void FxInfoEmRate_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[16];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);

    auto v10 = currentTime - totalTime;
    if (useConst) {
        emission.m_fCount = values[0] * totalTime;
    } else if (v10 >= 0.0f) {
        emission.m_fCount = m_InterpInfo.GetVal(0, currentTime, totalTime);
    } else {
        emission.m_fCount = m_InterpInfo.GetVal(0, len, -v10);
        emission.m_fCount = m_InterpInfo.GetVal(0, currentTime, v10 + totalTime) + emission.m_fCount;
    }
}
