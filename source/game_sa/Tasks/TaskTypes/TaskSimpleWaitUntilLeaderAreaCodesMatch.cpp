#include "StdInc.h"
#include "TaskSimpleWaitUntilLeaderAreaCodesMatch.h"

void CTaskSimpleWaitUntilLeaderAreaCodesMatch::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilLeaderAreaCodesMatch, 0x86c7e8, 9);
    RH_ScopedCategory("Tasks/TaskTypes"); 

    RH_ScopedVMTInstall(Clone, 0x5F67F0);
    RH_ScopedVMTInstall(GetTaskType, 0x5F6870);
    RH_ScopedVMTInstall(MakeAbortable, 0x6356A0);
    RH_ScopedVMTInstall(ProcessPed, 0x6356C0);
}

// 0x6356A0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    } else {
        ped->m_bUsesCollision = true;
        return true;
    }
}

// 0x6356C0
bool CTaskSimpleWaitUntilLeaderAreaCodesMatch::ProcessPed(CPed* ped) {
    if (CTaskSimpleWaitUntilAreaCodesMatch::ProcessPed(ped)) {
        ped->m_bUsesCollision = true;
        return true;
    }
    if (!m_b) {
        if (ped->GetTaskManager().Find<Type>()) {
            m_b = true;
            ped->m_bUsesCollision = false;
        }
    }
    return false;
}
