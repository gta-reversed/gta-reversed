#include "StdInc.h"

#include "FxInfoEmAngle.h"

// 0x4A4E90
FxInfoEmAngle_c::FxInfoEmAngle_c() : FxInfo_c() {
    m_nType = FX_INFO_EMANGLE_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A4F00
void FxInfoEmAngle_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4F20
void FxInfoEmAngle_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fAngleMin = values[0];
    emission.m_fAngleMax = values[1];
}
