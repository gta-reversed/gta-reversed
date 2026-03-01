#pragma once

#include "EntitySeekPosCalculator.h"

class NOTSA_EXPORT_VTABLE CEntitySeekPosCalculatorStandard : public CEntitySeekPosCalculator {
public:
    static void InjectHooks();

    void ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) override;
};
VALIDATE_SIZE(CEntitySeekPosCalculatorStandard, 0x4);
