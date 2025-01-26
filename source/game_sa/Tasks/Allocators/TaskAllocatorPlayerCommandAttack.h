#pragma once

#include "TaskAllocatorAttack.h"
#include "ePedType.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorPlayerCommandAttack final : public CTaskAllocatorAttack {
public:
    static void InjectHooks();

    CTaskAllocatorPlayerCommandAttack(CPed* target, int32 groupTargetID, ePedType pedTypeToAttack);
    ~CTaskAllocatorPlayerCommandAttack() override = default;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_PLAYER_COMMAND_ATTACK; }; // 0x69C4C0
    void               AllocateTasks(CPedGroupIntelligence* intel) override; // 0x69C4D0
    CTaskAllocator*    ProcessGroup(CPedGroupIntelligence* intel) override; // 0x69D110

public:
    ePedType m_PedTypeToAttack;
};
