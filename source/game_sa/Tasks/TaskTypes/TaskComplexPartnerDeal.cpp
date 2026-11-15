#include "StdInc.h"

#include "TaskComplexPartnerDeal.h"

void CTaskComplexPartnerDeal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexPartnerDeal, 0x870754, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x684190, "1", {}, const char *, CPed *, bool, float, CVector);
    RH_ScopedDestructorInstall(0x684200);

    RH_ScopedVMTInstall(Clone, 0x684D10);
    RH_ScopedVMTInstall(GetTaskType, 0x681DF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6823B0, { .Reversed = false });
    RH_ScopedVMTInstall(StreamRequiredAnims, 0x6823C0, { .Reversed = false });
    RH_ScopedVMTInstall(GetPartnerSequence, 0x682440, { .Reversed = false });
}

CTaskComplexPartnerDeal::CTaskComplexPartnerDeal(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, true, 1, point) {
    m_taskId = TASK_COMPLEX_PARTNER_DEAL;
    strcpy_s(m_animBlockName, "gangs");
}

CTask* CTaskComplexPartnerDeal::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6823B0, CTask*, CPed*>(this, ped);
}

void CTaskComplexPartnerDeal::StreamRequiredAnims() {
    return plugin::CallMethod<0x6823C0, CTask*>(this);
}

CTaskComplexSequence* CTaskComplexPartnerDeal::GetPartnerSequence() {
    return plugin::CallMethodAndReturn<CTaskComplexSequence*, 0x682440, CTask*>(this);
}
