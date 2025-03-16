#pragma once

#include "EventEditableResponse.h"

class CEntity;

class NOTSA_EXPORT_VTABLE CEventDanger : public CEventEditableResponse {
public:
    CEntity* m_dangerFrom;
    float m_dangerRadius;

public:
    CEventDanger(CEntity* dangerFrom, float dangerRadius);
    ~CEventDanger() override;

    eEventType GetEventType() const override { return EVENT_DANGER; }
    int32 GetEventPriority() const override { return 20; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    CEntity* GetSourceEntity() const override;
    CEventDanger* CloneEditable() override { return new CEventDanger(m_dangerFrom, m_dangerRadius); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CEventDanger* Constructor(CEntity* dangerFrom, float dangerRadius);
};
VALIDATE_SIZE(CEventDanger, 0x1C);
