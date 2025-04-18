#pragma once

#include "TaskAllocatorAttack.h"
#include "ePedType.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorPlayerCommandAttack : public CTaskAllocatorAttack {
public:
    CTaskAllocatorPlayerCommandAttack(CPed* ped, int32 groupId, ePedType pedType);

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_PLAYER_COMMAND_ATTACK; }; // 0x69C4C0
    void AllocateTasks(CPedGroupIntelligence* intel) override; // 0x69C4D0
    void ProcessGroup(CPedGroupIntelligence* intel) override; // 0x69D110
};
