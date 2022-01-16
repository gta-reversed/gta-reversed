#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class CTaskSimplePause : public CTaskSimple {
public:
    CTaskTimer m_timer;
    int32 m_nTime;

public:
    CTaskSimplePause(int32 time);
    ~CTaskSimplePause();

    CTask* Clone() override;
    eTaskType GetTaskType() override {
        return TASK_SIMPLE_PAUSE;
    };
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    bool MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event);
    bool ProcessPed(CPed* ped) override;
    bool ProcessPed_Reversed(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePause* Constructor(int32 time);

};

VALIDATE_SIZE(CTaskSimplePause, 0x18);
