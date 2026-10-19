#pragma once

#include "Pool.h"
#include "Heli.h"

class CVehiclePool : public CPool<CVehicle, CHeli> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CVehiclePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x006E2A50);
    }

public:
    using CPool::CPool;
};
