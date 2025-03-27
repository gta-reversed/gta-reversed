#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderEnteredCarAsDriver : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;

public:
    static void InjectHooks();

    CEventLeaderEnteredCarAsDriver(CVehicle* vehicle);
    ~CEventLeaderEnteredCarAsDriver() override;

    eEventType GetEventType() const override { return EVENT_LEADER_ENTERED_CAR_AS_DRIVER; }
    int32 GetEventPriority() const override  { return 46; }
    int32 GetLifeTime() override  { return 0; }
    bool AffectsPed(CPed* ped) override  { return false; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override  { return true; }
    bool CanBeInterruptedBySameEvent() override  { return true; }
    CEventLeaderEnteredCarAsDriver* CloneEditable() override  { return new CEventLeaderEnteredCarAsDriver(m_vehicle); }

private:
    CEventLeaderEnteredCarAsDriver* Constructor(CVehicle* vehicle);

};
VALIDATE_SIZE(CEventLeaderEnteredCarAsDriver, 0x18);
