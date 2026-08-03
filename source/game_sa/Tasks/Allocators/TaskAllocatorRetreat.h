#pragma once

#include "TaskAllocator.h"

// Android
class NOTSA_EXPORT_VTABLE CTaskAllocatorRetreat final : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::RETREAT;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::RETREAT; }; // 0x5F68F0
    void AllocateTasks(CPedGroupIntelligence* intel) override { /* nop */ }
};
