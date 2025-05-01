#pragma once

#include "TaskComplexWander.h"
#include "eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderCriminal : public CTaskComplexWander {
public:
    CTaskTimer m_TaskTimer;
    uint32     m_nMinNextScanTime;

public:
    CTaskComplexWanderCriminal(eMoveState MoveState, uint8 Dir, bool bWanderSensibly = true);
    ~CTaskComplexWanderCriminal() override = default; // 0x48E720

    CTask* Clone() const override { return new CTaskComplexWanderCriminal(m_nMoveState, m_nDir); }// 0x48E650
    eWanderType GetWanderType() override { return WANDER_TYPE_CRIMINAL; } // 0x48E6F0
    void LookForCarsToSteal(CPed* ped);     // 0x66B4F0
    void ScanForStuff(CPed* ped) override;  // 0x670350
};
