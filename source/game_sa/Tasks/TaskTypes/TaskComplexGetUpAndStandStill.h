#pragma once

#include "TaskComplex.h"

class CTaskComplexGetUpAndStandStill : public CTaskComplex {
public:
    CTaskComplexGetUpAndStandStill() = default;
    ~CTaskComplexGetUpAndStandStill() override = default;

    CTask* Clone() override { return new CTaskComplexGetUpAndStandStill(); }
    eTaskType GetTaskType() override { return TASK_COMPLEX_GET_UP_AND_STAND_STILL; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexGetUpAndStandStill* Constructor();

    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* CreateNextSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);
};

VALIDATE_SIZE(CTaskComplexGetUpAndStandStill, 0xC);
