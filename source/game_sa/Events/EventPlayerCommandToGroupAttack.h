#pragma once

#include "EventPlayerCommandToGroup.h"
#include "EventEditableResponse.h"

class NOTSA_EXPORT_VTABLE CEventPlayerCommandToGroupAttack : public CEventPlayerCommandToGroup {
public:
    static void InjectHooks();

    CEventPlayerCommandToGroupAttack(CPed* target) : CEventPlayerCommandToGroup(PLAYER_GROUP_COMMAND_ATTACK, target) {}
    ~CEventPlayerCommandToGroupAttack() override = default;

    eEventType GetEventType() const override { return EVENT_PLAYER_COMMAND_TO_GROUP; }
    int32 GetEventPriority() const override { return 44; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    CEventPlayerCommandToGroupAttack* CloneEditable() override { return new CEventPlayerCommandToGroupAttack(m_target); }

private:
    CEventPlayerCommandToGroupAttack* Constructor(CPed* target);
};
VALIDATE_SIZE(CEventPlayerCommandToGroupAttack, 0x1C);
