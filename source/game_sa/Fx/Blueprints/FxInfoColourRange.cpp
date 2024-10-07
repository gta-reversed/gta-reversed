#include "StdInc.h"

#include "FxInfoColourRange.h"

// 0x4A6630
FxInfoColourRange_c::FxInfoColourRange_c() : FxInfo_c() {
    m_nType = FX_INFO_COLOURRANGE_DATA;
    m_InterpInfo.Allocate(7);
}

// 0x4A66A0
void FxInfoColourRange_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A66C0
void FxInfoColourRange_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[7];
    m_InterpInfo.GetVal(values, mult);

    auto& render = *static_cast<RenderInfo_t*>(info);
    render.SetColor(values[0], values[2], values[4], values[6]);
    render.SetRangeColor(values[1], values[3], values[5]);
    render.m_nColorType = ERenderColorType::RANGE;
}
