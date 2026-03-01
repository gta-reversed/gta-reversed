#include "StdInc.h"
#include "TaskComplexEnterCarAsPassengerWait.h"

void CTaskComplexEnterCarAsPassengerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexEnterCarAsPassengerWait, 0x86E778, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B320);
    RH_ScopedInstall(Destructor, 0x63B3C0);

    RH_ScopedInstall(CreateSubTask, 0x6408D0, {.reversed = false});
    RH_ScopedVMTInstall(Clone, 0x63D850);
    RH_ScopedVMTInstall(GetTaskType, 0x63B3B0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x643E10, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x643F00, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x640730, {.reversed = false});
}

// 0x63B320
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(CVehicle* target, CPed* waitFor, bool forceFrontSeat, eMoveState ms) :
    m_Car{target},
    m_WaitForPed{waitFor},
    m_bForceFrontSeat{forceFrontSeat},
    m_MoveState{ms}
{
    CEntity::SafeRegisterRef(m_Car);
    CEntity::SafeRegisterRef(m_WaitForPed);
}
    
// NOTSA (for 0x63D850)
CTaskComplexEnterCarAsPassengerWait::CTaskComplexEnterCarAsPassengerWait(const CTaskComplexEnterCarAsPassengerWait& o) :
    CTaskComplexEnterCarAsPassengerWait{
        o.m_Car,
        o.m_WaitForPed,
        o.m_bForceFrontSeat,
        o.m_MoveState
    }
{
}

// 0x63B3C0
CTaskComplexEnterCarAsPassengerWait::~CTaskComplexEnterCarAsPassengerWait() {
    CEntity::SafeCleanUpRef(m_Car);
    CEntity::SafeCleanUpRef(m_WaitForPed);
}

// 0x6408D0
CTaskComplexEnterCarAsPassengerTimed* CTaskComplexEnterCarAsPassengerWait::CreateSubTask(int32 taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTaskComplexEnterCarAsPassengerTimed*, 0x6408D0, CTaskComplexEnterCarAsPassengerWait*, int32, CPed*>(this, taskType, ped);
}

// 0x643E10
CTask* CTaskComplexEnterCarAsPassengerWait::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x643E10, CTaskComplexEnterCarAsPassengerWait*, CPed*>(this, ped);
}

// 0x643F00
CTask* CTaskComplexEnterCarAsPassengerWait::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x643F00, CTaskComplexEnterCarAsPassengerWait*, CPed*>(this, ped);
}

// 0x640730
CTask* CTaskComplexEnterCarAsPassengerWait::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x640730, CTaskComplexEnterCarAsPassengerWait*, CPed*>(this, ped);
}
