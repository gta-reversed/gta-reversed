#pragma once

#include "Pool.h"
#include "CutsceneObject.h"

class CObjectPool : public CPool<CObject, CCutsceneObject> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CObjectPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(GetAt, 0x404870);
        RH_ScopedInstall(New, 0x005A1C20);
    }
};
