#pragma once

#include "EventPlayerCommandToGroup.h"

#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPlayerCommandToGroupGather : public CEventPlayerCommandToGroup {
public:
    static void InjectHooks();

    CEventPlayerCommandToGroupGather(CPed* target) : CEventPlayerCommandToGroup(PLAYER_GROUP_COMMAND_GATHER, target) {}
    ~CEventPlayerCommandToGroupGather() override = default;

    eEventType GetEventType() const override { return EVENT_PLAYER_COMMAND_TO_GROUP_GATHER; }
    int32 GetEventPriority() const override { return 45; }
    CEventPlayerCommandToGroupGather* CloneEditable() override { return new CEventPlayerCommandToGroupGather(m_target); }

    CEventPlayerCommandToGroupGather* Constructor(CPed* target);
};
VALIDATE_SIZE(CEventPlayerCommandToGroupGather, 0x1C);
