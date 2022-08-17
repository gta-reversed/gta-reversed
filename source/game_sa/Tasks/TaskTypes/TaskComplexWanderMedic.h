#pragma once

#include "TaskComplexWander.h"

class CTaskComplexWanderMedic : public CTaskComplexWander {
public:
    CTaskComplexWanderMedic(int32 MoveState, uint8 Dir, bool bWanderSensibly = true);

    eWanderType GetWanderType() override { return WANDER_TYPE_MEDIC; } // 0x658810
    CTask* Clone() override { return new CTaskComplexWanderMedic(m_nMoveState, m_nDir, m_bWanderSensibly); } // 0x6587A0
    void ScanForStuff(CPed* ped) override {} // 0x658820
};
