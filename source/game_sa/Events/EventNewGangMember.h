#pragma once

#include "Event.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventNewGangMember : public CEvent {
public:
    CPed* m_member;

public:
    static void InjectHooks();

    CEventNewGangMember(CPed* member);
    ~CEventNewGangMember() override;

    eEventType GetEventType() const override { return EVENT_NEW_GANG_MEMBER; }
    int32 GetEventPriority() const override { return 42; }
    int32 GetLifeTime() override { return 0; }
    CEventNewGangMember* Clone() override { return new CEventNewGangMember(m_member); }
    bool AffectsPed(CPed* ped) override { return true; }

    CEventNewGangMember* Constructor(CPed* member);
};
VALIDATE_SIZE(CEventNewGangMember, 0x10);
