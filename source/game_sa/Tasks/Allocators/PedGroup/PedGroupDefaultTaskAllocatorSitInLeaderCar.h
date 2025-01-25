#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorSitInLeaderCar final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override { plugin::CallMethod<0x5F6FC0>(this, pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR; }; // 0x5F6560

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorSitInLeaderCar, 0x86C784, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6FC0, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F6560);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorSitInLeaderCar, sizeof(void*)); /* vtable only */
