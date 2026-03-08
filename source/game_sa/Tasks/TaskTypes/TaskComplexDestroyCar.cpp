#include "StdInc.h"

#include "TaskComplexDestroyCar.h"
#include "TaskComplexDestroyCarArmed.h"
#include "TaskComplexDestroyCarMelee.h"
#include "TaskComplexLeaveCar.h"

void CTaskComplexDestroyCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDestroyCar, 0x86d968, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x621C00);
    RH_ScopedInstall(Destructor, 0x621CB0);

    RH_ScopedInstall(CreateSubTask, 0x6287A0);

    RH_ScopedVMTInstall(Clone, 0x623530);
    RH_ScopedVMTInstall(GetTaskType, 0x621C70);
    RH_ScopedVMTInstall(MakeAbortable, 0x621C80);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62D9E0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62DA90);
    RH_ScopedVMTInstall(ControlSubTask, 0x6288C0);
}

// 0x621C00
CTaskComplexDestroyCar::CTaskComplexDestroyCar(CVehicle* vehicleToDestroy, uint32 unused, uint32 unused2, uint32 unused3) :
      m_VehicleToDestroy{ vehicleToDestroy },
      m_unused{ unused },
      m_unused2{ unused2 },
      m_unused3{ unused3 }
{
    CEntity::SafeRegisterRef(m_VehicleToDestroy);
}

// 0x621CB0
CTaskComplexDestroyCar::~CTaskComplexDestroyCar() {
    CEntity::SafeCleanUpRef(m_VehicleToDestroy);
}

// 0x621C80
bool CTaskComplexDestroyCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}

// 0x62DA90
CTask* CTaskComplexDestroyCar::CreateFirstSubTask(CPed* ped) {
    m_arg0 = false;

    if (!m_VehicleToDestroy)
        return CreateSubTask(TASK_FINISHED);

    if (ped->m_pVehicle && ped->bInVehicle)
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);

    if (ped->GetActiveWeapon().IsTypeMelee())
        return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_MELEE);

    return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_ARMED);
}

// 0x62D9E0
CTask* CTaskComplexDestroyCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_BE_HIT:
    case TASK_COMPLEX_FALL_AND_GET_UP:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_CAR:
    case TASK_COMPLEX_DRAG_PED_FROM_CAR:
        return CreateSubTask(ped->GetActiveWeapon().IsTypeMelee() ? TASK_COMPLEX_DESTROY_CAR_MELEE : TASK_COMPLEX_DESTROY_CAR_ARMED, ped);
    default:
        return nullptr;
    }
}

// 0x6287A0
CTask* CTaskComplexDestroyCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_DESTROY_CAR_ARMED:
        return new CTaskComplexDestroyCarArmed(m_VehicleToDestroy, m_unused, m_unused2, m_unused3);
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false);
    case TASK_COMPLEX_DESTROY_CAR_MELEE:
        return new CTaskComplexDestroyCarMelee(m_VehicleToDestroy);
    default:
        return nullptr;
    }
}

// 0x6288C0
CTask* CTaskComplexDestroyCar::ControlSubTask(CPed* ped) {
    if (!m_VehicleToDestroy || m_VehicleToDestroy->m_fHealth <= 0.f) {
        return nullptr;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_DESTROY_CAR_ARMED: {
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
        if (ped->GetActiveWeapon().IsTypeMelee()) {
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_MELEE, ped);
        }
        break;
    }
    case TASK_COMPLEX_DESTROY_CAR_MELEE: {
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
        if (!ped->GetActiveWeapon().IsTypeMelee()) {
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR_ARMED, ped);
        }
        break;
    }
    case TASK_COMPLEX_LEAVE_CAR: {
        if (!ped->bInVehicle) {
            return CreateSubTask(ped->GetActiveWeapon().IsTypeMelee() ? TASK_COMPLEX_DESTROY_CAR_MELEE : TASK_COMPLEX_DESTROY_CAR_ARMED, ped);
        }
        break;
    }
    }

    return m_pSubTask;
}
