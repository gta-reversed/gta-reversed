#pragma once

#include "TaskAllocator.h"

// Android
class NOTSA_EXPORT_VTABLE CTaskAllocatorPlayerCommandRetreat final : public CTaskAllocator {
public:
    constexpr static inline auto Type = eTaskAllocatorType::PLAYER_COMMAND_RETREAT;

    eTaskAllocatorType GetType() override { return eTaskAllocatorType::PLAYER_COMMAND_RETREAT; }
    void AllocateTasks(CPedGroupIntelligence* intel) override;
};
