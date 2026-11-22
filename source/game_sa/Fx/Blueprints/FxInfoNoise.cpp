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

    const auto speed = movement.m_Vel.Magnitude();
    movement.m_Vel += CVector{ (float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f }.Normalized() * value * totalTime;
    if (const auto mag = movement.m_Vel.Magnitude(); mag > 0.0f) {
        movement.m_Vel /= mag;
    }
    movement.m_Vel *= speed;
}
