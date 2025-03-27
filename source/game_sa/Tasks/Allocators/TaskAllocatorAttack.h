#pragma once

#include "TaskAllocator.h"
#include "ePedType.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorAttack : public CTaskAllocator {
public:
    int32    f14;
    int8     f18;
    int8     f19;
    int32    m_nGroupId;
    ePedType m_PedType;
    uint32   m_nTime;
    uint32   m_nTimeOffset;
    bool     m_bAllocated;
    bool     m_bUpdateTime;

public:
    CTaskAllocatorAttack(CPed* ped0, int, CPed* ped1);

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_ATTACK; }; // 0x69C2C0
    void AllocateTasks(CPedGroupIntelligence* intel) override;
    void ProcessGroup(CPedGroupIntelligence* intel) override;
};
