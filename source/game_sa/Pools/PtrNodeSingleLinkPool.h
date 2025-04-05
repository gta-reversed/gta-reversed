#pragma once

#include "Pool.h"
#include "PtrNodeSingleLink.h"

class CPtrNodeSingleLinkPool : public CPool<CPtrNodeSingleLink<void*>> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPtrNodeSingleLinkPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0552240);
    }
};
