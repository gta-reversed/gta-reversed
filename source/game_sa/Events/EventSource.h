#pragma once

#include "Base.h"
#include "DecisionMakers/Decision.h"

class CEvent;
class CPed;

class CEventSource {
public:
    static eDecisionRelationship ComputeEventSourceType(const CEvent& event, const CPed& ped);

public:
    static void InjectHooks();
};
