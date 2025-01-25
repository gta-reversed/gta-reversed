#pragma once

#include <Base.h>

#include "./PedGroupDefaultTaskAllocatorType.h"

class CPedGroup;
class CPed;

class CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    virtual void                              __stdcall AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) = 0;
    virtual ePedGroupDefaultTaskAllocatorType __stdcall GetType() = 0;
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocator, sizeof(void*)); /* vtable only */
