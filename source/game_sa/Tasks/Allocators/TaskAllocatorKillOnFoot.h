#pragma once

#include "TaskAllocator.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorKillOnFoot : public CTaskAllocator {
public:
    CTaskAllocatorKillOnFoot(CPed* ped, int32 groupId);
    ~CTaskAllocatorKillOnFoot() override;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_ON_FOOT; }; // 0x69BBE0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
