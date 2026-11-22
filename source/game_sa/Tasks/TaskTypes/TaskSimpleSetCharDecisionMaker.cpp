#include "StdInc.h"
#include "TaskSimpleSetCharDecisionMaker.h"

void CTaskSimpleSetCharDecisionMaker::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetCharDecisionMaker, 0x859da0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x46A470);
    RH_ScopedInstall(Destructor, 0x46AF10);

    RH_ScopedVMTInstall(Clone, 0x46A490);
    RH_ScopedVMTInstall(GetTaskType, 0x46A4F0);
    RH_ScopedVMTInstall(MakeAbortable, 0x46A500);
    RH_ScopedVMTInstall(ProcessPed, 0x635430);
}

// 0x46A470
CTaskSimpleSetCharDecisionMaker::CTaskSimpleSetCharDecisionMaker(uint32 decisionMaker) :
    m_decisionMaker{ decisionMaker }
{
}

CTaskSimpleSetCharDecisionMaker::CTaskSimpleSetCharDecisionMaker(const CTaskSimpleSetCharDecisionMaker& o) :
    CTaskSimpleSetCharDecisionMaker{o.m_decisionMaker}
{
}

// 0x635430
bool CTaskSimpleSetCharDecisionMaker::ProcessPed(CPed* ped) {
    ped->GetIntelligence()->SetPedDecisionMakerType(m_decisionMaker);
    return true;
}
