#pragma once

#include "TaskSimple.h"
#include "TaskTimer.h"

class NOTSA_EXPORT_VTABLE CTaskSimplePause : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_PAUSE;

    explicit CTaskSimplePause(int32 time = 0);
    CTaskSimplePause(const CTaskSimplePause&);
    ~CTaskSimplePause() = default;

    CTask* Clone() const override { return new CTaskSimplePause{*this}; }
    eTaskType GetTaskType() const override { return TASK_SIMPLE_PAUSE; };
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskSimplePause* Constructor(int32 time);

private:
    CTaskTimer m_Timer{};
    int32      m_PauseIntervalMs{};
};

VALIDATE_SIZE(CTaskSimplePause, 0x18);
