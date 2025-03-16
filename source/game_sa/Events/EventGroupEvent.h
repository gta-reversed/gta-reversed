#pragma once

#include "Event.h"
#include "PedGroup.h"

class CPed;

class NOTSA_EXPORT_VTABLE CEventGroupEvent : public CEvent {
public:
    CPed*   m_ped;
    CEvent* m_event;

public:
    CEventGroupEvent(CPed* ped, CEvent* event);
    ~CEventGroupEvent() override;

    eEventType GetEventType() const override { return EVENT_GROUP_EVENT; }
    int32 GetEventPriority() const override { return 41; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override { return false; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override { return pedGroup->GetMembership().IsMember(m_ped); }
    float GetLocalSoundLevel() override { return 100.0f; }

    bool BaseEventTakesPriorityOverBaseEvent(const CEventGroupEvent& other);

    auto& GetEvent() const { return *m_event; }

private:
    bool IsPriorityEvent() const;
};

VALIDATE_SIZE(CEventGroupEvent, 0x14);
