#include "StdInc.h"
#include "TaskSimpleCarSetPedSlowDraggedOut.h"
#include "TaskSimplePlayerOnFoot.h"
#include <TaskComplexWander.h>
#include <TaskSimpleStandStill.h>
#include <PedPlacement.h>

void CTaskSimpleCarSetPedSlowDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedSlowDraggedOut, 0x86EF30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6483B0);
    RH_ScopedInstall(Destructor, 0x648430);

    RH_ScopedVMTInstall(Clone, 0x64A0B0);
    RH_ScopedVMTInstall(GetTaskType, 0x648410);
    RH_ScopedVMTInstall(MakeAbortable, 0x648420);
    RH_ScopedVMTInstall(ProcessPed, 0x64C220);
}

// 0x6483B0
CTaskSimpleCarSetPedSlowDraggedOut::CTaskSimpleCarSetPedSlowDraggedOut(CVehicle* target, eTargetDoor door) :
    m_Vehicle{target},
    m_Door{door}
{}

// 0x64C220
bool CTaskSimpleCarSetPedSlowDraggedOut::ProcessPed(CPed* ped) {
    if (!ped->m_pVehicle || !m_Vehicle) {
        return true;
    }

    // Remove ped as passenger/driver from the vehicle
    if (m_Vehicle->IsDriver(ped)) { // 0x64C27C
        m_Vehicle->RemoveDriver(true);
        if (m_Vehicle->m_nDoorLock == CARLOCK_COP_CAR) {
            m_Vehicle->m_nDoorLock = CARLOCK_UNLOCKED;
        }
        if (ped->IsCop() && m_Vehicle->IsLawEnforcementVehicle()) {
            m_Vehicle->ChangeLawEnforcerState(false);
        }
    } else { // 0x64C2B4
        for (const auto p : m_Vehicle->GetPassengers()) {
            if (p == ped) {
                m_Vehicle->RemovePassenger(ped); // Why not break anywhere? I mean, a ped shouldn't be in multiple seats at the same time
            }
        }
    }

    // Ped's not in a vehicle anymore (But don't clear `m_pVehicle` yet!)
    ped->bInVehicle = false;

    // Set default task after setting the ped out
    {
        CTask* task;
        if (ped->IsPlayer()) {
            task = new CTaskSimplePlayerOnFoot{}; // 0x64C320
        } else if (ped->m_nPedType == PED_MISSION) {
            task = new CTaskSimpleStandStill{999'999, true}; // 0x64C355
        } else {
            task = CTaskComplexWander::GetWanderTaskByPedType(ped); // 0x64C378
        }
        ped->GetTaskManager().SetTask(task, TASK_PRIMARY_DEFAULT);
    }

    // 0x64C38F
    ped->RestartNonPartialAnims();
    ped->m_bUsesCollision = true;
    ped->UpdateStatLeavingVehicle();
    ped->ReplaceWeaponWhenExitingVehicle();
    ped->ResetMoveSpeed();
    
    // 0x64C3D0 - Position ped out of the vehicle
    const auto doorOffset = m_Vehicle->GetAnimGroup().ComputeAnimDoorOffsets(
        notsa::contains({TARGET_DOOR_FRONT_RIGHT, TARGET_DOOR_REAR_RIGHT}, m_Door)
            ? JACK_PED_RIGHT
            : JACK_PED_LEFT
    );
    const auto offset = CVector{doorOffset - CCarEnterExit::ms_vecPedGetUpAnimOffset, 0.f};
    ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(ped->GetMatrix().TransformPoint(offset))));

    // 0x64C50B - Reset rotation
    ped->m_fCurrentRotation
        = ped->m_fAimingRotation
        = ped->GetHeading();
    ped->CalculateNewOrientation();

    // 0x64C52B - Make sure ped's not in a collision
    if (!CCarEnterExit::IsRoomForPedToLeaveCar(m_Vehicle, m_Door, &offset)) {
        ped->PositionPedOutOfCollision(m_Door, m_Vehicle, true);
    }

    // 0x64C554 - Set flags and reset move anim
    ped->bHasJustLeftCar      = true;
    ped->m_nPedState          = PEDSTATE_IDLE;
    ped->m_nSwimmingMoveState = PEDMOVE_STILL;
    ped->SetMoveAnim();

    // Task's over
    return true;
}
