#include "StdInc.h"

#include "TaskSimpleDieInCar.h"
#include "TaskSimpleDie.h"

// 0x62FC20
CTaskSimpleDieInCar::CTaskSimpleDieInCar(AssocGroupId groupId, AnimationId animId) :
    CTaskSimpleDie(groupId, animId, 4.0f, 0.0f) {
    // NOP
}

// 0x6398F0
bool CTaskSimpleDieInCar::ProcessPed(CPed* ped) {
    return CTaskSimpleDie::ProcessPed(ped);
}

void CTaskSimpleDieInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDieInCar, 0x86dde0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x62FC20, "1", {}, AssocGroupId, AnimationId);
    RH_ScopedDestructorInstall(0x62FC70);

    RH_ScopedVMTInstall(Clone, 0x635EF0);
    RH_ScopedVMTInstall(GetTaskType, 0x62FC60);
    RH_ScopedVMTInstall(ProcessPed, 0x6398F0);
}
