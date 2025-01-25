#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorFollowLimited final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override { plugin::CallMethod<0x5F6C70>(this, pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::FOLLOW_LIMITED; }; // 0x5F64A0

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorFollowLimited, 0x86C764, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6C70, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F64A0);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorFollowLimited, sizeof(void*)); /* vtable only */
