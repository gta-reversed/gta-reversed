#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexDriveFireTruck : public CTaskComplex {
public:
    CVehicle* m_pVehicle;
    uint8     m_bIsDriver;
    CPed*     m_pPartnerFireman;
    CFire*    m_pFire;

public:
    static constexpr auto Type = TASK_COMPLEX_DRIVE_FIRE_TRUCK;

    CTaskComplexDriveFireTruck(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver);
    ~CTaskComplexDriveFireTruck() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;


    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexDriveFireTruck* Constructor(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver);
};
VALIDATE_SIZE(CTaskComplexDriveFireTruck, 0x1C);
