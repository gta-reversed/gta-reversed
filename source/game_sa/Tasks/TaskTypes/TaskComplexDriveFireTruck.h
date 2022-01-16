#pragma once

#include "TaskComplex.h"

class CTaskComplexDriveFireTruck : public CTaskComplex {
public:
    CVehicle* m_pVehicle;
    uint8     m_bIsDriver;
    uint8     _pad_11[3];
    CPed*     m_pPartnerFireman;
    CFire*    m_pFire;

public:
    CTaskComplexDriveFireTruck(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver);
    ~CTaskComplexDriveFireTruck() override;

    CTask*    Clone() override;
    eTaskType GetTaskType() override { return TASK_COMPLEX_DRIVE_FIRE_TRUCK; }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* Clone_Reversed();
    CTask* CreateNextSubTask_Reversed(CPed* ped);
    CTask* CreateFirstSubTask_Reversed(CPed* ped);
    CTask* ControlSubTask_Reversed(CPed* ped);

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexDriveFireTruck* Constructor(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver);
};

VALIDATE_SIZE(CTaskComplexDriveFireTruck, 0x1C);
