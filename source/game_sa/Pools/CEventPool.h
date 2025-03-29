#pragma once

#include "Pool.h"
#include <Events/EventDamage.h>
#include "Event.h"

class CEventPool : public CPool<CEvent, CEventDamage> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CEventPool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x4B5570);
    }
};
