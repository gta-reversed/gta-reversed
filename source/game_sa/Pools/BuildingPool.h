#pragma once

#include "Pool.h"
#include "Building.h"

class CBuildingPool : public CPool<CBuilding> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CBuildingPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x403FA0);
    }
};
