#include "StdInc.h"

#include "TaskSimpleDuckToggle.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleUseGun.h"

void CTaskSimpleDuckToggle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDuckToggle, 0x86dd98, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F690);
    RH_ScopedInstall(Destructor, 0x62F6D0);

    RH_ScopedVMTInstall(Clone, 0x635C90);
    RH_ScopedVMTInstall(GetTaskType, 0x62F6B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F6C0);
    RH_ScopedVMTInstall(ProcessPed, 0x62F6E0);
}

// 0x62F690
CTaskSimpleDuckToggle::CTaskSimpleDuckToggle(eMode mode) :
    m_ToggleMode{ mode }
{
}

// 0x62F6E0
bool CTaskSimpleDuckToggle::ProcessPed(CPed* ped) {
    auto* const intel = ped->GetIntelligence();

    // Try to toggle off
    if (auto* const tDuck = intel->GetTaskDuck(false); tDuck && notsa::contains({ eMode::OFF, eMode::AUTO }, m_ToggleMode)) {
        tDuck->MakeAbortable(ped, ABORT_PRIORITY_LEISURE);
        if (auto* const tUseGun = intel->GetTaskUseGun()) {
            tUseGun->ClearAnim(ped);
        }
        return true;
    }

    // Try to toggle on
    if (notsa::contains({ eMode::ON, eMode::AUTO }, m_ToggleMode)) {
        auto* const tSimplestDuck  = notsa::dyn_cast<CTaskSimpleDuck>(intel->GetTaskManager().GetSimplestActiveTask());
        auto* const tSecondaryDuck = intel->GetTaskDuck(true);

        const auto SetScriptControlledDuckTask = [intel] {
            intel->GetTaskManager().SetTaskSecondary(new CTaskSimpleDuck{ DUCK_SCRIPT_CONTROLLED, 0 }, TASK_SECONDARY_DUCK); // 0x62F7E9
        };

        if (!tSecondaryDuck) {   // 0x62F791
            if (tSimplestDuck) { // 0x62F79C
                tSimplestDuck->MakeAbortable(ped, ABORT_PRIORITY_URGENT);
            }
            SetScriptControlledDuckTask();
            if (auto* const tUseGun = intel->GetTaskUseGun()) {
                tUseGun->ClearAnim(ped);
            }
        } else if (tSecondaryDuck->GetControlType() != DUCK_SCRIPT_CONTROLLED) { // 0x62F821
            if (tSimplestDuck) {                                                 // 0x62F82F
                tSimplestDuck->AbortBecauseOfOtherDuck(ped);
            }
            if (!tSecondaryDuck->SetControlType(DUCK_SCRIPT_CONTROLLED)) { // 0x62F84A
                tSecondaryDuck->AbortBecauseOfOtherDuck(ped);
                SetScriptControlledDuckTask();
            }
        }
    }

    return true;
}
