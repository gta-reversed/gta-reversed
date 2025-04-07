#include "StdInc.h"

#include "extensions/utility.hpp"

#include "TaskComplexArrestPed.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleWaitUntilPedIsOutCar.h"
#include "TaskSimpleArrestPed.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexDestroyCar.h"
// #include "TaskComplexSeekEntity.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexOpenDriverDoor.h"
#include "TaskComplexOpenPassengerDoor.h"

#include "eTargetDoor.h"

void CTaskComplexArrestPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexArrestPed, 0x8709A8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68B990);
    RH_ScopedInstall(Destructor, 0x68BA00);
    RH_ScopedVMTInstall(MakeAbortable, 0x68BA60);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x690220, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6907A0);
    RH_ScopedVMTInstall(ControlSubTask, 0x68D350, { .reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x68CF80, { .reversed = false });
}

// 0x68B990
CTaskComplexArrestPed::CTaskComplexArrestPed(CPed* ped) : CTaskComplex() {
    m_PedToArrest = ped;
    m_Vehicle = nullptr;
    CEntity::SafeRegisterRef(m_PedToArrest);
}

// 0x68BA00
CTaskComplexArrestPed::~CTaskComplexArrestPed() {
    CEntity::SafeCleanUpRef(m_PedToArrest);
}

// 0x68BA60


bool CTaskComplexArrestPed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x690220 See #gists in discord


CTask* CTaskComplexArrestPed::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x690220, CTaskComplexArrestPed*, CPed*>(this, ped);
}

// NOTSA
void MakeSurePedHasWeaponInHand(CPed* ped) {
    // Make sure ped has an actual weapon in their hand
    if (!ped->GetActiveWeapon().IsTypeMelee())
        return;

    if (ped->DoWeHaveWeaponAvailable(WEAPON_SHOTGUN)) { // Use shotgun (if available)
        ped->SetCurrentWeapon(WEAPON_SHOTGUN);
        return;
    }

    // Otherwise a pistol
    if (!ped->DoWeHaveWeaponAvailable(WEAPON_PISTOL)) { // Make sure they have one
        ped->GiveWeapon(WEAPON_PISTOL, 10, false);
    }
    ped->SetCurrentWeapon(WEAPON_PISTOL);
}

// 0x6907A0


CTask* CTaskComplexArrestPed::CreateFirstSubTask(CPed* ped) {
    if (!m_PedToArrest) {
        return nullptr;
    }

    m_bSubTaskNeedsToBeCreated = false;

    if (!m_PedToArrest->bInVehicle) {
        return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
    }

    if (m_PedToArrest->m_pVehicle->IsBike() || m_PedToArrest->m_pVehicle->IsSubQuad()) { // Just drag ped from a bike/quad
        return CreateSubTask(TASK_COMPLEX_DRAG_PED_FROM_CAR, ped);
    }

    if (m_PedToArrest->m_pVehicle->IsSubBoat()) { // If they're in a boat, just destroy it
        MakeSurePedHasWeaponInHand(ped);
        return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
    } else {
        if (m_PedToArrest->m_pVehicle->IsUpsideDown() || m_PedToArrest->m_pVehicle->IsOnItsSide()) {
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        return CreateSubTask(TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR, ped);
    }
}

// 0x68D350

// 0x0
CTask* CTaskComplexArrestPed::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68D350, CTaskComplexArrestPed*, CPed*>(this, ped);

    // Automatically make ped say something on function return
    const notsa::ScopeGuard Have_A_Nice_Day_Sir{
        [this, ped] {
            if (m_PedToArrest && m_PedToArrest->IsPlayer()) {
                if (FindPlayerWanted()->m_nCopsInPursuit == 1) {
                    ped->Say(CTX_GLOBAL_SOLO);
                }
            }
        }
    };

    // Tries to abort current sub-task and replace it with `taskType`.
    const auto TryReplaceSubTask = [this, ped](auto taskType) {
        // Inverted `if` and got rid of `taskType == TASK_NONE` (in which case `m_pSubTask` was returned always)
        if (m_pSubTask->MakeAbortable(ped)) {
            return CreateSubTask(taskType, ped);
        } else {
            return m_pSubTask;
        }
    };

    const auto DoDestroyCarTask = [&] {
        MakeSurePedHasWeaponInHand(ped);
        return TryReplaceSubTask(TASK_COMPLEX_DESTROY_CAR);
    };

    // 0x68D370, 0x68D37F
    if (!m_PedToArrest || m_PedToArrest->m_fHealth <= 0.f) {
        return TryReplaceSubTask(TASK_FINISHED);
    }

    // 0x68D39F
    if (m_bSubTaskNeedsToBeCreated) {
        if (m_pSubTask->MakeAbortable(ped)) {
            m_pSubTask->AsComplex()->CreateFirstSubTask(ped);
        }
        return m_pSubTask;
    }

    // 0x68D3CB
    if (m_PedToArrest->bIsBeingArrested) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_SIMPLE_ARREST_PED:
        case TASK_COMPLEX_SEEK_ENTITY:
            break;
        default:
            return TryReplaceSubTask(TASK_COMPLEX_SEEK_ENTITY);
        }
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_DRAG_PED_FROM_CAR: // 0x68D49D
    case TASK_COMPLEX_DESTROY_CAR: { // 0x68D5F6
        if (!m_PedToArrest->bInVehicle) { // If not in vehicle anymore, try to kill them
            return TryReplaceSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT);
        }
        break;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x68D626
        // See if ped is falling, and is close enough
        if (const auto task = static_cast<CTaskComplexFallAndGetUp*>(m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_FALL_AND_GET_UP))) {
            if (task->IsFalling()) {
                const auto dir = ped->GetPosition() - m_PedToArrest->GetPosition();
                if (std::abs(dir.z) <= 2.f && dir.SquaredMagnitude() <= sq(3.0f)) {
                    task->SetDownTime(100'000);
                    return TryReplaceSubTask(TASK_SIMPLE_ARREST_PED);
                }
            }

            // Getting up already/not close enough
            return TryReplaceSubTask(TASK_COMPLEX_SEEK_ENTITY);
        }

        if (!m_PedToArrest->bInVehicle || !m_PedToArrest->m_pVehicle) {
            break;
        }

        // Ped has gotten into a vehicle, we need a different task!

        if (   m_PedToArrest->m_pVehicle->IsBoat()
            || m_PedToArrest->m_pVehicle->IsSubPlane()
            || m_PedToArrest->m_pVehicle->IsSubHeli()
        ) {
            return DoDestroyCarTask();
        }

        if (!ped->GetActiveWeapon().IsTypeMelee()) {
            if (!FindPlayerWanted()->IsClosestCop(ped->AsCop(), 2)) {
                return DoDestroyCarTask();
            }
        }

        if (m_PedToArrest->m_pVehicle != m_Vehicle) {
            if (m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                if (m_PedToArrest->m_pVehicle->IsBike() || m_PedToArrest->m_pVehicle->IsSubQuad()) {
                    return TryReplaceSubTask(TASK_COMPLEX_DRAG_PED_FROM_CAR);
                }

                if (!m_PedToArrest->m_pVehicle->IsUpsideDown() && !m_PedToArrest->m_pVehicle->IsOnItsSide()) {
                    return TryReplaceSubTask(TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR);
                }

                return DoDestroyCarTask();
            }
        }

        break;
    }
    case TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR: { // 0x68D424
        if (m_PedToArrest->bInVehicle) {
            // Maybe wait until ped gets out of the car..
            if (const auto task = m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                if (ped->IsEntityInRange(m_PedToArrest, 5.f)) {
                    return TryReplaceSubTask(TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR);
                }
            }

            // Ped can't open driver door, but we can open front right door?
            if (!m_PedToArrest->m_pVehicle->IsRoomForPedToLeaveCar(TARGET_DOOR_DRIVER, nullptr)) {
                if (m_PedToArrest->m_pVehicle->IsRoomForPedToLeaveCar(TARGET_DOOR_FRONT_RIGHT, nullptr)) {
                    return TryReplaceSubTask(TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR);
                }
            }
        }

        // Ped not in vehicle any more vehicle, just kill 'em
        return TryReplaceSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT);
    }
    case TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR: { // 0x68D510
        // Pretty much the copy of the above, with minor changes (See change 1,2)

        if (m_PedToArrest->bInVehicle) {
            // Maybe wait until ped gets out of the car..
            if (const auto task = m_PedToArrest->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                if (ped->IsEntityInRange(m_PedToArrest, 5.f)) {
                    return TryReplaceSubTask(TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR);
                }
            }

            // Ped can't open passenger door, but driver door can be opened by us? - Change 1
            if (!m_PedToArrest->m_pVehicle->IsRoomForPedToLeaveCar(TARGET_DOOR_FRONT_RIGHT, nullptr)) {
                if (m_PedToArrest->m_pVehicle->IsRoomForPedToLeaveCar(TARGET_DOOR_DRIVER, nullptr)) {
                    return TryReplaceSubTask(TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR);
                }
            }
        }

        // Change 2
        return DoDestroyCarTask();
    }
    }

    // Just continue with current sub-task
    return m_pSubTask;
}

// 0x68CF80
CTask* CTaskComplexArrestPed::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68CF80, CTaskComplexArrestPed*, int32, CPed*>(this, taskType, ped);

    switch (taskType) {
    case TASK_SIMPLE_ARREST_PED:
        if (m_PedToArrest->m_pVehicle) {
            if (m_PedToArrest->m_pVehicle->IsDriver(m_PedToArrest)) {
                m_PedToArrest->m_pVehicle->vehicleFlags.bIsHandbrakeOn = true;
                m_PedToArrest->m_pVehicle->m_nStatus = STATUS_FORCED_STOP;
            }
        }
        return new CTaskSimpleArrestPed(m_PedToArrest);

    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return new CTaskComplexKillPedOnFoot(m_PedToArrest, -1, 0, 0, 0, 1);

    case TASK_COMPLEX_DESTROY_CAR:
        return new CTaskComplexDestroyCar(m_PedToArrest->m_pVehicle, 0, 0, 0);

    case TASK_COMPLEX_SEEK_ENTITY: {
        float radius = m_PedToArrest->bIsBeingArrested ? 4.0f : 3.0f;
        // return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorStandard>(m_PedToArrest, 50'000, 1000, radius, 2.0f, 2.0f, 1, 1);
        NOTSA_UNREACHABLE("Not implemented!");
    }
    case TASK_COMPLEX_DRAG_PED_FROM_CAR:
        return new CTaskComplexDragPedFromCar(m_PedToArrest, 100'000);

    case TASK_COMPLEX_CAR_OPEN_DRIVER_DOOR:
        return new CTaskComplexOpenDriverDoor(m_PedToArrest->m_pVehicle);

    case TASK_COMPLEX_CAR_OPEN_PASSENGER_DOOR:
        return new CTaskComplexOpenPassengerDoor(m_PedToArrest->m_pVehicle, 8); // todo: magic number

    case TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR: {
        return new CTaskSimpleWaitUntilPedIsOutCar{m_PedToArrest, m_PedToArrest->GetPosition() - ped->GetPosition()};
    }
    default:
        return nullptr;
    }
}
