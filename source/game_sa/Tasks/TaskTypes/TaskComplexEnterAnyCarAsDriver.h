#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexEnterAnyCarAsDriver : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER;

    CTaskComplexEnterAnyCarAsDriver() = default;
    ~CTaskComplexEnterAnyCarAsDriver() = default;

    CTask* Clone() const override { return new CTaskComplexEnterAnyCarAsDriver{}; }
    eTaskType GetTaskType() const override { return Type; }
    CTask* CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override { return m_pSubTask; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexEnterAnyCarAsDriver* Destructor() { this->CTaskComplexEnterAnyCarAsDriver::~CTaskComplexEnterAnyCarAsDriver(); return this; }
};
