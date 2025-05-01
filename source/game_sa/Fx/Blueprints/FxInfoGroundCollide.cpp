#include "StdInc.h"

#include "FxInfoGroundCollide.h"

// 0x4A5980
FxInfoGroundCollide_c::FxInfoGroundCollide_c() : FxInfo_c() {
    m_nType = FX_INFO_GROUNDCOLLIDE_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A59F0
void FxInfoGroundCollide_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A7100
void FxInfoGroundCollide_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    auto& movement = *static_cast<MovementInfo_t*>(info);

    CVector origin = movement.m_Pos;
    auto diff = movement.m_Pos.z - movement.m_Vel.z;
    CColPoint colPoint;
    CEntity* colEntity;
    if (!CWorld::ProcessVerticalLine(origin, diff, colPoint, colEntity, true))
        return;

    if (colPoint.m_vecPoint.z > origin.z || movement.m_Pos.z >= colPoint.m_vecPoint.z)
        return;

    float values[16];
    m_InterpInfo.GetVal(values, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= values[2] * totalTime * 5.0f;

    auto v13 = colPoint.m_vecNormal.SquaredMagnitude() * values[0];
    CVector out;
    out = movement.m_Vel - colPoint.m_vecNormal * (v13 + v13);
    out += in;
    out.Normalise();

    movement.m_Pos.z = colPoint.m_vecPoint.z;
    movement.m_Vel = values[1] * out.Magnitude() * out;
}
