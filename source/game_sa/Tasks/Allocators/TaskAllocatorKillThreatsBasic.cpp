#include "StdInc.h"

#include "TaskAllocatorKillThreatsBasic.h"

// 0x69C710
CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic(CPed* ped) : CTaskAllocator(ped) {
    CEntity::SafeRegisterRef(m_Ped0);
}

// 0x69C780
CTaskAllocatorKillThreatsBasic::~CTaskAllocatorKillThreatsBasic() {
    CEntity::SafeCleanUpRef(m_Ped0);
}

// 0x69D170
void CTaskAllocatorKillThreatsBasic::AllocateTasks(CPedGroupIntelligence* intel) {
    plugin::CallMethod<0x69D170, CTaskAllocatorKillThreatsBasic*, CPedGroupIntelligence*>(this, intel);
}

// 0x69C7E0
CTaskAllocator* CTaskAllocatorKillThreatsBasic::ProcessGroup(CPedGroupIntelligence* intel) {
    return plugin::CallMethodAndReturn<CTaskAllocator*, 0x69C7E0>(this, intel);
}

// 0x69C850
void CTaskAllocatorKillThreatsBasic::ComputeClosestPeds(CPedGroup& group1, CPedGroup& group2, CPed** peds) {
    plugin::CallMethod<0x69C850, CTaskAllocatorKillThreatsBasic*, CPedGroup&, CPedGroup&, CPed**>(this, group1, group2, peds);
}
