#pragma once

#include "Pool.h"
#include "CopPed.h"
#include "Ped.h"

class CPedPool : public CPool<CPed, CCopPed> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x005E45E0);
        RH_ScopedInstall(GetAtRef, 0x404910);
    }
};
