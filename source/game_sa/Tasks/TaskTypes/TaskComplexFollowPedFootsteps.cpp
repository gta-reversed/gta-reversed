#include "StdInc.h"

#include "TaskComplexFollowPedFootsteps.h"
// #include "PointRoute.h"

void CTaskComplexFollowPedFootsteps::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPedFootsteps, 0x870cc0, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x694E20, "1", {}, CPed*);
    RH_ScopedDestructorInstall(0x695DC0);

    RH_ScopedVMTInstall(Clone, 0x695890);
    RH_ScopedVMTInstall(GetTaskType, 0x694EC0);
    RH_ScopedVMTInstall(MakeAbortable, 0x694ED0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x694EE0, { .Reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x695000, { .Reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x695090, { .Reversed = false });
    RH_ScopedVMTInstall(CreateSubTask, 0x695E40, { .Reversed = false });
}

// 0x694E20
CTaskComplexFollowPedFootsteps::CTaskComplexFollowPedFootsteps(CPed* ped) :
    m_targetPed{ ped },
    m_subTaskCreateCheckTimer{ CTimer::GetTimeInMS() },
    m_pointRoute{ new CPointRoute() } {
}

// 0x695DC0
CTaskComplexFollowPedFootsteps::~CTaskComplexFollowPedFootsteps() {
    delete std::exchange(m_pointRoute, nullptr);
}

// 0x694EE0
CTask* CTaskComplexFollowPedFootsteps::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x694EE0, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

// 0x695000
CTask* CTaskComplexFollowPedFootsteps::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695000, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

// 0x695090
CTask* CTaskComplexFollowPedFootsteps::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695090, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

// 0x695E40
CTask* CTaskComplexFollowPedFootsteps::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695E40, CTaskComplexFollowPedFootsteps*, eTaskType, CPed*>(this, taskType, ped);
}
