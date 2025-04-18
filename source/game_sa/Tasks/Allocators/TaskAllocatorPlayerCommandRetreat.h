#pragma once

#include "TaskAllocator.h"

// Android
class CTaskAllocatorPlayerCommandRetreat : public CTaskAllocator {
public:
    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_PLAYER_COMMAND_RETREAT; }
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
