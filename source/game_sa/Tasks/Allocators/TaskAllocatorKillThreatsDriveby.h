#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

// Originally didn't inherit from `CTaskAllocatorKillThreatsBasic`,
// but they share a few things, so why not
class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsDriveby final : public CTaskAllocatorKillThreatsBasic {
public:
    constexpr static inline auto Type = eTaskAllocatorType::TASK_ALLOCATOR_KILL_THREATS_DRIVEBY;

    static void InjectHooks();

    using CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic;
    ~CTaskAllocatorKillThreatsDriveby() override = default;

    eTaskAllocatorType GetType() override { return TASK_ALLOCATOR_KILL_THREATS_DRIVEBY; }; // 0x69CB50
    void               AllocateTasks(CPedGroupIntelligence* intel) override;

public:
    CPed::Ref  m_Threat{};
    CTaskTimer m_Timer{};

private: // Wrappers for hooks
    // 0x69CAF0
    CTaskAllocatorKillThreatsDriveby* Constructor(CPed* a) {
        this->CTaskAllocatorKillThreatsDriveby::CTaskAllocatorKillThreatsDriveby(a);
        return this;
    }

    // 0x69CB60
    CTaskAllocatorKillThreatsDriveby* Destructor() {
        this->CTaskAllocatorKillThreatsDriveby::~CTaskAllocatorKillThreatsDriveby();
        return this;
    }
};
