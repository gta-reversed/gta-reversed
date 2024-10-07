#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

class CTaskAllocatorKillThreatsBasicRandomGroup : public CTaskAllocatorKillThreatsBasic {
public:
    using CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_THREATS_BASIC_RANDOM_GROUP; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
