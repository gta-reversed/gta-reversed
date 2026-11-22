#include "StdInc.h"

#include "TaskComplexKillPedOnFoot.h"

void CTaskComplexKillPedOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFoot, 0x86d894, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    //RH_ScopedConstructorInstall(0x620E30, "1", {}, CPed*, int32, int32, int32, int32, uint8); // even with a wrapper for this original ctor it's ambigous so let's not hook it
    RH_ScopedDestructorInstall(0x620EF0);

    RH_ScopedInstall(CreateSubTask, 0x625E70, { .Reversed = false });

    RH_ScopedVMTInstall(Clone, 0x623370);
    RH_ScopedVMTInstall(GetTaskType, 0x620EE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x625E40, { .Reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62B150, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62B490, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x626260, { .Reversed = false });
}

CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(
    CPed* target,
    int32 time,
    int32 pedFlags,
    int32 delay,
    int32 chance,
    uint8 nCompetence,
    bool  bWaitForPlayerToBeSafe,
    bool  bWaitingForPlayerToBeSafe
) :
    m_bWaitForPlayerToBeSafe{ bWaitForPlayerToBeSafe },
    m_bWaitingForPlayerToBeSafe{ bWaitingForPlayerToBeSafe },
    m_PedToKill{ target },
    m_pedFlags{ pedFlags },
    m_actionDelay{ delay },
    m_actionChance{ chance },
    m_nCompetence{ nCompetence },
    m_time{ time },
    m_startTime{ CTimer::GetTimeInMS() } {
}

// 0x623370
CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(const CTaskComplexKillPedOnFoot& o) :
    CTaskComplexKillPedOnFoot{
        o.m_PedToKill,
        o.m_time,
        o.m_pedFlags,
        o.m_actionDelay,
        o.m_actionChance,
        o.m_nCompetence
    } {
    m_bAimImmediate          = o.m_bAimImmediate;
    m_bWaitForPlayerToBeSafe = o.m_bWaitForPlayerToBeSafe;
}

// 0x625E40
bool CTaskComplexKillPedOnFoot::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x625E40, CTaskComplexKillPedOnFoot*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x62B150
CTask* CTaskComplexKillPedOnFoot::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x62B150, CTaskComplexKillPedOnFoot*, CPed*>(this, ped);
}

// 0x62B490
CTask* CTaskComplexKillPedOnFoot::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x62B490, CTaskComplexKillPedOnFoot*, CPed*>(this, ped);
}

// 0x626260
CTask* CTaskComplexKillPedOnFoot::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x626260, CTaskComplexKillPedOnFoot*, CPed*>(this, ped);
}

// 0x625E70
CTask* CTaskComplexKillPedOnFoot::CreateSubTask(int32 taskId, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x625E70, CTaskComplexKillPedOnFoot*, int32, CPed*>(this, taskId, ped);
}
