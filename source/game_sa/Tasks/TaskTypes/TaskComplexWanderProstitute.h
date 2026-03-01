#pragma once

#include "TaskComplexWanderStandard.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderProstitute : public CTaskComplexWanderStandard {
public:
    uint32 m_nStartTimeInMs;

public:
    CTaskComplexWanderProstitute(eMoveState MoveState, uint8 Dir, bool bWanderSensibly = true);

    eWanderType GetWanderType() override { return WANDER_TYPE_PROSTITUTE; } // 0x6726C0
    CTask* Clone() const override { return new CTaskComplexWanderProstitute(m_nMoveState, m_nDir, m_bWanderSensibly); } // 0x673C80
    CTask* CreateFirstSubTask(CPed* ped) override { return CTaskComplexWander::CreateFirstSubTask(ped); } // 0x674920
    void ScanForStuff(CPed* ped) override;
};
