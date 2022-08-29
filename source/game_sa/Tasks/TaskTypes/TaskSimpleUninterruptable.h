#pragma once

#include "TaskSimple.h"

class NOTSA_EXPORT_VTABLE CTaskSimpleUninterruptable : public CTaskSimple {
public:
    static constexpr auto Type = TASK_SIMPLE_UNINTERRUPTABLE;

    CTaskSimpleUninterruptable() = default;
    ~CTaskSimpleUninterruptable() override = default;

    eTaskType GetTaskType() override { return Type; }
    CTask* Clone() override { return new CTaskSimpleUninterruptable(); }
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override { return priority == ABORT_PRIORITY_IMMEDIATE; }
    bool ProcessPed(CPed* ped) override { return false; }

    static void InjectHooks();
    CTaskSimpleUninterruptable* Constructor();
};
VALIDATE_SIZE(CTaskSimpleUninterruptable, 0x8);
