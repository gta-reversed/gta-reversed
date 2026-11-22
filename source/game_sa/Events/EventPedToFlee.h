#pragma once

#include "Event.h"
#include "Ped.h"

class NOTSA_EXPORT_VTABLE CEventPedToFlee : public CEvent {
public:
    CEventPedToFlee(CPed* ped);
    ~CEventPedToFlee() override = default;

    eEventType GetEventType() const override { return EVENT_PED_TO_FLEE; }
    int32      GetEventPriority() const override { return 2; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventPedToFlee(m_PedToFlee); } // 0x4B73D0
    bool       AffectsPed(CPed* ped) override { return ped->IsAlive(); }

    CPed* GetPedToFlee() const { return m_PedToFlee; }

public:
    static void InjectHooks();

private:
    CPed::Ref m_PedToFlee{};
};
VALIDATE_SIZE(CEventPedToFlee, 0x10);
