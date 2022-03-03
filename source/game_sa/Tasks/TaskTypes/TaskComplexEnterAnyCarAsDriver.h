#pragma once

#include "TaskComplex.h"

class CTaskComplexEnterAnyCarAsDriver : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER;

    static void InjectHooks();

    ~CTaskComplexEnterAnyCarAsDriver() = default;

    CTask* Clone() override { return new CTaskComplexEnterAnyCarAsDriver{}; }
    eTaskType GetTaskType() override { return Type; };
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    CTaskComplexEnterAnyCarAsDriver* Destructor() {
        this->CTaskComplexEnterAnyCarAsDriver::~CTaskComplexEnterAnyCarAsDriver();
        return this;
    }

    CTask* Clone_Reversed() { return CTaskComplexEnterAnyCarAsDriver::Clone(); }
    eTaskType GetTaskType_Reversed() { return CTaskComplexEnterAnyCarAsDriver::GetTaskType(); }
    CTask* CreateNextSubTask_Reversed(CPed * ped) { return CTaskComplexEnterAnyCarAsDriver::CreateNextSubTask(ped); }
    CTask* CreateFirstSubTask_Reversed(CPed * ped) { return CTaskComplexEnterAnyCarAsDriver::CreateFirstSubTask(ped); }
    CTask* ControlSubTask_Reversed(CPed * ped) { return CTaskComplexEnterAnyCarAsDriver::ControlSubTask(ped); }
};
