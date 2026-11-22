#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventInWater : public CEvent {
public:
    CEventInWater(float acceleration);
    ~CEventInWater() override = default;

    eEventType GetEventType() const override { return EVENT_IN_WATER; }
    int32      GetEventPriority() const override { return 62; }
    int32      GetLifeTime() override { return 0; }
    CEvent*    Clone() const noexcept override { return new CEventInWater(m_acceleration); }
    bool       AffectsPed(CPed* ped) override;
    bool       TakesPriorityOver(const CEvent& refEvent) override;

public:
    static void InjectHooks();

private:
    float m_acceleration{};
};
VALIDATE_SIZE(CEventInWater, 0x10);
