#include "StdInc.h"

#include "FxInfoFriction.h"

// 0x4A5550
FxInfoFriction_c::FxInfoFriction_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_FRICTION_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A55C0
void FxInfoFriction_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A55E0
void FxInfoFriction_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Vel *= std::pow(values[0], totalTime * 50.0f);
}
