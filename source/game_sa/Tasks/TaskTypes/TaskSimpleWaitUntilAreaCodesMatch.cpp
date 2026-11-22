#include "StdInc.h"
#include "TaskSimpleWaitUntilAreaCodesMatch.h"
#include "EntryExitManager.h"

void CTaskSimpleWaitUntilAreaCodesMatch::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitUntilAreaCodesMatch, 0x86e544, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x635540);
    RH_ScopedInstall(Destructor, 0x6355E0);

    RH_ScopedVMTInstall(Clone, 0x637190);
    RH_ScopedVMTInstall(GetTaskType, 0x6355C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6355D0);
    RH_ScopedVMTInstall(ProcessPed, 0x635640);
}

// 0x635540
CTaskSimpleWaitUntilAreaCodesMatch::CTaskSimpleWaitUntilAreaCodesMatch(CPed* pedToWaitFor) :
    CTaskSimpleStandStill{1000, true, false, 8.f},
    m_pedToWaitFor{pedToWaitFor}
{
    CEntity::SafeRegisterRef(m_pedToWaitFor);
}

// 0x6355E0
CTaskSimpleWaitUntilAreaCodesMatch::~CTaskSimpleWaitUntilAreaCodesMatch() {
    CEntity::SafeCleanUpRef(m_pedToWaitFor);
}

// 0x635640
bool CTaskSimpleWaitUntilAreaCodesMatch::ProcessPed(CPed* ped) {
    if (!m_pedToWaitFor) {
        return true;
    }

    if (!CEntryExitManager::ms_exitEnterState) {
        if (m_pedToWaitFor->GetAreaCode() == ped->GetAreaCode()) {
            return true;
        }
    }

    return !ped->bInVehicle && CTaskSimpleStandStill::ProcessPed(ped);
}
