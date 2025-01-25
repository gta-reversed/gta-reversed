#include "StdInc.h"

#include "TaskAllocatorAttack.h"
#include "TaskAllocatorKillOnFoot.h"

// 0x69C240
CTaskAllocatorAttack::CTaskAllocatorAttack(CPed* ped1, int32 groupId, CPed* ped2) {
    m_Ped0 = ped1;
    m_Ped1 = ped2;
    m_GroupId = groupId;
    m_Time = 0;
    f14 = 0;
    f18 = 0;
    f19 = 0;

    CEntity::SafeRegisterRef(m_Ped0);
    CEntity::SafeRegisterRef(m_Ped1);
}

// 0x69C3F0
void CTaskAllocatorAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    m_Time = CTimer::GetTimeInMS();
    f14 = 3000;
    f18 = 1;
    auto task = CTaskAllocatorKillOnFoot(m_Ped0, m_GroupId);
    task.AllocateTasks(intel);
}

// 0x69D0C0
CTaskAllocator* CTaskAllocatorAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    return CTaskAllocator::ProcessGroup(intel);
}
