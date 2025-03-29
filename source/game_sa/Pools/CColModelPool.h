#pragma once

#include "Pool.h"
#include "ColModel.h"

class CColModelPool : public CPool<CColModel> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CColModelPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x0040FB80);
    }
};
