#pragma once

#include "Base.h"

class CEvent;
class CPed;

class CEventSource {
public:
    static int32 ComputeEventSourceType(const CEvent& event, const CPed& ped);

public:
    static void InjectHooks();
};

