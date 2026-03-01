#include "StdInc.h"
#include "TaskSimpleSetKindaStayInSamePlace.h"

void CTaskSimpleSetKindaStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetKindaStayInSamePlace, 0x86dd74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F610);
    RH_ScopedInstall(Destructor, 0x62F650);

    RH_ScopedVMTInstall(Clone, 0x635C30);
    RH_ScopedVMTInstall(GetTaskType, 0x62F630);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F640);
    RH_ScopedVMTInstall(ProcessPed, 0x62F660);
}

// 0x62F610
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(bool bKindaStayInSamePlace) :
    m_bKindaStayInSamePlace{ bKindaStayInSamePlace }
{
}

// 0x635C30
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(const CTaskSimpleSetKindaStayInSamePlace& o) :
    CTaskSimpleSetKindaStayInSamePlace{ o.m_bKindaStayInSamePlace }
{
}
