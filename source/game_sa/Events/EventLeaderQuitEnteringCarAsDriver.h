#pragma once

#include "Event.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderQuitEnteringCarAsDriver : public CEvent {
public:
    static void InjectHooks();

    CEventLeaderQuitEnteringCarAsDriver() = default;
    ~CEventLeaderQuitEnteringCarAsDriver() override = default;

    eEventType GetEventType() const override { return EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER; }
    int32 GetEventPriority() const override { return 48; }
    int32 GetLifeTime() override { return 0; }
    CEventLeaderQuitEnteringCarAsDriver* Clone() override { return new CEventLeaderQuitEnteringCarAsDriver(); }
    bool AffectsPed(CPed* ped) override { return false; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }

private:
    CEventLeaderQuitEnteringCarAsDriver* Constructor();
};
VALIDATE_SIZE(CEventLeaderQuitEnteringCarAsDriver, 0xC);
