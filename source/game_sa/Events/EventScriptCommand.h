#pragma once

#include "Event.h"

class CTask;

class NOTSA_EXPORT_VTABLE CEventScriptCommand : public CEvent {
public:
    int32  m_primaryTaskIndex;
    CTask* m_task;
    bool   m_affectsDeadPeds;

public:
    static constexpr auto Type = eEventType::EVENT_SCRIPT_COMMAND;

    CEventScriptCommand(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds = false);
    ~CEventScriptCommand() override;

    eEventType GetEventType() const override { return Type; }
    int32 GetEventPriority() const override;
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override;
    bool IsValid(CPed* ped) override;
    virtual CTask* CloneScriptTask();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventScriptCommand* Constructor(int32 primaryTaskIndex, CTask* task, bool affectsDeadPeds);

};

VALIDATE_SIZE(CEventScriptCommand, 0x18);
