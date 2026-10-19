#pragma once

#include "Pool.h"
#include "TaskSimpleSlideToCoord.h"

class CTaskPool : public CPool<CTask, CTaskSimpleSlideToCoord> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CTaskPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x61A500);
    }

public:
    using CPool::CPool;
};
