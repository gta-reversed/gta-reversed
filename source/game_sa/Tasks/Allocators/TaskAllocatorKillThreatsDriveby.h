#pragma once

#include "TaskAllocatorKillThreatsBasic.h"

// Originally didn't inherit from `CTaskAllocatorKillThreatsBasic`,
// but it has the same layout (`m_Threat`, `m_Timer`), so why not
class NOTSA_EXPORT_VTABLE CTaskAllocatorKillThreatsDriveby final : public CTaskAllocatorKillThreatsBasic {
public:
    constexpr static inline auto Type = eTaskAllocatorType::KILL_THREATS_DRIVEBY;

    static void InjectHooks();

    using CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic;
    ~CTaskAllocatorKillThreatsDriveby() override = default;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::KILL_THREATS_DRIVEBY; }; // 0x69CB50
    void               AllocateTasks(CPedGroupIntelligence* intel) override;

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
VALIDATE_SIZE(CTaskAllocatorKillThreatsDriveby, 0x14);
