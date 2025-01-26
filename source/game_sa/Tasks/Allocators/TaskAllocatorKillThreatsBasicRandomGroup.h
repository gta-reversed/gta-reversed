#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

class CTaskAllocatorKillThreatsBasicRandomGroup final : public CTaskAllocatorKillThreatsBasic {
public:
    static void InjectHooks();

    using CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic;
    ~CTaskAllocatorKillThreatsBasicRandomGroup() override = default;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_THREATS_BASIC_RANDOM_GROUP; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
