#pragma once

#include "TaskAllocator.h"

// Android
class CTaskAllocatorRetreat : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::TASK_ALLOCATOR_RETREAT;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_RETREAT; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
