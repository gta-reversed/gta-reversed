#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedHateBadlyLit : public CEventAcquaintancePed {
public:
    uint32 m_startTimeInMs;
    CVector m_point;

public:
    CEventAcquaintancePedHateBadlyLit(CPed* ped, int32 startTimeInMs, const CVector& point);
    ~CEventAcquaintancePedHateBadlyLit() override = default;

    eEventType GetEventType() const override { return EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT; }
    int32 GetEventPriority() const override { return 25; }
    bool AffectsPed(CPed* ped) override;
    bool CanBeInterruptedBySameEvent() override { return true; }
    CEventEditableResponse* CloneEditable() override { return new CEventAcquaintancePedHateBadlyLit(m_AcquaintancePed, m_startTimeInMs, m_point); }

public:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventAcquaintancePedHateBadlyLit* Constructor(CPed* ped, int32 startTimeInMs, const CVector& point);
};
VALIDATE_SIZE(CEventAcquaintancePedHateBadlyLit, 0x28);
