#include "StdInc.h"

#include "FxInfoTrail.h"

// 0x4A6220
FxInfoTrail_c::FxInfoTrail_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_TRAIL_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A6290
void FxInfoTrail_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A62B0
void FxInfoTrail_c::GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / length;
    }

    float values[2];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_fTrailTime = values[0];
    render.m_nTrailScreenMode = 1;
    if (values[1] > 0.1f) {
        render.m_nTrailScreenMode = 2;
    }
}
