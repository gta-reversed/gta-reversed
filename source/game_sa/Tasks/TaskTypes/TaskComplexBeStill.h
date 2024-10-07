#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexBeStill : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_BE_STILL;

    CTaskComplexBeStill() = default;
    ~CTaskComplexBeStill() override = default; // 0x5F6700

    eTaskType GetTaskType() const override { return Type; } // 0x5F66D0
    CTask* Clone() const override { return new CTaskComplexBeStill(); } // 0x5F6680

    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
};
