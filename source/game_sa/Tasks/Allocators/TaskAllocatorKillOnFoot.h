#pragma once

#include "TaskAllocator.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorKillOnFoot : public CTaskAllocator {
public:
    constexpr static inline auto Type = TASK_ALLOCATOR_KILL_ON_FOOT;

    static void InjectHooks();

public:
    CTaskAllocatorKillOnFoot(CPed* ped, int32 groupId);
    ~CTaskAllocatorKillOnFoot() override = default;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_ON_FOOT; }; // 0x69BBE0
    void AllocateTasks(CPedGroupIntelligence* intel) override;

private:
    CPed::Ref m_Target{};
    int32     m_GroupTargetID{};

private:
    // 0x69BB80
    CTaskAllocatorKillOnFoot* Constructor(CPed* a, int32 b) {
        this->CTaskAllocatorKillOnFoot::CTaskAllocatorKillOnFoot(a, b);
        return this;
    }

    // 0x69BBF0
    CTaskAllocatorKillOnFoot* Destructor() {
        this->CTaskAllocatorKillOnFoot::~CTaskAllocatorKillOnFoot();
        return this;
    }
};
