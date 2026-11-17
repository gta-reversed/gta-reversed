#pragma once

#include "Event.h"


class NOTSA_EXPORT_VTABLE CEventRevived : public CEvent {
public:
    static constexpr auto Type = eEventType::EVENT_REVIVE;

    CEventRevived()           = default;
    ~CEventRevived() override = default;

    eEventType     GetEventType() const override { return EVENT_REVIVE; }
    int32          GetEventPriority() const override { return 74; }
    int32          GetLifeTime() override { return 0; }
    CEventRevived* Clone() const noexcept override { return new CEventRevived(); }
    bool           AffectsPed(CPed* ped) override { return !ped->IsCreatedByMission() && !ped->IsAlive(); }

public:
    static void InjectHooks();
};
VALIDATE_SIZE(CEventRevived, 0xC);
