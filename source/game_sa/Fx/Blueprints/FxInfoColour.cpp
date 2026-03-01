#include "StdInc.h"

#include "FxInfoColour.h"

// 0x4A5EA0
FxInfoColour_c::FxInfoColour_c() : FxInfo_c() {
    m_nType = FX_INFO_COLOUR_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A5F10
void FxInfoColour_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

void FxInfoColour_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    static_cast<RenderInfo_t*>(info)->SetColor(values[0], values[1], values[2], values[3]);
}
