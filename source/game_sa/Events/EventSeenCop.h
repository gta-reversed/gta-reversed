#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventSeenCop : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override { return EVENT_SEEN_COP; }
    int32 GetEventPriority() const override { return 21; }
    bool AffectsPed(CPed* ped) override { return CEventAcquaintancePed::AffectsPed(ped); }
    CEventSeenCop* CloneEditable() override { return new CEventSeenCop(m_AcquaintancePed); }

public:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventSeenCop* Constructor1(CPed* cop);
};
VALIDATE_SIZE(CEventSeenCop, 0x18);
