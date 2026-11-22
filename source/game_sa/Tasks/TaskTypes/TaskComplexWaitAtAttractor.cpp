#include "StdInc.h"
#include "TaskComplexWaitAtAttractor.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexWaitAtAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitAtAttractor, 0x86e108, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6325F0);
    RH_ScopedInstall(Destructor, 0x632630);

    RH_ScopedVMTInstall(Clone, 0x636940);
    RH_ScopedVMTInstall(GetTaskType, 0x632620);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632640);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632650);
    RH_ScopedVMTInstall(ControlSubTask, 0x6326E0);
}

// 0x6325F0
CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(CPedAttractor* attractor, int32 slot) :
    m_attractor{attractor},
    m_slot{slot}
{
}

// 0x632630
CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(const CTaskComplexWaitAtAttractor& o) :
    CTaskComplexWaitAtAttractor{o.m_attractor, o.m_slot}
{
}

// 0x632650
CTask* CTaskComplexWaitAtAttractor::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 10'000 };
}
