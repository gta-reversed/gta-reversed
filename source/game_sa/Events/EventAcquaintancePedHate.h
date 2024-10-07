#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedHate : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override { return EVENT_ACQUAINTANCE_PED_HATE; }
    int32 GetEventPriority() const override { return 26; }
    CEventEditableResponse* CloneEditable() override { return new CEventAcquaintancePedHate(m_AcquaintancePed); }

public:
    friend class CEventAcquaintancePed;
    static void InjectHooks();

    CEventAcquaintancePedHate* Constructor2(CPed* ped);
};
VALIDATE_SIZE(CEventAcquaintancePedHate, 0x18);
