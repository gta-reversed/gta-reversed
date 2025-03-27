#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventOnFire : public CEvent {
public:
    CEventOnFire() = default;
    ~CEventOnFire() override = default;

    eEventType GetEventType() const override { return EVENT_ON_FIRE; }
    int32 GetEventPriority() const override { return 66; }
    int32 GetLifeTime() override { return 0; }
    CEventOnFire* Clone() override { return new CEventOnFire(); }
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override { return 60.0f; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
VALIDATE_SIZE(CEventOnFire, 0xC);
