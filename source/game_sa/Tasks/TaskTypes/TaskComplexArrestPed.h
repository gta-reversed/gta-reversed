#pragma once

#include "TaskComplex.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexArrestPed : public CTaskComplex {
public:
    bool      m_bSubTaskNeedsToBeCreated;
    CPed*     m_PedToArrest;
    int32     field_14;
    int32     field_18;
    int32     field_1C;
    CVehicle* m_Vehicle;

public:
    static constexpr auto Type = TASK_COMPLEX_ARREST_PED;

    explicit CTaskComplexArrestPed(CPed* ped);
    ~CTaskComplexArrestPed() override;

    CTask* Clone() const override { return new CTaskComplexArrestPed(m_PedToArrest); }
    eTaskType GetTaskType() const override { return Type; }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexArrestPed* Constructor(CPed* ped) { this->CTaskComplexArrestPed::CTaskComplexArrestPed(ped); return this; }
    CTaskComplexArrestPed* Destructor() { this->CTaskComplexArrestPed::~CTaskComplexArrestPed(); return this; }
};
VALIDATE_SIZE(CTaskComplexArrestPed, 0x24);
