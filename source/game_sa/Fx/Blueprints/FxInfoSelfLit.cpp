#include "StdInc.h"

#include "FxInfoSelfLit.h"
#include "RenderInfo.h"

// 0x4A6760
FxInfoSelfLit_c::FxInfoSelfLit_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_SELFLIT_DATA;
    m_InterpInfo.Allocate(0);
}

// 0x4A67B0
void FxInfoSelfLit_c::Load(FILESTREAM file, int32 version) {
    // NOP
}

// 0x4A67C0
void FxInfoSelfLit_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<RenderInfo_t*>(info)->m_bSelfLit = true;
}
