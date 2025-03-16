#pragma once

#include "EventEditableResponse.h"

enum ePlayerGroupCommand : int32
{
    PLAYER_GROUP_COMMAND_ATTACK = 0,
    PLAYER_GROUP_COMMAND_GATHER = 2,
};

class NOTSA_EXPORT_VTABLE CEventPlayerCommandToGroup : public CEventEditableResponse {
public:
    ePlayerGroupCommand m_command;
    CPed*               m_target;

public:
    static void InjectHooks();

    explicit CEventPlayerCommandToGroup(ePlayerGroupCommand command = ePlayerGroupCommand::PLAYER_GROUP_COMMAND_ATTACK, CPed* target = nullptr);
    ~CEventPlayerCommandToGroup() override;

    eEventType GetEventType() const override { return EVENT_PLAYER_COMMAND_TO_GROUP; }
    int32_t GetEventPriority() const override { return 44; }
    CEventPlayerCommandToGroup* CloneEditable() override { return new CEventPlayerCommandToGroup(ePlayerGroupCommand::PLAYER_GROUP_COMMAND_ATTACK, m_target); }
    int32_t GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override { return false; }
    bool AffectsPedGroup(CPedGroup* pedGroup) override;
    CEntity* GetSourceEntity() const override { return reinterpret_cast<CEntity*>(m_target); }
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }

private:
    CEventPlayerCommandToGroup* Constructor(ePlayerGroupCommand command, CPed* target);
};
VALIDATE_SIZE(CEventPlayerCommandToGroup, 0x1C);
