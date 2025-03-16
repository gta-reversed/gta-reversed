#include "StdInc.h"

#include "TaskComplexUseWaterCannon.h"

#include "TaskSimpleCarDrive.h"
#include "TaskSimpleCarDriveTimed.h"

void CTaskComplexUseWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexUseWaterCannon, 0x86F5D0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x659780);
    //VTABLE
    RH_ScopedVMTInstall(Clone, 0x659D10);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6597D0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6597C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x65A640);
}

CTaskComplexUseWaterCannon* CTaskComplexUseWaterCannon::Constructor(CFire* pFire)
{
    this->CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(pFire);
    return this;
}

// 0x659780
CTaskComplexUseWaterCannon::CTaskComplexUseWaterCannon(CFire* pFire)
{
    m_pFire = pFire;
}

// 0x65A620
CTaskComplexUseWaterCannon::~CTaskComplexUseWaterCannon()
{
}

// 0x659D10
CTask* CTaskComplexUseWaterCannon::Clone() const {
    return new CTaskComplexUseWaterCannon(m_pFire);
}

// 0x6597C0
CTask* CTaskComplexUseWaterCannon::CreateNextSubTask(CPed* ped)
{
    return nullptr;
}

// 0x6597D0
CTask* CTaskComplexUseWaterCannon::CreateFirstSubTask(CPed* ped)
{
    return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
}

// 0x65A640
CTask* CTaskComplexUseWaterCannon::ControlSubTask(CPed* ped)
{
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_CAR_DRIVE)
    {
        if (ped->m_pVehicle->m_nStatus == STATUS_PHYSICS && m_pFire->IsActive())
            ped->m_pVehicle->AsAutomobile()->FireTruckControl(m_pFire);
        else
            return new CTaskSimpleCarDriveTimed(ped->m_pVehicle, 0);
    }

    return m_pSubTask;
}