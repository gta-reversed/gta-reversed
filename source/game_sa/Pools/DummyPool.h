#pragma once

#include "Pool.h"
#include "Dummy.h"

class CDummyPool : public CPool<CDummy> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CDummyPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x00532630);
    }

public:
    using CPool::CPool;
};
