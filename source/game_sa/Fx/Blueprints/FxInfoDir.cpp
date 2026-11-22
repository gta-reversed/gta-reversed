#include "StdInc.h"

#include "FxInfoDir.h"

// 0x4A6440
FxInfoDir_c::FxInfoDir_c() : FxInfo_c() {
    m_nType = FX_INFO_DIR_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A64B0
void FxInfoDir_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A64D0
void FxInfoDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[3];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_bHasDir = true;
    render.m_Direction = *(CVector*)&values[0];
    if (render.m_Direction.Magnitude() < 0.001f) {
        render.m_bUseVel = true;
    }
}
