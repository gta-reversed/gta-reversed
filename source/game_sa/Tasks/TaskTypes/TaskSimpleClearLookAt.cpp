#include "StdInc.h"
#include "TaskSimpleClearLookAt.h"
#include "Ragdoll/IKChainManager.h"

void CTaskSimpleClearLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleClearLookAt, 0x86E404, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x634670);
    RH_ScopedInstall(Destructor, 0x6346A0);

    RH_ScopedVMTInstall(Clone, 0x636F20);
    RH_ScopedVMTInstall(GetTaskType, 0x634690);
    RH_ScopedVMTInstall(MakeAbortable, 0x6346B0);
    RH_ScopedVMTInstall(ProcessPed, 0x6346C0);
}

// 0x6346C0
bool CTaskSimpleClearLookAt::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500u);
    }
    return true;
}
