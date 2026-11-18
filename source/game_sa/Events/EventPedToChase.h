#pragma once

#include "Event.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventPedToChase : public CEvent {
public:
    CEventPedToChase(CPed* pedToChase);
    ~CEventPedToChase() override = default;

    eEventType GetEventType() const override { return EVENT_PED_TO_CHASE; }
    int32      GetEventPriority() const override { return 1; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventPedToChase(m_PedToChase); }
    bool       AffectsPed(CPed* ped) override { return ped->IsAlive(); }

    CPed* GetPedToChase() const { return m_PedToChase; }

public:
    static void InjectHooks();

private:
    CPed::Ref m_PedToChase{};
};
VALIDATE_SIZE(CEventPedToChase, 0x10);
