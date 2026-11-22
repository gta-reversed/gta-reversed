#pragma once

#include "Event.h"
#include "PedGroup.h"
#include "Ped.h"


class NOTSA_EXPORT_VTABLE CEventGroupEvent : public CEvent {
public:
    static constexpr auto Type = EVENT_GROUP_EVENT;

    CEventGroupEvent(CPed* ped, CEvent* event);
    ~CEventGroupEvent() override;

    eEventType GetEventType() const override { return EVENT_GROUP_EVENT; }
    int32      GetEventPriority() const override { return 41; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventGroupEvent(m_Ped, m_GroupEvent->Clone()); } // 0x4B6EE0
    bool       AffectsPed(CPed* ped) override { return false; }
    bool       AffectsPedGroup(CPedGroup* pedGroup) override { return pedGroup->GetMembership().IsMember(m_Ped); }
    float      GetLocalSoundLevel() override { return 100.0f; }

    bool BaseEventTakesPriorityOverBaseEvent(const CEventGroupEvent& other);

    CPed* GetPed() const { return m_Ped; }
    auto& GetGroupEvent() const { return *m_GroupEvent; }

public:
    static void InjectHooks();

private:
    bool IsPriorityEvent() const;

private:
    CPed::Ref m_Ped{};
    CEvent*   m_GroupEvent{};
};

VALIDATE_SIZE(CEventGroupEvent, 0x14);
