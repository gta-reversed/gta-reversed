#include "StdInc.h"

#include "TaskComplexDriveFireTruck.h"

#include "TaskComplexDriveToPoint.h"
#include "TaskComplexDriveWander.h"
#include "TaskComplexUseWaterCannon.h"
#include "TaskSimpleCarDrive.h"
#include "FireManager.h"

void CTaskComplexDriveFireTruck::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDriveFireTruck, 0x86F570, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x659310);
    RH_ScopedInstall(CreateSubTask, 0x65A240);
    RH_ScopedVMTInstall(Clone, 0x659BC0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65B140);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65B090);
    RH_ScopedVMTInstall(ControlSubTask, 0x65B1E0);
}

CTaskComplexDriveFireTruck* CTaskComplexDriveFireTruck::Constructor(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver) {
    this->CTaskComplexDriveFireTruck::CTaskComplexDriveFireTruck(vehicle, partnerFireman, bIsDriver);
    return this;
}

// 0x659310
CTaskComplexDriveFireTruck::CTaskComplexDriveFireTruck(CVehicle* vehicle, CPed* partnerFireman, bool bIsDriver) : CTaskComplex() {
    m_pVehicle        = vehicle;
    m_pPartnerFireman = partnerFireman;
    m_bIsDriver       = bIsDriver;
    m_pFire           = nullptr;

    CEntity::SafeRegisterRef(m_pVehicle);
    CEntity::SafeRegisterRef(m_pPartnerFireman);
}

// 0x6593A0
CTaskComplexDriveFireTruck::~CTaskComplexDriveFireTruck() {
    CEntity::SafeCleanUpRef(m_pVehicle);
    CEntity::SafeCleanUpRef(m_pPartnerFireman);
}

// 0x659BC0
CTask* CTaskComplexDriveFireTruck::Clone() const {
    return new CTaskComplexDriveFireTruck(m_pVehicle, m_pPartnerFireman, m_bIsDriver);
}

// 0x65B090
CTask* CTaskComplexDriveFireTruck::CreateNextSubTask(CPed* ped) {
    eTaskType subTaskType = m_pSubTask->GetTaskType();

    if (subTaskType == TASK_COMPLEX_CAR_DRIVE_WANDER)
        return CreateSubTask(TASK_FINISHED, ped);

    if (subTaskType == TASK_COMPLEX_CAR_DRIVE_TO_POINT)
        return ped->bInVehicle ? CreateSubTask(TASK_COMPLEX_USE_WATER_CANNON, ped) : CreateSubTask(TASK_FINISHED, ped);

    if (subTaskType == TASK_COMPLEX_USE_WATER_CANNON)
        return ped->bInVehicle ? CreateFirstSubTask(ped) : CreateSubTask(TASK_FINISHED, ped);

    if (subTaskType == TASK_SIMPLE_CAR_DRIVE)
        return CreateSubTask(TASK_FINISHED, ped);

    return nullptr;
}

// 0x65B140
CTask* CTaskComplexDriveFireTruck::CreateFirstSubTask(CPed* ped) {
    if (!m_pVehicle || !ped->bInVehicle)
        return CreateSubTask(TASK_FINISHED, ped);

    if (!m_bIsDriver)
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);

    m_pFire = gFireManager.FindNearestFire(m_pVehicle->GetPosition(), true, true);
    return CreateSubTask(m_pFire ? TASK_COMPLEX_CAR_DRIVE_TO_POINT : TASK_COMPLEX_CAR_DRIVE_WANDER, ped);
}

// 0x65B1E0
CTask* CTaskComplexDriveFireTruck::ControlSubTask(CPed* ped) {
    eTaskType subTaskId = m_pSubTask->GetTaskType();

    if (!m_pVehicle || !ped->bInVehicle)
        return CreateFirstSubTask(ped);

    if (subTaskId == TASK_COMPLEX_CAR_DRIVE_TO_POINT) {
        if (m_bIsDriver && m_pFire->IsBeingExtinguished()) {
            CFire* newFire = gFireManager.FindNearestFire(m_pVehicle->GetPosition(), true, true);

            if (!newFire)
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_WANDER, ped);

            if (newFire != m_pFire) {
                m_pFire = newFire;
                reinterpret_cast<CTaskComplexDriveToPoint*>(m_pSubTask)->m_Point = m_pFire->m_vecPosition;
            }
        }
    }

    if (subTaskId == TASK_COMPLEX_CAR_DRIVE_WANDER) {
        if (m_bIsDriver && gFireManager.FindNearestFire(m_pVehicle->GetPosition(), true, true))
            return CreateFirstSubTask(ped);
    }

    return m_pSubTask;
}

// 0x65A240
CTask* CTaskComplexDriveFireTruck::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE_WANDER:
        return new CTaskComplexCarDriveWander(m_pVehicle, DRIVING_STYLE_STOP_FOR_CARS, 10.0F);
    case TASK_COMPLEX_CAR_DRIVE_TO_POINT:
        return new CTaskComplexDriveToPoint(m_pVehicle, m_pFire->m_vecPosition, 30.0F, 0, MODEL_INVALID, 25.0F, DRIVING_STYLE_AVOID_CARS);
    case TASK_COMPLEX_USE_WATER_CANNON:
        return new CTaskComplexUseWaterCannon(m_pFire);
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(m_pVehicle, nullptr, false);
    default:
        return nullptr;
    }
}
