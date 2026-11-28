#include "StdInc.h"

#include <reversiblebugfixes/Bugs.hpp>
#include "TaskSimpleCarSetPedInAsPassenger.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "TaskSimpleCarSetPedOut.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexLeaveCarAndDie.h"
#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskSimpleBikeJacked.h"

// OG constructor was at 0x646FE0
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor targetDoor, bool warpingInToCar, CTaskUtilityLineUpPedWithCar* utility) :
    m_TargetVehicle{ targetVehicle },
    m_TargetDoor{ targetDoor },
    m_Utility{ utility },
    m_WarpingInToCar{ warpingInToCar } {
}

CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* targetVehicle, eTargetDoor targetDoor, CTaskUtilityLineUpPedWithCar* utility) :
    CTaskSimpleCarSetPedInAsPassenger{
        targetVehicle,
        targetDoor,
        false,
        utility
    } {
}

// For 0x649D90
CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(const CTaskSimpleCarSetPedInAsPassenger& o) :
    CTaskSimpleCarSetPedInAsPassenger{
        o.m_TargetVehicle,
        o.m_TargetDoor,
        o.m_WarpingInToCar,
        o.m_Utility
    } {
    m_DoorFlagsToClear    = o.m_DoorFlagsToClear;
    m_NumGettingInToClear = o.m_NumGettingInToClear;
}

// 0x64B5D0
bool CTaskSimpleCarSetPedInAsPassenger::ProcessPed(CPed* ped) {
    // NOTE: The original calls `GetParent()->GetTaskType()` here if `ped->bInVehicle`, and ignores the result

    if (notsa::bugfixes::GenericCrashing) {
        if (!m_TargetVehicle) { // The original doesn't check if the vehicle still exists
            return true;
        }
    }

    CEntity::ChangeEntityReference(ped->m_pVehicle, m_TargetVehicle.Get());
    ped->m_fAimingRotation = ped->m_fCurrentRotation;
    ped->bInVehicle        = true;

    if (ped->IsPlayer()) {
        ped->GetPlayerData()->m_bPlayersGangActive = true;
        ped->AsPlayer()->ClearAdrenaline();
    }

    if (ped->IsPlayer() && !m_TargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer) {
        m_TargetVehicle->vehicleFlags.bHasBeenOwnedByPlayer = true;
        CCrime::ReportCrime(CRIME_CAR_STEAL, m_TargetVehicle, FindPlayerPed());
    }

    const auto seat = m_TargetVehicle->vehicleFlags.bIsBus
        ? -1
        : CCarEnterExit::ComputePassengerIndexFromCarDoor(m_TargetVehicle, m_TargetDoor);
    if (seat != -1) { // 0x64B6CA - Force the ped sitting in the seat out
        auto* const pedInSeat = m_TargetVehicle->GetPassengers()[seat];
        if (pedInSeat && pedInSeat != ped && !pedInSeat->GetTaskManager().HasAnyOf<CTaskComplexLeaveCar, CTaskComplexCarSlowBeDraggedOutAndStandUp, CTaskComplexLeaveCarAndDie, CTaskSimpleBikeJacked>()) {
            CTaskSimpleCarSetPedOut{ m_TargetVehicle, (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(m_TargetVehicle, pedInSeat), true, true }.ProcessPed(pedInSeat);
        }
    }

    if (m_WarpingInToCar) {
        ped->SetPosn(m_TargetVehicle->GetPosition());
    }

    if (seat == -1) {
        m_TargetVehicle->AddPassenger(ped);
    } else {
        m_TargetVehicle->AddPassenger(ped, (uint8)seat);
    }

    ped->UpdateStatEnteringVehicle();
    ped->SetMoveState(PEDMOVE_NONE);
    ped->SetMoveAnim();
    ped->SetUsesCollision(false);

    if (m_TargetVehicle->m_nAlarmState == (uint16)-1) {
        m_TargetVehicle->m_nAlarmState = 15'000;
    }

    ped->SetPedState(PEDSTATE_DRIVING);

    if (m_DoorFlagsToClear) {
        m_TargetVehicle->ClearGettingInFlags(m_DoorFlagsToClear);
    }

    if (m_NumGettingInToClear) {
        m_TargetVehicle->m_nNumGettingIn -= m_NumGettingInToClear;
    }

    ped->RemoveWeaponWhenEnteringVehicle(0);
    ped->bRenderPedInCar = !m_TargetVehicle->vehicleFlags.bIsBus;

    CCarEnterExit::RemoveGetInAnims(ped);
    CCarEnterExit::AddInCarAnim(m_TargetVehicle, ped, false);

    if (!m_WarpingInToCar) {
        if (notsa::bugfixes::GenericCrashing) {
            if (m_Utility) { // The original doesn't check for a null utility
                m_Utility->ProcessPed(ped, m_TargetVehicle, nullptr);
            }
        } else {
            m_Utility->ProcessPed(ped, m_TargetVehicle, nullptr);
        }
    }

    ped->GetTaskManager().SetTask(new CTaskSimpleCarDrive{ m_TargetVehicle, m_Utility, false }, TASK_PRIMARY_DEFAULT);

    return true;
}

void CTaskSimpleCarSetPedInAsPassenger::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSetPedInAsPassenger, 0x86ee04, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x646FE0, "1", {}, CVehicle*, eTargetDoor, CTaskUtilityLineUpPedWithCar*);
    RH_ScopedDestructorInstall(0x647080);

    RH_ScopedVMTInstall(Clone, 0x649D90);
    RH_ScopedVMTInstall(GetTaskType, 0x647060);
    RH_ScopedVMTInstall(MakeAbortable, 0x647070);
    RH_ScopedVMTInstall(ProcessPed, 0x64B5D0);
}
