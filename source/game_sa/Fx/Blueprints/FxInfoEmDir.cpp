#include "StdInc.h"

#include "FxInfoEmDir.h"

// 0x4A4DC0
FxInfoEmDir_c::FxInfoEmDir_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_EMDIR_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A4E30
void FxInfoEmDir_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A4E50
void FxInfoEmDir_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[3];
    m_InterpInfo.GetVal(values, currentTime);

    static_cast<EmissionInfo_t*>(info)->m_Direction = *(CVector*)&values[0];
}
