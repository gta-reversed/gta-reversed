#include "StdInc.h"

#include "FxInfoHeatHaze.h"

// 0x4A61A0
FxInfoHeatHaze_c::FxInfoHeatHaze_c() : FxInfo_c() {
    m_nType = FX_INFO_HEATHAZE_DATA;
    m_InterpInfo.Allocate(0);
}

// 0x4A61F0
void FxInfoHeatHaze_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A6210
void FxInfoHeatHaze_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<RenderInfo_t*>(info)->m_bHeatHaze = true;
}
