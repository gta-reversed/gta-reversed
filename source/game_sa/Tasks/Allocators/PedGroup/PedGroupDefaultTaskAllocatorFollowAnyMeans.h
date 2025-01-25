#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class CPedGroupDefaultTaskAllocatorFollowAnyMeans final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              __stdcall AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override { ((void(__stdcall*)(CPedGroup*, CPed*))(0x5F6B40))(pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType __stdcall GetType() const override { return ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS; }; // 0x5F6470

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocator, 0x86C75C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6B40, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F6470);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorFollowAnyMeans, sizeof(void*)); /* vtable only */
