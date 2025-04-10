/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexKillPedOnFoot : public CTaskComplex {
public:
    bool       m_bShotFiredByPlayerFlag : 1 = true;
    bool       m_bNewTarget : 1 = true;
    bool       m_bRoomToDragPedOutOfCar : 1 = true;
    bool       m_bTargetKilled : 1 = false;
    bool       m_bAimImmediate : 1 = false;
    bool       m_bWaitForPlayerToBeSafe : 1 = false;
    bool       m_bWaitingForPlayerToBeSafe : 1 = false;
   
    CPed*      m_target;
    int32      m_pedFlags;
    int32      m_actionDelay;
    int32      m_actionChance;
    uint8      m_nCompetence;
    uint32     m_startTime;
    int32      m_time;
    CTaskTimer m_timer;

public:
    static constexpr auto Type = TASK_COMPLEX_KILL_PED_ON_FOOT;

    CTaskComplexKillPedOnFoot(CPed* target, int32 time = -1, int32 pedFlags = 0, int32 delay = 0, int32 chance = 0, uint8 nCompetence = 1, /* notsa => */bool bWaitForPlayerToBeSafe = false, bool bWaitingForPlayerToBeSafe = false);

    ~CTaskComplexKillPedOnFoot() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskComplexKillPedOnFoot(m_target, m_time, m_pedFlags, m_actionDelay, m_actionChance, m_nCompetence); }
    bool   MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(int32 taskId, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexKillPedOnFoot* Constructor(CPed* target, int32 time, int32 pedFlags, int32 delay, int32 chance, int8 a7);
};

VALIDATE_SIZE(CTaskComplexKillPedOnFoot, 0x38);
