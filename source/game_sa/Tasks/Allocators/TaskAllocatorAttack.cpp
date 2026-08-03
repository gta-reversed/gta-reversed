#include "StdInc.h"

#include "TaskAllocatorAttack.h"
#include "TaskAllocatorKillOnFoot.h"

// 0x69C240
CTaskAllocatorAttack::CTaskAllocatorAttack(CPed* target, int32 groupTargetID, CPed* originator) :
    m_Target{target},
    m_GroupTargetID{groupTargetID},
    m_Originator{originator}
{ }

// 0x69C3F0
void CTaskAllocatorAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    m_NextUpdateTimer.Start(3'000);
    CTaskAllocatorKillOnFoot{m_Target, m_GroupTargetID}.AllocateTasks(intel);
}

// 0x69D0C0
CTaskAllocator* CTaskAllocatorAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    if (!m_NextUpdateTimer.IsStarted() || m_NextUpdateTimer.IsOutOfTime()) {
        AllocateTasks(intel);
    }
    return this;
}

void CTaskAllocatorAttack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorAttack, 0x870e60, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69C240);
    RH_ScopedInstall(Destructor, 0x69C2D0);

    RH_ScopedVMTInstall(GetType, 0x69C2C0);
    RH_ScopedVMTInstall(AllocateTasks, 0x69C3F0);
    RH_ScopedVMTInstall(ProcessGroup, 0x69D0C0);
}
