#pragma once

#include "TaskComplexLeaveCar.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndDie : public CTaskComplexLeaveCar {
 
public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_DIE;

    CTaskComplexLeaveCarAndDie(const CTaskComplexLeaveCarAndDie&);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAndDie{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateFirstSubTask(CPed * ped) override;
};
