#include "StdInc.h"

#include "TaskAllocatorKillThreatsDriveby.h"

// 0x69CAF0
CTaskAllocatorKillThreatsDriveby::CTaskAllocatorKillThreatsDriveby(CPed* ped) : CTaskAllocator(ped) {
    CEntity::SafeRegisterRef(m_Ped0);
}

// 0x69CB60
CTaskAllocatorKillThreatsDriveby::~CTaskAllocatorKillThreatsDriveby() {
    CEntity::SafeCleanUpRef(m_Ped0);
}

// 0x69CBC0
CTaskAllocator* CTaskAllocatorKillThreatsDriveby::ProcessGroup(CPedGroupIntelligence* intel) {
    return plugin::CallMethodAndReturn<CTaskAllocator*, 0x69CBC0>(this, intel);
}

// 0x69CC30
void CTaskAllocatorKillThreatsDriveby::AllocateTasks(CPedGroupIntelligence* intel) {
    plugin::CallMethod<0x69CC30, CTaskAllocatorKillThreatsDriveby*, CPedGroupIntelligence*>(this, intel);
}
