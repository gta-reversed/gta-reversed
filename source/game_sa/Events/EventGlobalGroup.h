#pragma once

#include "EventGroup.h"

class CEntity;
class CPed;
class CPedGroup;

class CEventGlobalGroup : public CEventGroup {
public:
    static void InjectHooks();

    CEventGlobalGroup(CPed* ped) : CEventGroup(ped) {};
    ~CEventGlobalGroup() {}

    float GetSoundLevel(CEntity* entity, CVector& position);
    void AddEventsToPed(CPed* ped);
    void AddEventsToGroup(CPedGroup* pedGroup);
};

