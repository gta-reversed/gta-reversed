#pragma once

#include "Pool.h"
#include "PtrNodeDoubleLink.h"

class CPtrNodeDoubleLinkPool : public CPool<CPtrNodeDoubleLink<void*>> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeDoubleLinkPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x05522E0);
    }

public:
    using CPool::CPool;
};
