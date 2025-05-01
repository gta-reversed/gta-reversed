#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexDestroyCar : public CTaskComplex {
public:
    bool      m_arg0{};
    CVehicle* m_VehicleToDestroy{};
    uint32    m_unused{};
    uint32    m_unused2{};
    uint32    m_unused3{};

public:
    static constexpr auto Type = TASK_COMPLEX_DESTROY_CAR;

    CTaskComplexDestroyCar(CVehicle* vehicleToDestroy, uint32 a3 = 0, uint32 a4 = 0, uint32 a5 = 0);
    ~CTaskComplexDestroyCar() override;

    eTaskType GetTaskType() const override { return Type; } // 0x621C70
    CTask* Clone() const override { return new CTaskComplexDestroyCar(m_VehicleToDestroy, m_unused, m_unused2, m_unused3); } // 0x623530
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateSubTask(eTaskType taskType, CPed* ped = nullptr);

    static void InjectHooks();
    CTaskComplexDestroyCar* Constructor(CVehicle* vehicleToDestroy, uint32 a3, uint32 a4, uint32 a5) { this->CTaskComplexDestroyCar::CTaskComplexDestroyCar(vehicleToDestroy, a3, a4, a5); return this; }
    CTaskComplexDestroyCar* Destructor() { this->CTaskComplexDestroyCar::~CTaskComplexDestroyCar(); return this; }
};
VALIDATE_SIZE(CTaskComplexDestroyCar, 0x20);
