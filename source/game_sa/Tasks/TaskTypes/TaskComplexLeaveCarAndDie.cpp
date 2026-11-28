#include "StdInc.h"
#include "TaskComplexLeaveCarAndDie.h"

void CTaskComplexLeaveCarAndDie::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexLeaveCarAndDie, 0x86dd00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(Clone, 0x62F1D0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F240);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x642280);
}

// NOTSA
CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie& o) :
    CTaskComplexLeaveCar{ o.m_TargetVehicle, o.m_TargetDoor, o.m_DelayTime, false, true } {
    m_Die = true;
}

// 0x642280
CTask* CTaskComplexLeaveCarAndDie::CreateFirstSubTask(CPed* ped) {
    ped->m_nPedState = PEDSTATE_DIE; // Don't use `SetPedState` because it has side-effects
    return CTaskComplexLeaveCar::CreateFirstSubTask(ped);
}
