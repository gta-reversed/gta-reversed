#pragma once

#include "Event.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"

class NOTSA_EXPORT_VTABLE CEventAreaCodes : public CEvent {
public:
    CPed* m_ped;

    static void InjectHooks();

    CEventAreaCodes(CPed* ped);
    ~CEventAreaCodes() override;

    eEventType GetEventType() const override { return EVENT_AREA_CODES; }
    int32 GetEventPriority() const override { return 55; }
    int32 GetLifeTime() override { return 0; }
    CEventAreaCodes* Clone() override { return new CEventAreaCodes(m_ped); }
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;

private:
    CEventAreaCodes* Constructor(CPed* ped);
};
VALIDATE_SIZE(CEventAreaCodes, 0x10);
