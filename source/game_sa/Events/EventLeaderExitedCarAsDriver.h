#pragma once

#include "EventEditableResponse.h"
#include "PedGroup.h"

#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"

class NOTSA_EXPORT_VTABLE CEventLeaderExitedCarAsDriver : public CEventEditableResponse {
public:
    static void InjectHooks();

    CEventLeaderExitedCarAsDriver() = default;
    ~CEventLeaderExitedCarAsDriver() override = default;

    eEventType GetEventType() const override { return EVENT_LEADER_EXITED_CAR_AS_DRIVER; }
    int32 GetEventPriority() const override { return 47; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return false; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    CEventLeaderExitedCarAsDriver* CloneEditable() override { return new CEventLeaderExitedCarAsDriver(); }

private:
    CEventLeaderExitedCarAsDriver* Constructor();
};
VALIDATE_SIZE(CEventLeaderExitedCarAsDriver, 0x14);
