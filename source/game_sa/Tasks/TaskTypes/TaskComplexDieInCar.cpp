#include "StdInc.h"

#include "TaskComplexDieInCar.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskSimpleDieInCar.h"
#include "CarCtrl.h"

void CTaskComplexDieInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDieInCar, 0x86DE04, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(ControlSubTask, 0x6377B0);
    RH_ScopedInstall(CreateSubTask, 0x62FD50);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6375F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x637850);
    RH_ScopedVMTInstall(MakeAbortable, 0x62FCC0);
    RH_ScopedInstall(PreparePedVehicleForPedDeath, 0x62FD00);
}

// 0x62FC80
CTaskComplexDieInCar::CTaskComplexDieInCar(eWeaponType weaponType) : CTaskComplex() {
    m_nWeaponType       = weaponType;
    m_nTimeMS           = 0;
    m_nOffset           = 0;
    m_bPreparedForDeath = false;
    m_bUpdateTime       = false;
}

// 0x6377B0


CTask* CTaskComplexDieInCar::ControlSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_CAR_DRIVE || !m_bPreparedForDeath)
        return m_pSubTask;

    if (m_bUpdateTime) {
        m_nTimeMS = CTimer::GetTimeInMS();
        m_bUpdateTime = false;
    }

    if (CTimer::GetTimeInMS() < m_nTimeMS + m_nOffset)
        return m_pSubTask;

    if (ped->m_nCreatedBy == PED_MISSION || ped->m_pVehicle->CanPedStepOutCar(false))
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR_AND_DIE, ped);

    return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);
}

// 0x62FD50
CTask* CTaskComplexDieInCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
        return new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0);
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(ped->m_pVehicle, nullptr, false);
    case TASK_SIMPLE_DIE_IN_CAR:
        return new CTaskSimpleDieInCar(ANIM_GROUP_DEFAULT, ped->m_pVehicle->IsDriver(ped) ? ANIM_ID_CAR_DEAD_LHS : ANIM_ID_CAR_DEAD_RHS);
    default:
        return nullptr;
    }
}

// 0x6375F0


CTask* CTaskComplexDieInCar::CreateFirstSubTask(CPed* ped) {
    assert(ped);
    ped->SetPedState(PEDSTATE_DIE);

    auto currentEvent = ped->GetEventHandlerHistory().GetCurrentEvent();
    if (currentEvent) {
        if (currentEvent->GetEventType() == EVENT_DAMAGE) {
            if (const auto driver = ped->m_pVehicle->m_pDriver) {
                if (driver != ped) {
                    auto* event = static_cast<CEventDamage*>(currentEvent->Clone());
                    event->m_bAddToEventGroup = false;
                    driver->GetEventGroup().Add(event, false);
                    delete event;
                }
            }

            for (const auto passenger : ped->m_pVehicle->GetPassengers()) {
                if (!passenger)
                    continue;

                if (passenger != ped) {
                    auto* event = static_cast<CEventDamage*>(currentEvent->Clone());
                    event->m_bAddToEventGroup = false;
                    passenger->GetEventGroup().Add(event, false);
                    delete event;
                }
            }
        }
    }

    if (ped->bForceDieInCar)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (m_nWeaponType == WEAPON_EXPLOSION)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (ped->m_pVehicle->IsSubPlane() || ped->m_pVehicle->IsSubHeli())
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    if (ped->m_pVehicle->CanPedStepOutCar(false))
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR_AND_DIE, ped);

    if (ped->m_pVehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE || ped->m_pVehicle->m_pDriver != ped)
        return CreateSubTask(TASK_SIMPLE_DIE_IN_CAR, ped);

    PreparePedVehicleForPedDeath(ped->m_pVehicle);
    m_nTimeMS = CTimer::GetTimeInMS();
    m_nOffset = 2000;
    m_bPreparedForDeath = true;

    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
}

// 0x62FD00
void CTaskComplexDieInCar::PreparePedVehicleForPedDeath(CVehicle *vehicle) {
    if (vehicle->m_nStatus == STATUS_SIMPLE) {
        CCarCtrl::SwitchVehicleToRealPhysics(vehicle);
    }
    vehicle->m_autoPilot.m_nCruiseSpeed    = 0;
    vehicle->m_autoPilot.m_nCarMission     = eCarMission::MISSION_NONE;
    vehicle->m_autoPilot.m_nTempAction     = 6;
    vehicle->m_autoPilot.m_nTempActionTime = CTimer::GetTimeInMS() + 2000;
}

// 0x637850


CTask* CTaskComplexDieInCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_DIE_IN_CAR:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_LEAVE_CAR_AND_DIE:
        return CreateSubTask(ped->m_pVehicle && ped->bInVehicle ? TASK_SIMPLE_DIE_IN_CAR : TASK_FINISHED, ped);
    default:
        return nullptr;
    }
}

// 0x62FCC0

// 0x0
bool CTaskComplexDieInCar::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    switch (priority) {
    case ABORT_PRIORITY_URGENT:
    case ABORT_PRIORITY_IMMEDIATE:
        return m_pSubTask->MakeAbortable(ped, priority, event);
    default:
        return false;
    }
}