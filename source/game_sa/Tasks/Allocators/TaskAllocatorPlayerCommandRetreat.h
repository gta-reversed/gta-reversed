#pragma once

#include "TaskAllocator.h"

// Android
class CTaskAllocatorPlayerCommandRetreat : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::TASK_ALLOCATOR_PLAYER_COMMAND_RETREAT;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_PLAYER_COMMAND_RETREAT; }
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
