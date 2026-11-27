#include "StdInc.h"

#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexKillPedOnFootArmed.h"
#include "TaskComplexKillPedOnFootMelee.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskSimpleCarDriveTimed.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimplePause.h"
#include "TaskSimpleNone.h"
#include "Events/EventAreaCodes.h"
#include "CarEnterExit.h"
#include "CullZones.h"

constexpr int32 ROOM_TO_DRAG_CHECK_INTERVAL_MS = 2'000;

void CTaskComplexKillPedOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFoot, 0x86d894, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    //RH_ScopedConstructorInstall(0x620E30, "1", {}, CPed*, int32, int32, int32, int32, uint8); // even with a wrapper for this original ctor it's ambigous so let's not hook it
    RH_ScopedDestructorInstall(0x620EF0);

    RH_ScopedInstall(CreateSubTask, 0x625E70);
    RH_ScopedInstall(SayAttackGang, 0x65E9A0);

    RH_ScopedVMTInstall(Clone, 0x623370);
    RH_ScopedVMTInstall(GetTaskType, 0x620EE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x625E40);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62B150);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62B490);
    RH_ScopedVMTInstall(ControlSubTask, 0x626260);
}

// 0x620E30
CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(
    CPed* target,
    int32 time,
    int32 pedFlags,
    int32 delay,
    int32 chance,
    uint8 competence,
    bool  waitForPlayerToBeSafe,
    bool  waitingForPlayerToBeSafe
) :
    m_WaitForPlayerToBeSafe{ waitForPlayerToBeSafe },
    m_WaitingForPlayerToBeSafe{ waitingForPlayerToBeSafe },
    m_PedToKill{ target },
    m_PedFlags{ pedFlags },
    m_ActionDelay{ delay },
    m_ActionChance{ chance },
    m_Competence{ competence },
    m_StartTime{ CTimer::GetTimeInMS() },
    m_Time{ time } {
}

// 0x623370
CTaskComplexKillPedOnFoot::CTaskComplexKillPedOnFoot(const CTaskComplexKillPedOnFoot& o) :
    CTaskComplexKillPedOnFoot{
        o.m_PedToKill,
        o.m_Time,
        o.m_PedFlags,
        o.m_ActionDelay,
        o.m_ActionChance,
        o.m_Competence
    } {
    m_AimImmediate          = o.m_AimImmediate;
    m_WaitForPlayerToBeSafe = o.m_WaitForPlayerToBeSafe;
}

// 0x625E40
bool CTaskComplexKillPedOnFoot::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    ped->bDontAcceptIKLookAts = false;
    return !m_pSubTask || m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62B150
CTask* CTaskComplexKillPedOnFoot::CreateNextSubTask(CPed* ped) {
    if (!m_PedToKill) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_NONE:                // 0x62B1AA
    case TASK_SIMPLE_STAND_STILL:  // 0x62B19F
    case TASK_COMPLEX_DESTROY_CAR: // 0x62B410
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_PAUSE: { // 0x62B1B9
        if (ped->IsCop() && ped->AsCop()->m_bDontPursuit) {
            // NOTE: The original reads `m_ChanceOnRoadBlock` (+0x1C) of the player's wanted here (0x62B1DA)
            return CreateSubTask(
                FindPlayerWanted()->m_ChanceOnRoadBlock > 0
                    ? TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED
                    : TASK_SIMPLE_PAUSE,
                ped
            );
        }
        if (m_WaitForPlayerToBeSafe && m_WaitingForPlayerToBeSafe && !FindPlayerWanted()->m_bEverybodyBackOff) { // 0x62B1FD
            m_WaitingForPlayerToBeSafe = false;
            return CreateFirstSubTask(ped);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x62B259
        if (notsa::cast<CTaskComplexDragPedFromCar>(m_pSubTask)->GetQuitAfterDraggingPedOut()) {
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        return CreateSubTask(
            ped->GetActiveWeapon().IsTypeMelee()
                ? TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE
                : TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED,
            ped
        );
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x62B2B8
        auto* const player = m_PedToKill->IsPlayer() ? m_PedToKill->AsPlayer() : nullptr;
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_SIMPLE_CAR_DRIVE_TIMED, ped);
        }
        const auto* const standingOn = m_PedToKill->m_standingOnEntity;
        if (CCullZones::NoPolice() || standingOn && standingOn != ped->m_standingOnEntity && (m_PedToKill->GetPosition() - ped->GetPosition()).Magnitude() < 5.f) {
            if (player && player->GetPlayerData()->m_bStoppedMoving && (ped->IsGangster() || ped->IsCop()) && !ped->IsCreatedByMission()) { // 0x62B380
                ped->GiveWeapon(WEAPON_PISTOL, 1'000, true);
                ped->SetCurrentWeapon(WEAPON_PISTOL);
                ped->SetMoveState(PEDMOVE_STILL);
            }
        }
        return CreateSubTask(
            ped->GetActiveWeapon().IsTypeMelee()
                ? TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE
                : TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED,
            ped
        );
    }
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x62B443
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: // 0x62B427
    case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_COMPLEX_SIGNAL_AT_PED: // 0x62B482
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x62B490
CTask* CTaskComplexKillPedOnFoot::CreateFirstSubTask(CPed* ped) {
    m_NewTarget             = false;
    m_RoomToDragPedOutOfCar = true;

    if (!m_PedToKill) {
        ped->bDontAcceptIKLookAts = false;
        return nullptr;
    }

    if (m_PedToKill->m_fHealth <= 0.f) {
        m_TargetKilled = true;
    }

    const float dist     = (m_PedToKill->GetPosition() - ped->GetPosition()).Magnitude();
    auto* const player   = m_PedToKill->IsPlayer() ? m_PedToKill->AsPlayer() : nullptr;

    CTask* const subTask = [&]() -> CTask* {
        if (ped->IsInVehicle()) { // 0x62B53E
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }

        if (player) { // 0x62B55E
            const auto* const standingOn = m_PedToKill->m_standingOnEntity;
            if (CCullZones::NoPolice() || standingOn && standingOn != ped->m_standingOnEntity && dist < 5.f) {
                if (player->GetPlayerData()->m_bStoppedMoving && (ped->IsGangster() || ped->IsCop()) && !ped->IsCreatedByMission()) { // 0x62B591
                    ped->GiveWeapon(WEAPON_PISTOL, 1'000, true);
                    ped->SetCurrentWeapon(WEAPON_PISTOL);
                    ped->SetMoveState(PEDMOVE_STILL);
                }
            }
        }

        auto* const veh = m_PedToKill->m_pVehicle;
        if (!m_PedToKill->bInVehicle || !veh || !veh->IsDriver(m_PedToKill) && !veh->IsPassenger(m_PedToKill)) { // 0x62B741
            return CreateSubTask(
                ped->GetActiveWeapon().IsTypeMelee()
                    ? TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE
                    : TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED,
                ped
            );
        }

        m_RoomToDragPedOutOfCar = true;
        if (veh->IsSubHeli() || veh->IsSubPlane() || ped->bStayInSamePlace) { // 0x62B642
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        if (CVector2D{ veh->GetMoveSpeed() }.Magnitude() > 0.1f || !veh->CanPedOpenLocks(ped)) { // 0x62B672
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        if (veh->IsBike() && !ped->GetActiveWeapon().IsTypeMelee() || veh->IsBoat()) { // 0x62B6A5
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        if (!UpdateRoomToDragPedOutOfCar(veh)) { // 0x62B6E3
            return CreateSubTask(TASK_COMPLEX_DESTROY_CAR, ped);
        }
        return CreateSubTask(
            m_PedToKill->bDontDragMeOutCar
                ? TASK_COMPLEX_DESTROY_CAR
                : TASK_COMPLEX_DRAG_PED_FROM_CAR,
            ped
        );
    }();

    ped->DropEntityThatThisPedIsHolding(true);

    return subTask;
}

// 0x626260
CTask* CTaskComplexKillPedOnFoot::ControlSubTask(CPed* ped) {
    ped->bDontAcceptIKLookAts = true;

    auto newTaskType          = TASK_NONE;
    if (!m_PedToKill) {
        newTaskType = TASK_FINISHED;
    } else if (notsa::isa<CTaskSimplePause>(m_pSubTask)) { // 0x6262B1
        if (m_WaitForPlayerToBeSafe && m_WaitingForPlayerToBeSafe && !FindPlayerWanted()->m_bEverybodyBackOff) {
            m_pSubTask->MakeAbortable(ped);
        }
        return m_pSubTask;
    } else if (m_Time > 0 && CTimer::GetTimeInMS() > m_StartTime + m_Time) { // 0x6262F3
        if (m_pSubTask->MakeAbortable(ped)) {
            return new CTaskSimpleStandStill{};
        }
    } else if (m_NewTarget) { // 0x62634A
        if (m_pSubTask->MakeAbortable(ped)) {
            return CreateFirstSubTask(ped);
        }
    } else {
        if (!m_TargetKilled) {
            if (m_PedToKill->m_fHealth <= 0.f) { // 0x626395
                if (m_pSubTask->MakeAbortable(ped)) {
                    m_TargetKilled = true;
                    if (notsa::isa<CTaskComplexSignalAtPed>(m_pSubTask)) {
                        return m_pSubTask;
                    }
                    if (ped->IsCop() && !m_PedToKill->IsPlayer()) {
                        ped->Say(CTX_GLOBAL_ARREST_CRIM);
                    }
                    if (ped->bSignalAfterKill) {
                        return CreateSubTask(TASK_COMPLEX_SIGNAL_AT_PED, ped);
                    }
                    return new CTaskSimpleNone{};
                }
            } else if (m_PedToKill->IsPlayer()) { // 0x62643B
                const auto* const wanted = m_PedToKill->AsPlayer()->GetWanted();
                // NOTE: The original reads `m_ChanceOnRoadBlock` (+0x1C) of the player's wanted here (0x62647E)
                const auto waitForPlayer = wanted->m_bEverybodyBackOff || ped->IsCop() && ped->AsCop()->m_bDontPursuit && FindPlayerWanted()->m_ChanceOnRoadBlock == 0;
                if (waitForPlayer && !notsa::isa<CTaskSimplePause>(m_pSubTask)) { // 0x626485
                    newTaskType = TASK_SIMPLE_PAUSE;
                    if (m_WaitForPlayerToBeSafe && wanted->m_bEverybodyBackOff) {
                        m_WaitingForPlayerToBeSafe = true;
                    }
                }
            }
        }

        if (newTaskType != TASK_SIMPLE_PAUSE) { // 0x6264CD
            switch (const auto tt = m_pSubTask->GetTaskType()) {
            case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: { // 0x6264FC
                notsa::cast<CTaskComplexKillPedOnFootMelee>(m_pSubTask)->m_bShotFiredByPlayerFlag = m_ShotFiredByPlayer;
                if (!ped->GetActiveWeapon().IsTypeMelee()) {
                    newTaskType = TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;
                    break;
                }
                if (!m_PedToKill->bInVehicle) {
                    break;
                }
                auto* const veh = m_PedToKill->m_pVehicle;
                if (notsa::bugfixes::GenericCrashing && !veh) { // The original reads the vehicle's type before its null check
                    break;
                }
                if (veh->IsSubPlane() || veh->IsSubHeli()) { // 0x62653C
                    newTaskType = TASK_COMPLEX_DESTROY_CAR;
                    break;
                }
                if (!veh || veh->IsBoat() || ped->bStayInSamePlace || !veh->CanPedOpenLocks(ped)) { // 0x626564
                    break;
                }
                newTaskType = UpdateRoomToDragPedOutOfCar(veh)
                    ? TASK_COMPLEX_DRAG_PED_FROM_CAR
                    : TASK_COMPLEX_DESTROY_CAR;
                break;
            }
            case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED: { // 0x6266D9
                notsa::cast<CTaskComplexKillPedOnFootArmed>(m_pSubTask)->m_bShotFiredByPlayer = m_ShotFiredByPlayer;
                if (ped->GetActiveWeapon().IsTypeMelee() && !ped->IsPlayer()) {
                    newTaskType = TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE;
                } else if (ped->GetActiveWeapon().GetTotalAmmo() == 0 && !ped->IsPlayer()) { // 0x626709
                    const auto slot = rng::find_if(ped->m_aWeapons, [](const CWeapon& wep) {
                        return (int32)wep.GetTotalAmmo() > 0;
                    });
                    if (slot != ped->m_aWeapons.end()) {
                        ped->SetCurrentWeapon((int32)rng::distance(ped->m_aWeapons.begin(), slot));
                    } else {
                        ped->SetCurrentWeapon(WEAPON_UNARMED);
                        newTaskType = TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE;
                    }
                }
                auto* const veh = m_PedToKill->m_pVehicle;
                if (!m_PedToKill->bInVehicle || !veh || !veh->IsDriver(m_PedToKill) && !veh->IsPassenger(m_PedToKill)) { // 0x62675C
                    break;
                }
                if (veh->IsSubPlane() || veh->IsSubHeli()) { // 0x6267B0
                    newTaskType = TASK_COMPLEX_DESTROY_CAR;
                    break;
                }
                if (veh->IsBike() || veh->IsBoat() || ped->bStayInSamePlace) { // 0x6267CC
                    break;
                }
                if (CVector2D{ veh->GetMoveSpeed() }.Magnitude() > 0.1f || !veh->CanPedOpenLocks(ped)) { // 0x6267F4
                    break;
                }
                newTaskType = UpdateRoomToDragPedOutOfCar(veh)
                    ? TASK_COMPLEX_DRAG_PED_FROM_CAR
                    : TASK_COMPLEX_DESTROY_CAR;
                break;
            }
            case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x6265C2
                auto* const veh = m_PedToKill->m_pVehicle;
                if (!m_PedToKill->bInVehicle || !veh || !veh->IsDriver(m_PedToKill) && !veh->IsPassenger(m_PedToKill)) {
                    break;
                }
                if (CVector2D{ veh->GetMoveSpeed() }.Magnitude() > 0.1f) { // 0x626603
                    newTaskType = TASK_COMPLEX_DESTROY_CAR;
                    break;
                }
                if (!m_Timer.IsOutOfTime()) { // 0x62663B
                    break;
                }
                if (!UpdateRoomToDragPedOutOfCar(veh)) {
                    newTaskType = TASK_COMPLEX_DESTROY_CAR;
                }
                m_Timer.Start(ROOM_TO_DRAG_CHECK_INTERVAL_MS);
                break;
            }
            case TASK_COMPLEX_DESTROY_CAR: { // 0x626884
                if (!m_PedToKill->bInVehicle) {
                    newTaskType = ped->GetActiveWeapon().IsTypeMelee()
                        ? TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE
                        : TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED;
                    break;
                }
                auto* const veh = m_PedToKill->m_pVehicle;
                if (veh && !CCarEnterExit::IsVehicleHealthy(veh)) { // 0x6268B8
                    newTaskType = TASK_COMPLEX_SIGNAL_AT_PED;
                    break;
                }
                if (m_PedToKill->bDontDragMeOutCar) { // 0x6268D9
                    break;
                }
                if (notsa::bugfixes::GenericCrashing && !veh) { // The original reads the vehicle without a null check
                    break;
                }
                if (veh->IsSubPlane() || veh->IsSubHeli() || veh->IsBike() || veh->IsBoat() || ped->bStayInSamePlace) { // 0x6268EC
                    break;
                }
                if (CVector2D{ veh->GetMoveSpeed() }.Magnitude() > 0.1f || !veh->CanPedOpenLocks(ped)) { // 0x626940
                    break;
                }
                if (m_RoomToDragPedOutOfCar) { // 0x62696B
                    newTaskType = TASK_COMPLEX_DRAG_PED_FROM_CAR;
                    break;
                }
                if (!m_Timer.IsOutOfTime()) {
                    break;
                }
                if (UpdateRoomToDragPedOutOfCar(veh)) {
                    newTaskType = TASK_COMPLEX_DRAG_PED_FROM_CAR;
                }
                m_Timer.Start(ROOM_TO_DRAG_CHECK_INTERVAL_MS);
                break;
            }
            case TASK_NONE: // 0x6269CF
            case TASK_SIMPLE_STAND_STILL:
            case TASK_COMPLEX_LEAVE_CAR:
            case TASK_SIMPLE_CAR_DRIVE_TIMED:
            case TASK_COMPLEX_SIGNAL_AT_PED:
                break;
            default:
                NOTSA_UNREACHABLE_CASE(tt);
            }
        }
        m_ShotFiredByPlayer = false;
    }

    if (m_PedToKill) { // 0x6269D3
        if (const auto* const targetContact = m_PedToKill->m_pContactEntity) {
            if (const auto* const ourContact = ped->m_pContactEntity) {
                if (targetContact->GetAreaCode() != ourContact->GetAreaCode()) {
                    ped->GetEventGroup().Add(CEventAreaCodes{ m_PedToKill });
                }
            }
        }
    }

    if (newTaskType != TASK_NONE && m_pSubTask->MakeAbortable(ped)) { // 0x626A2E
        return CreateSubTask(newTaskType, ped);
    }

    if (m_PedToKill) {
        SayAttackGang(ped, m_PedToKill);
    }

    return m_pSubTask;
}

// 0x625E70
CTask* CTaskComplexKillPedOnFoot::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT_ARMED: { // 0x626092
        auto* const tArmed = new CTaskComplexKillPedOnFootArmed{
            m_PedToKill,
            (uint32)m_PedFlags,
            (uint32)m_ActionDelay,
            (uint32)m_ActionChance,
            (int8)m_Competence
        };
        tArmed->m_aimImmediate = m_AimImmediate;
        m_AimImmediate         = false;
        m_Timer.Start(ROOM_TO_DRAG_CHECK_INTERVAL_MS);
        return tArmed;
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT_MELEE: { // 0x6260F6
        auto* const tMelee = new CTaskComplexKillPedOnFootMelee{ m_PedToKill };
        m_Timer.Start(ROOM_TO_DRAG_CHECK_INTERVAL_MS);
        return tMelee;
    }
    case TASK_COMPLEX_DRAG_PED_FROM_CAR: { // 0x626009
        auto* const tDrag = new CTaskComplexDragPedFromCar{ m_PedToKill, 0 };
        m_Timer.Start(ROOM_TO_DRAG_CHECK_INTERVAL_MS);
        return tDrag;
    }
    case TASK_COMPLEX_LEAVE_CAR: // 0x625FCC
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, true };
    case TASK_SIMPLE_CAR_DRIVE_TIMED: // 0x626039
        return new CTaskSimpleCarDriveTimed{ ped->m_pVehicle, 2'000 };
    case TASK_COMPLEX_DESTROY_CAR: // 0x626138
        return new CTaskComplexDestroyCar{ m_PedToKill->m_pVehicle };
    case TASK_SIMPLE_STAND_STILL: // 0x625F89
        return new CTaskSimpleStandStill{};
    case TASK_NONE: // 0x625F5F
        return new CTaskSimpleNone{};
    case TASK_SIMPLE_PAUSE: { // 0x625EC6
        CTaskSimpleStandStill{}.ProcessPed(ped);
        return new CTaskSimplePause{ m_WaitForPlayerToBeSafe && m_WaitingForPlayerToBeSafe ? 10'000 : 2'000 };
    }
    case TASK_COMPLEX_SIGNAL_AT_PED: { // 0x626193
        if (m_PedToKill == FindPlayerPed()) {
            ped->Say(CTX_GLOBAL_PLAYER_WASTED);
        } else if (auto* const grp = CPedGroups::GetPedsGroup(m_PedToKill); !grp || grp->GetMembership().GetLeader() != FindPlayerPed()) {
            ped->Say(CTX_GLOBAL_ENEMY_GANG_WASTED);
        }
        return new CTaskComplexSignalAtPed{ m_PedToKill, CGeneral::GetRandomNumberInRange(0, 1'500), true };
    }
    case TASK_FINISHED: // 0x62617E
        ped->bDontAcceptIKLookAts = false;
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

// NOTSA
bool CTaskComplexKillPedOnFoot::UpdateRoomToDragPedOutOfCar(const CVehicle* veh) {
    return m_RoomToDragPedOutOfCar = CCarEnterExit::IsRoomForPedToLeaveCar(veh, CCarEnterExit::ComputeTargetDoorToExit(veh, m_PedToKill));
}

// 0x65E9A0
void CTaskComplexKillPedOnFoot::SayAttackGang(CPed* ped, CPed* target) {
    if (!ped || !target || !ped->IsGangster()) {
        return;
    }
    if (!target->IsGangster() && target != FindPlayerPed()) {
        return;
    }
    switch (target->m_nPedType) {
    case PED_TYPE_GANG1: ped->Say(CTX_GLOBAL_ATTACK_GANG_BALLAS); break;
    case PED_TYPE_GANG3: ped->Say(CTX_GLOBAL_ATTACK_GANG_LSV); break;
    case PED_TYPE_GANG8: ped->Say(CTX_GLOBAL_ATTACK_GANG_VLA); break;
    default:             break;
    }
}
