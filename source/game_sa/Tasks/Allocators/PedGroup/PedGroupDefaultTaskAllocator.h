#pragma once

#include <Base.h>

#include "./PedGroupDefaultTaskAllocatorType.h"

class CPedGroup;
class CPed;

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    virtual void                              AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const = 0;
    virtual ePedGroupDefaultTaskAllocatorType GetType() const = 0;
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocator, sizeof(void*)); /* vtable only */
