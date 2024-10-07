#pragma once

#include "TaskComplexWander.h"
#include "eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderMedic : public CTaskComplexWander {
public:
    CTaskComplexWanderMedic(eMoveState MoveState, uint8 Dir, bool bWanderSensibly = true);

    eWanderType GetWanderType() override { return WANDER_TYPE_MEDIC; } // 0x658810
    CTask* Clone() const override { return new CTaskComplexWanderMedic(m_nMoveState, m_nDir, m_bWanderSensibly); } // 0x6587A0
    void ScanForStuff(CPed* ped) override {} // 0x658820
};
