#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedDislike : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override { return EVENT_ACQUAINTANCE_PED_DISLIKE; }
    int32 GetEventPriority() const override { return 24; }
    CEventEditableResponse* CloneEditable() override { return new CEventAcquaintancePedDislike(m_AcquaintancePed); }
};
VALIDATE_SIZE(CEventAcquaintancePedDislike, 0x18);
