#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class CPedGroupDefaultTaskAllocatorStandStill final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              __stdcall AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) override { ((void(__stdcall*)(CPedGroup*, CPed*))(0x5F6DA0))(pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType __stdcall GetType() override { return ePedGroupDefaultTaskAllocatorType::STAND_STILL; }; // 0x5F64D0

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocator, 0x86C76C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6DA0, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F64D0);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorStandStill, sizeof(void*)); /* vtable only */
