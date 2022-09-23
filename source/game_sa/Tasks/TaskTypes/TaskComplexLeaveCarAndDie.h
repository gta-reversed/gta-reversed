#pragma once

#include "TaskComplexLeaveCar.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndDie : public CTaskComplexLeaveCar {
 
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_DIE;

    CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie&);

    CTask*    Clone() override { return new CTaskComplexLeaveCarAndDie{ *this }; }
    eTaskType GetTaskType() override { return Type; }
    CTask*    CreateFirstSubTask(CPed * ped) override;
};
