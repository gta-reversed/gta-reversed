#include "StdInc.h"

#include "FxInfoJitter.h"

// 0x4A5B20
FxInfoJitter_c::FxInfoJitter_c() : FxInfo_c() {
    m_nType = FX_INFO_JITTER_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A5B90
void FxInfoJitter_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5BB0
void FxInfoJitter_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float value;
    m_InterpInfo.GetVal(&value, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= value * totalTime;

    auto& movement = *static_cast<MovementInfo_t*>(info);
    movement.m_Pos += in;
}
