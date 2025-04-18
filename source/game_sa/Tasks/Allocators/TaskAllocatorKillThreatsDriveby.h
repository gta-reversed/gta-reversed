#pragma once

#include "TaskAllocator.h"

class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsDriveby : public CTaskAllocator {
public:
    CEntity* m_pEntity;
    uint32   m_nTime;
    uint32   m_nTimeOffset;
    int8     byte10;
    int8     byte11;

public:
    explicit CTaskAllocatorKillThreatsDriveby(CPed* ped);
    ~CTaskAllocatorKillThreatsDriveby() override;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_THREATS_DRIVEBY; }; // 0x69CB50
    void ProcessGroup(CPedGroupIntelligence* intel) override;
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
