#include "StdInc.h"

#include "FxInfoNoise.h"

FxInfoNoise_c::FxInfoNoise_c() : FxInfo_c() {
    m_nType = FX_INFO_NOISE_DATA;
    m_InterpInfo.Allocate(1);
}

void FxInfoNoise_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5300
void FxInfoNoise_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    auto& movement = *static_cast<MovementInfo_t*>(info);
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float value;
    m_InterpInfo.GetVal(&value, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= value * totalTime;

    auto length = movement.m_Vel.Magnitude();
    movement.m_Vel += in;

    if (movement.m_Vel.Magnitude() > 0.0f) {
        movement.m_Vel.Normalise();
    }

    movement.m_Vel *= length;
}
