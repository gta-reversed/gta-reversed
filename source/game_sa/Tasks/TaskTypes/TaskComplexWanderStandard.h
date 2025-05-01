#pragma once

#include "TaskComplexWander.h"
#include "TaskTimer.h"
#include "eMoveState.h"

class NOTSA_EXPORT_VTABLE CTaskComplexWanderStandard : public CTaskComplexWander {
public:
    CTaskTimer m_TaskTimer;
    uint32     m_nMinNextScanTime;

public:
    CTaskComplexWanderStandard(eMoveState MoveState, uint8 Dir, bool bWanderSensibly = true);

    CTask* Clone() const override { return new CTaskComplexWanderStandard(m_nMoveState, m_nDir); } // 0x48E530
    eWanderType GetWanderType() override { return WANDER_TYPE_STANDARD; } // 0x48E5D0
    void ScanForStuff(CPed* ped) override;

    bool WillChat(const CPed& first, const CPed& second);
    void SetNextMinScanTime(CPed* ped);
    bool LookForSexyCars(CPed* ped);
    bool LookForChatPartners(CPed* ped);
    bool LookForGangMembers(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexWanderStandard, 0x38);
