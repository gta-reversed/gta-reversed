#include "StdInc.h"

#include "TaskSimpleSetStayInSamePlace.h"

// 0x62F590
CTaskSimpleSetStayInSamePlace::CTaskSimpleSetStayInSamePlace(bool bStayInSamePlace) :
    m_StayInSamePlace{ bStayInSamePlace } {
}

// 0x62F5E0
bool CTaskSimpleSetStayInSamePlace::ProcessPed(CPed* ped) {
    ped->bStayInSamePlace = m_StayInSamePlace;
    return true;
}

void CTaskSimpleSetStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetStayInSamePlace, 0x86dd50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x62F590, "1", {}, bool);
    RH_ScopedDestructorInstall(0x62F5D0);

    RH_ScopedVMTInstall(Clone, 0x635BD0);
    RH_ScopedVMTInstall(GetTaskType, 0x62F5B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F5C0);
    RH_ScopedVMTInstall(ProcessPed, 0x62F5E0);
}
