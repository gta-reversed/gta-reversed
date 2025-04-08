#pragma once

#include "Pool.h"
#include "PedIntelligence.h"

class CPedIntelligencePool : public CPool<CPedIntelligence> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPedIntelligencePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0605EC0);
    }

public:
    using CPool::CPool;
};
