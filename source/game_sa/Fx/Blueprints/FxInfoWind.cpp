#include "StdInc.h"

#include "FxInfoWind.h"
#include "MovementInfo.h"

// 0x4A5A10
FxInfoWind_c::FxInfoWind_c() : FxInfo_c() {
    m_nType = FX_INFO_WIND_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A5A80
void FxInfoWind_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5AA0
void FxInfoWind_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / length;
    }

    float value;
    m_InterpInfo.GetVal(&value, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    auto wind = value * *g_fxMan.m_pfWindSpeed * totalTime;
    movement.m_Vel += wind * *g_fxMan.m_pWindDir;
}
