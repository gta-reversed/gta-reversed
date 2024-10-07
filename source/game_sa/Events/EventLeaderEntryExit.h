#pragma once

#include "EventAreaCodes.h"

#include "EventEditableResponse.h"
#include "PedGroup.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexArrestPed.h"


class NOTSA_EXPORT_VTABLE CEventLeaderEntryExit : public CEventAreaCodes {
public:
    static void InjectHooks();
    CEventLeaderEntryExit* Constructor(CPed* ped);

    CEventLeaderEntryExit(CPed* ped) : CEventAreaCodes(ped) {}
    ~CEventLeaderEntryExit() override = default;

    eEventType GetEventType() const override { return EVENT_LEADER_ENTRY_EXIT; }
    int32 GetLifeTime() override { return 0; }
    CEventLeaderEntryExit* Clone() override { return new CEventLeaderEntryExit(m_ped); }
    bool AffectsPedGroup(CPedGroup* pedGroup) override { return m_ped && pedGroup->GetMembership().GetLeader() == m_ped; }
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    bool CanBeInterruptedBySameEvent() override { return true; }
};
VALIDATE_SIZE(CEventLeaderEntryExit, 0x10);
