#pragma once

#include "Pool.h"
//#include <PatrolRoute.h>

class CPatrolRoutePool : public CPool<void*> {
//public:
//    static void InjectHooks() {
//        RH_ScopedClass(CPatrolRoutePool);
//        RH_ScopedCategory("Pools");
//
//        RH_ScopedInstall(New, 0x41B660);
//    }

public:
    using CPool::CPool;
};
