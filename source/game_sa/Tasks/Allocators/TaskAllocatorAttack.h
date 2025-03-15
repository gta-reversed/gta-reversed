#pragma once

#include "TaskAllocator.h"
#include "ePedType.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorAttack : public CTaskAllocator {
public:
    constexpr static inline auto Type = TASK_ALLOCATOR_ATTACK;

    static void InjectHooks();

    CTaskAllocatorAttack(CPed* target, int32 groupTargetID, CPed* originator);
    ~CTaskAllocatorAttack() override = default;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_ATTACK; } // 0x69C2C0
    void               AllocateTasks(CPedGroupIntelligence* intel) override;
    CTaskAllocator*    ProcessGroup(CPedGroupIntelligence* intel) override;

public:
    CPed::Ref  m_Target;
    int32      m_GroupTargetID;
    CPed::Ref  m_Originator;
    CTaskTimer m_NextUpdateTimer;

private: // Wrappers for hooks
    // 0x69C240
    CTaskAllocatorAttack* Constructor(CPed * ped1, int32 groupId, CPed * ped2) {
        this->CTaskAllocatorAttack::CTaskAllocatorAttack(ped1, groupId, ped2);
        return this;
    }

    // 0x69C2D0
    CTaskAllocatorAttack* Destructor() {
        this->CTaskAllocatorAttack::~CTaskAllocatorAttack();
        return this;
    }
};
