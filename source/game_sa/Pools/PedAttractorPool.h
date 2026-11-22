#pragma once

#include "Pool.h"
#include "PedAttractor.h"

class CPedAttractorPool : public CPool<CPedAttractor> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedAttractorPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x5EA9F0);
    }

public:
    using CPool::CPool;
};
