#include "StdInc.h"

#include "TaskAllocatorKillOnFoot.h"

// 0x69BB80
CTaskAllocatorKillOnFoot::CTaskAllocatorKillOnFoot(CPed* ped, int32 groupId) {
    m_Ped0 = ped;
    m_GroupId = groupId;
    CEntity::SafeRegisterRef(m_Ped0);
}

// 0x69BBF0
CTaskAllocatorKillOnFoot::~CTaskAllocatorKillOnFoot() {
    CEntity::SafeCleanUpRef(m_Ped0);
}

// 0x69BCA0
void CTaskAllocatorKillOnFoot::AllocateTasks(CPedGroupIntelligence* intel) {
    plugin::CallMethod<0x69BCA0, CTaskAllocatorKillOnFoot*, CPedGroupIntelligence*>(this, intel);
}
