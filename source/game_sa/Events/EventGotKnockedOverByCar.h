#pragma once

#include "EventEditableResponse.h"
#include "Vehicle.h"

class NOTSA_EXPORT_VTABLE CEventGotKnockedOverByCar : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;

public:
    CEventGotKnockedOverByCar(CVehicle* vehicle);
    ~CEventGotKnockedOverByCar() override;

    eEventType GetEventType() const override { return EVENT_GOT_KNOCKED_OVER_BY_CAR; }
    int32 GetEventPriority() const override { return 37; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_vehicle ? m_vehicle->m_pDriver : nullptr; }
    float GetLocalSoundLevel() override { return 55.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override;
};

VALIDATE_SIZE(CEventGotKnockedOverByCar, 0x18);
