#include "StdInc.h"

#include "FxInfoFloat.h"
#include "FxManager.h"

// 0x4A5DA0
FxInfoFloat_c::FxInfoFloat_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_FLOAT_DATA;
    m_InterpInfo.Allocate(0);
}

// 0x4A5DF0
void FxInfoFloat_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5E10
void FxInfoFloat_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    static_cast<MovementInfo_t*>(info)->m_bHasFloatInfo = true;
}
