#pragma once

#include "EntitySeekPosCalculator.h"
#include "Vector2D.h"

class NOTSA_EXPORT_VTABLE CEntitySeekPosCalculatorRadiusAngleOffset : public CEntitySeekPosCalculator {
    float m_radius{};
    float m_angle{};
public:
    static void InjectHooks();

    CEntitySeekPosCalculatorRadiusAngleOffset() = default;
    CEntitySeekPosCalculatorRadiusAngleOffset(float radius, float angle);

    void ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) override;
};
VALIDATE_SIZE(CEntitySeekPosCalculatorRadiusAngleOffset, 0xC);
