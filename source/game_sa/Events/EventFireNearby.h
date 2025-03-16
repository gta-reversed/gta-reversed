#pragma once

#include "EventEditableResponse.h"


class NOTSA_EXPORT_VTABLE CEventFireNearby : public CEventEditableResponse {
public:
    CVector m_position;

public:
    explicit CEventFireNearby(const CVector& position);
    ~CEventFireNearby() override = default; // 0x4B1F80

    eEventType GetEventType() const override { return EVENT_FIRE_NEARBY; }
    int32 GetEventPriority() const override { return 17; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refevent) override { return true; }
    CEventFireNearby* CloneEditable() override { return new CEventFireNearby(m_position); }
};
VALIDATE_SIZE(CEventFireNearby, 0x20);
