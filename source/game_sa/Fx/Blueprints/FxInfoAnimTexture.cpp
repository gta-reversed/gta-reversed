#include "StdInc.h"

#include "FxInfoAnimTexture.h"
#include "RenderInfo.h"

// 0x4A6550
FxInfoAnimTexture_c::FxInfoAnimTexture_c() : FxInfo_c() {
    m_nType = FX_INFO_ANIMTEX_DATA;
    m_InterpInfo.Allocate(1);
}

// 0x4A65C0
void FxInfoAnimTexture_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A65E0
void FxInfoAnimTexture_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[16];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.m_bHasAnimTextures = true;
    render.m_nCurrentTexId = static_cast<int8>(values[0]);
}
