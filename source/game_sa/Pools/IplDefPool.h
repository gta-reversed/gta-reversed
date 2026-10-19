#pragma once

#include "Pool.h"
#include <IplDef.h>

class CIplDefPool : public CPool<IplDef> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CIplDefPool);
        RH_ScopedCategory("Pools");

        //RH_ScopedInstall(Constructor, 0x405900);
        rh->InstallStatic(RHCurrentCat.name + "/" + RHCurrentScopeName.name, "New", 0x004059B0, &RHCurrentNS::New);
    }

public:
    using CPool::CPool;
};
