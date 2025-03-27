#pragma once

#include "EventAcquaintancePed.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "EventAcquaintancePed.h"


class NOTSA_EXPORT_VTABLE CEventAcquaintancePedRespect : public CEventAcquaintancePed {
public:
    using CEventAcquaintancePed::CEventAcquaintancePed;

    eEventType GetEventType() const override { return EVENT_ACQUAINTANCE_PED_RESPECT; }
    int32 GetEventPriority() const override { return 23; }
    CEventEditableResponse* CloneEditable() override { return new CEventAcquaintancePedRespect(m_AcquaintancePed); }
};
VALIDATE_SIZE(CEventAcquaintancePedRespect, 0x18);
