#pragma once

#include "TaskComplex.h"
#include "Vehicle.h"


class NOTSA_EXPORT_VTABLE CTaskComplexWaitForBus : public CTaskComplex {
public:

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_BUS;

    CTaskComplexWaitForBus() = default;
    CTaskComplexWaitForBus(const CTaskComplexWaitForBus&);
    ~CTaskComplexWaitForBus() = default;

    CTask*    Clone() const override { return new CTaskComplexWaitForBus{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

public:
    static void InjectHooks();

private:
    CVehicle::Ref m_BusToWaitFor{}; //!< BUGFIX: Original code didn't register references
};
