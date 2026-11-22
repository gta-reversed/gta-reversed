#include "StdInc.h"

#include "FxInfoEmRotation.h"

// 0x4A51B0
FxInfoEmRotation_c::FxInfoEmRotation_c() : FxInfo_c() {
    m_nType = FX_INFO_EMROTATION_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A5220
void FxInfoEmRotation_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5240
void FxInfoEmRotation_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fRotationMinAngle = values[0];
    emission.m_fRotationMaxAngle = values[1];
}
