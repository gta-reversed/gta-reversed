#include "StdInc.h"

#include "FxInfoAttractPt.h"

// 0x4A5650
FxInfoAttractPt_c::FxInfoAttractPt_c() : FxInfo_c() {
    m_nType = FX_INFO_ATTRACTPT_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A56C0
void FxInfoAttractPt_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A56E0
void FxInfoAttractPt_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float values[4];
    m_InterpInfo.GetVal(values, mult);

    auto& movement = *static_cast<MovementInfo_t*>(info);
    CVector in = *(CVector*)&values[0] - movement.m_Pos;

    auto length = in.Magnitude();
    if (length > 0.0f)
        in.Normalise();

    if (length > 0.1f) {
        movement.m_Vel += in * values[0] * totalTime;
    }
}
