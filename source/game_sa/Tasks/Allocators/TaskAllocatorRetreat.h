#pragma once

#include "TaskAllocator.h"

// Android
class CTaskAllocatorRetreat : public CTaskAllocator {
public:
    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_RETREAT; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
