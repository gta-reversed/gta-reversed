#pragma once

#include "Event.h"

#include "Event.h"

class NOTSA_EXPORT_VTABLE CEventHitByWaterCannon : public CEvent {
public:
    CVector m_point;
    CVector m_moveSpeed;

public:
    CEventHitByWaterCannon(const CVector& point, const CVector& moveSpeed);
    ~CEventHitByWaterCannon() override = default;

    eEventType GetEventType() const override { return EVENT_WATER_CANNON; }
    int32 GetEventPriority() const override { return 64; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override { return new CEventHitByWaterCannon(m_point, m_moveSpeed); }
    bool AffectsPed(CPed* ped) override;
    float GetLocalSoundLevel() override { return 55.0f; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventHitByWaterCannon* Constructor(const CVector& point, const CVector& moveSpeed);

};
VALIDATE_SIZE(CEventHitByWaterCannon, 0x24);
