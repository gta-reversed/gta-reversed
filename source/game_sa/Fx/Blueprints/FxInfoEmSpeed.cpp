#include "StdInc.h"

#include "FxInfoEmSpeed.h"
#include "FxManager.h"

// 0x4A4D00
FxInfoEmSpeed_c::FxInfoEmSpeed_c() : FxInfo_c(), m_InterpInfo() {
    m_nType = FX_INFO_EMSPEED_DATA;
    m_InterpInfo.Allocate(2);
}

// 0x4A4D70
void FxInfoEmSpeed_c::Load(FILESTREAM file, int32 version) {
    // nick7:
    // well, this is interesting: this is not a pointer, so there's 2 ways:
    //  1) WIN32 version of SA calls __vfptr->Load
    //  2) Android version of SA calls FxInterpInfo32_c::Load
    // anyway, this is NOT a bug here, but this is not good
    m_InterpInfo.Load(file);
}

// 0x4A4D90
void FxInfoEmSpeed_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[2];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fSpeed     = values[0];
    emission.m_fSpeedBias = values[1];
}
