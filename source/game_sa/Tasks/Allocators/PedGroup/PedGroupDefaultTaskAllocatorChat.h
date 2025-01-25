#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

class CPedGroupDefaultTaskAllocatorChat final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    void                              __stdcall AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override { ((void(__stdcall*)(CPedGroup*, CPed*))(0x5F8180))(pedGroup, ped); };
    ePedGroupDefaultTaskAllocatorType __stdcall GetType() const override { return ePedGroupDefaultTaskAllocatorType::CHAT; }; // 0x5F6500

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocator, 0x86C774, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F8180, { .reversed = false });
        RH_ScopedVMTInstall(GetType, 0x5F6500);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorChat, sizeof(void*)); /* vtable only */
