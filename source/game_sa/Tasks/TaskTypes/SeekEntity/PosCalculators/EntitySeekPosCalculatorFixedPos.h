#pragma once

#include "EntitySeekPosCalculator.h"
#include "Vector.h"
#include "Base.h"

class NOTSA_EXPORT_VTABLE CEntitySeekPosCalculatorFixedPos : public CEntitySeekPosCalculator {
    CVector m_fixedPos{};
public:
    static void InjectHooks();

    CEntitySeekPosCalculatorFixedPos(const CVector& pos);

    void ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) override;
};
