#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorStandStill final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override { plugin::CallMethod<0x5F6DA0>(this, pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::STAND_STILL; }; // 0x5F64D0

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorStandStill, 0x86C76C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6DA0, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F64D0);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorStandStill, sizeof(void*)); /* vtable only */
