#include "StdInc.h"

#include "FxInfoColourBright.h"

// 0x4A67D0
FxInfoColourBright_c::FxInfoColourBright_c() : FxInfo_c() {
    m_nType = FX_INFO_COLOURBRIGHT_DATA;
    m_InterpInfo.Allocate(5);
}

// 0x4A6840
void FxInfoColourBright_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6860
void FxInfoColourBright_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[5];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.SetColor(values[0], values[1], values[2], values[3]);
    render.m_Color2.alpha = (uint8)values[4];
    render.m_nColorType = ERenderColorType::BRIGHT;
}
