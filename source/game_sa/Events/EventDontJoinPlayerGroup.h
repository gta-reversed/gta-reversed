#pragma once

#include "Event.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventDontJoinPlayerGroup : public CEvent {
public:
    CPed* m_player;

public:
    static void InjectHooks();

    CEventDontJoinPlayerGroup(CPed* player);
    ~CEventDontJoinPlayerGroup() override;

    eEventType GetEventType() const override { return EVENT_DONT_JOIN_GROUP; }
    int32 GetEventPriority() const override { return 43; }
    int32 GetLifeTime() override { return 0; }
    CEventDontJoinPlayerGroup* Clone() override { return new CEventDontJoinPlayerGroup(m_player); }
    bool AffectsPed(CPed* ped) override { return true; }

    CEventDontJoinPlayerGroup* Constructor(CPed* player);
};
VALIDATE_SIZE(CEventDontJoinPlayerGroup, 0x10);
