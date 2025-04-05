#pragma once

#include "Pool.h"
#include "PedGeometryAnalyser.h"

class CPointRoutePool : public CPool<CPointRoute> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPointRoutePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0041B5B0);
    }
};
