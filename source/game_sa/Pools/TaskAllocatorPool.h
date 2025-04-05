#pragma once

#include "Pool.h"
#include "Allocators/TaskAllocator.h"

class CTaskAllocatorPool : public CPool<CTaskAllocator, CTaskAllocator> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CTaskAllocatorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0069D8E0);
    }

public:
    using CPool::CPool;
};
