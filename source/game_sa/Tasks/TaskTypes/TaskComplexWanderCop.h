#pragma once

#include "TaskComplexWander.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskTimer.h"
#include "eMoveState.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexWanderCop : public CTaskComplexWander {
public:
    CTaskComplexGoToPointAndStandStill* m_pGoToPointAndStandStillTask;
    CTaskTimer                          m_nScanForStuffTimer;
    uint32                              m_nTimePassedSinceLastLookedForCriminals;
    uint32                              m_nTimePassedSinceLastLookedForCarAlarmsAndStolenCopCars;
    CPed*                               m_pLastCriminalPedLookedFor;
    CTaskTimer                          m_nSubTaskCreatedTimer;

public:
    CTaskComplexWanderCop(eMoveState moveState, uint8 dir);
    ~CTaskComplexWanderCop() override;

    eWanderType GetWanderType() override { return WANDER_TYPE_COP; } // 0x460D50

// 0x0
CTask* Clone() const override { return new CTaskComplexWanderCop(m_nMoveState, m_nDir); }

 // 0x460CE0
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    void ScanForStuff(CPed* ped) override;

    void LookForCarAlarms(CPed* ped);
    void LookForStolenCopCars(CPed* ped);
    void LookForCriminals(CPed* ped);
    bool ShouldPursuePlayer(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexWanderCop* Constructor(eMoveState moveState, uint8 dir);

};

VALIDATE_SIZE(CTaskComplexWanderCop, 0x50);
