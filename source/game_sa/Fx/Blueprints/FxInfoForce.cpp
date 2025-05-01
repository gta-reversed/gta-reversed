#include "StdInc.h"

#include "FxInfoForce.h"

// 0x4A5450
FxInfoForce_c::FxInfoForce_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_FORCE_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A54C0
void FxInfoForce_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A54E0
void FxInfoForce_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Vel += CVector(values[0], values[1], values[2]) * totalTime;
}
