#include "StdInc.h"

#include <extensions/utility.hpp>
#include <reversiblebugfixes/Bugs.hpp>

#include "TaskComplexKillCriminal.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexCarDriveMission.h"
#include "Events/EventAcquaintancePedHate.h"
#include "InterestingEvents.h"
#include "LoadMonitor.h"
#include "CarCtrl.h"

constexpr auto NEEDED_FOR_MS            = 300'000u;
constexpr auto DRIVEBY_RANGE            = 60.f;
constexpr auto DRIVEBY_ABORT_RANGE      = 70.f;
constexpr auto DRIVEBY_FREQUENCY_PERC   = 70;
constexpr auto ON_FOOT_RANGE            = 25.f;
constexpr auto LEAVE_CAR_RANGE          = 15.f;
constexpr auto CRIMINAL_CAR_MIN_HEALTH  = 250.f; //!< Below this the criminals leave their car
constexpr auto CHASE_CRUISE_SPEED_BONUS = 10.f;  // 0x85862C
constexpr auto KILL_PED_CRUISE_SPEED    = 20;

static bool NoPedOrNoHp(const CPed* ped) {
    return !ped || ped->m_fHealth <= 0.f;
}

static bool IsPlayer(const CPed* ped) {
    return ped && ped->IsPlayer();
}

static void GivePistol(CPed* ped) {
    ped->GiveWeapon(WEAPON_PISTOL, 1'000, true);
    ped->SetCurrentWeapon(WEAPON_PISTOL);
}

// 0x68BE70
CTaskComplexKillCriminal::CTaskComplexKillCriminal(CPed* criminal, bool randomize) :
    m_Criminal{ criminal },
    m_Randomize{ randomize } {
    if (m_Criminal && !m_Criminal->CanBeCriminal()) {
        m_Criminal = nullptr;
    }
}

// NOTSA (For 0x68CE50)
CTaskComplexKillCriminal::CTaskComplexKillCriminal(const CTaskComplexKillCriminal& o) :
    CTaskComplexKillCriminal{ o.m_Criminal, false } {
}

// 0x68BF30
CTaskComplexKillCriminal::~CTaskComplexKillCriminal() {
    if (!m_Cop) {
        return;
    }
    m_Cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS();
    m_Cop->bCullExtraFarAway        = false;
    m_Cop->m_fRemovalDistMultiplier = 1.f;
    if (auto* const veh = m_Cop->m_pVehicle) {
        veh->m_nExtendedRemovalRange                   = 0;
        veh->vehicleFlags.bNeverUseSmallerRemovalRange = false;
        if (veh->IsDriver(m_Cop)) { // 0x68bfc7
            auto& ap = veh->m_autoPilot;
            ap.SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
            ap.SetCarMission(MISSION_CRUISE);
            ap.SetCruiseSpeed(10);
            if (veh->GetStatus() != STATUS_SIMPLE) {
                CCarCtrl::JoinCarWithRoadSystem(veh);
            }
            veh->vehicleFlags.bSirenOrAlarm = false;
        }
    }
}

// 0x68C050
CTask* CTaskComplexKillCriminal::CreateSubTask(eTaskType tt, CPed* ped, bool force) {
    if (!force && m_pSubTask && m_pSubTask->GetTaskType() == tt) {
        return m_pSubTask;
    }

    switch (tt) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x68c105
        return new CTaskComplexEnterCarAsPassenger{ ped->m_pVehicle };
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: // 0x68c0ce
        return new CTaskComplexEnterCarAsDriver{ ped->m_pVehicle };
    case TASK_COMPLEX_LEAVE_CAR: // 0x68c177
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
    case TASK_SIMPLE_CAR_DRIVE: // 0x68c13e
        return new CTaskSimpleCarDrive{ ped->m_pVehicle };
    case TASK_COMPLEX_CAR_DRIVE_MISSION: { // 0x68c1b3
        auto* const veh = ped->m_pVehicle;
        if (!veh) {
            return nullptr;
        }
        if (notsa::bugfixes::GenericCrashing && !m_Criminal) { // NOTE/BUG: Originally not checked (The criminal can be gone when a drive-by is over, see `CreateNextSubTask`)
            return nullptr;
        }
        if (auto* const criminalVeh = m_Criminal->GetVehicleIfInOne()) {
            return new CTaskComplexCarDriveMission{
                veh,
                criminalVeh,
                veh->IsBike()
                    ? MISSION_FOLLOWCAR_CLOSE
                    : MISSION_BLOCKCAR_CLOSE,
                DRIVING_STYLE_AVOID_CARS,
                (float)criminalVeh->m_autoPilot.m_nCruiseSpeed + CHASE_CRUISE_SPEED_BONUS
            };
        }
        return new CTaskComplexCarDriveMission{ veh, m_Criminal, MISSION_KILLPED_CLOSE, DRIVING_STYLE_AVOID_CARS, (float)KILL_PED_CRUISE_SPEED };
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x68c33a
        auto* const tKillPed = new CTaskComplexKillPedOnFoot{ m_Criminal };
        ped->SetCurrentWeapon(WEAPON_PISTOL);
        return tKillPed;
    }
    case TASK_SIMPLE_GANG_DRIVEBY: { // 0x68c308
        auto* const tDriveBy = new CTaskSimpleGangDriveBy{ m_Criminal, nullptr, DRIVEBY_ABORT_RANGE, DRIVEBY_FREQUENCY_PERC, eDrivebyStyle::AI_ALL_DIRN, false };
        ped->SetCurrentWeapon(WEAPON_PISTOL);
        return tDriveBy;
    }
    case TASK_FINISHED: { // 0x68c2f4
        if (m_Criminal) {
            m_Criminal->SetPedDefaultDecisionMaker();
        }
        return nullptr;
    }
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x68C3C0
CPed* CTaskComplexKillCriminal::FindNextCriminalToKill(CPed* ped, bool any) {
    const auto GetDistSq = [ped](const CPed* criminal) {
        return (criminal->GetPosition() - ped->GetPosition()).SquaredMagnitude(); // NOTE: 3D
    };

    // NOTE/BUG: Never updated in the loop below, so the result is the last ped that is closer than the current criminal (Or just the last one), not the closest one
    const float maxDistSq = !any && !NoPedOrNoHp(m_Criminal)
        ? GetDistSq(m_Criminal)
        : 0.f;
    CPed*       next{};
    for (auto* const criminal : m_Cop->m_apCriminalsToKill) {
        if (NoPedOrNoHp(criminal) || criminal == m_Criminal) {
            continue;
        }
        if (GetDistSq(criminal) < maxDistSq || maxDistSq == 0.f) {
            next = criminal;
        }
    }
    return next;
}

// 0x68C6E0
bool CTaskComplexKillCriminal::ChangeTarget(CPed* newTarget) {
    if (newTarget == m_Criminal) {
        return true;
    }

    if (NoPedOrNoHp(newTarget)) {
        return false;
    }

    if (m_Criminal && m_Criminal->bInVehicle) {
        return false;
    }

    // NOTE/BUG: The new target is passed on as the ped the sub task belongs to
    if (notsa::isa_and_present<CTaskComplexKillPedOnFoot>(m_pSubTask) && !m_pSubTask->MakeAbortable(newTarget, ABORT_PRIORITY_URGENT, nullptr)) { // 0x68c725
        return false;
    }

    if (!notsa::contains(m_Cop->m_apCriminalsToKill, newTarget)) { // 0x68c760
        return false;
    }

    m_Criminal = newTarget;

    if (auto* const partner = m_Cop->m_pCopPartner; partner && partner->bInVehicle) { // 0x68c7a8
        if (auto* const tPartnerKillCriminal = partner->GetTaskManager().Find<CTaskComplexKillCriminal>(false)) {
            tPartnerKillCriminal->ChangeTarget(newTarget);
        }
    }

    m_HasFinished = false;

    return true;
}

// 0x68DAD0
bool CTaskComplexKillCriminal::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (!event) {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }

    const auto IgnoreEvent = [event] {
        const_cast<CEvent*>(event)->m_nTimeActive++;
        return false;
    };

    switch (const auto evType = event->GetEventType()) {
    case EVENT_ACQUAINTANCE_PED_HATE:
    case EVENT_VEHICLE_DAMAGE_COLLISION: // 0x68db35
        return IgnoreEvent();
    case EVENT_DAMAGE:
    case EVENT_VEHICLE_DAMAGE_WEAPON:
    case EVENT_GUN_AIMED_AT:
    case EVENT_SHOT_FIRED:            { // 0x68db5f
        auto* const evSrc = event->GetSourceEntity();
        if (m_Criminal && evSrc == m_Criminal) { // 0x68daf8
            return IgnoreEvent();
        }
        if (!evSrc || !evSrc->GetIsTypePed() || evSrc->AsPed()->IsPlayer() || !m_Cop) {
            return IgnoreEvent();
        }
        // NOTE/BUG: The original takes slot 0 for a failure (and -1, the real failure, for a success)
        if (m_Cop->AddCriminalToKill(evSrc->AsPed()) == (notsa::IsFixBugs() ? -1 : 0)) { // 0x68dbae
            return IgnoreEvent();
        }
        // 0x68dbb7 - Go after the attacker right away (NOTE: The distance is 3D)
        if (evType == EVENT_DAMAGE || evType == EVENT_VEHICLE_DAMAGE_WEAPON) {
            if (!m_Criminal || (m_Criminal->GetPosition() - ped->GetPosition()).SquaredMagnitude() < sq(ON_FOOT_RANGE)) {
                ChangeTarget(evSrc->AsPed());
            }
        }
        return IgnoreEvent();
    }
    default: // 0x68dc3a
        return m_pSubTask->MakeAbortable(ped, priority, event);
    }
}

// 0x68E4F0
CTask* CTaskComplexKillCriminal::CreateNextSubTask(CPed* ped) {
    const auto* const partner = m_Cop->m_pCopPartner;
    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: { // 0x68e6f6
        if (ped->bInVehicle) {
            return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
        }
        m_CantGetInCar = true;
        return CreateSubTask(
            m_HasFinished || NoPedOrNoHp(m_Criminal)
                ? TASK_FINISHED
                : TASK_COMPLEX_KILL_PED_ON_FOOT,
            ped
        );
    }
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: { // 0x68e52f
        if (!ped->bInVehicle) {
            m_CantGetInCar = true;
            return CreateSubTask(
                m_HasFinished || NoPedOrNoHp(m_Criminal)
                    ? TASK_FINISHED
                    : TASK_COMPLEX_KILL_PED_ON_FOOT,
                ped
            );
        }
        const bool isPartnerReady = NoPedOrNoHp(partner) || partner->bInVehicle; // 0x68e586

        if (!m_HasFinished && !NoPedOrNoHp(m_Criminal)) { // 0x68e5ab
            if (!m_Criminal->IsInVehicle()) {
                if (!ped->m_pVehicle || (ped->m_pVehicle->GetPosition() - m_Criminal->GetPosition()).SquaredMagnitude() <= sq(ON_FOOT_RANGE)) { // 0x68e5ea | NOTE: 3D
                    return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
                }
            }
            return CreateSubTask( // 0x68e635
                isPartnerReady ? TASK_COMPLEX_CAR_DRIVE_MISSION : TASK_SIMPLE_CAR_DRIVE,
                ped
            );
        }
        if (auto* const next = FindNextCriminalToKill(ped, true); next && ChangeTarget(next)) { // 0x68e685
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped, true);
        }
        if (ped->IsInVehicle()) {
            ped->m_pVehicle->vehicleFlags.bSirenOrAlarm = false;
        }
        return CreateSubTask(
            isPartnerReady
                ? TASK_FINISHED
                : TASK_SIMPLE_CAR_DRIVE,
            ped
        );
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x68e77b
        if (!ped->m_pVehicle || m_CantGetInCar) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
        }
        if (!m_HasFinished && !NoPedOrNoHp(m_Criminal) && !m_Criminal->IsInVehicle()) {
            if ((ped->GetPosition() - m_Criminal->GetPosition()).SquaredMagnitude() <= sq(ON_FOOT_RANGE)) { // 0x68e7ce | NOTE: 3D
                return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
            }
        }
        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: { // 0x68e887
        auto* const next = NoPedOrNoHp(m_Criminal)
            ? FindNextCriminalToKill(ped, true)
            : m_Criminal.Get();
        if (next && ChangeTarget(next)) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped, true);
        }

        // Nobody is left, go back to the car (if any)
        m_HasFinished = true;
        if (m_CantGetInCar || !ped->m_pVehicle) { // 0x68e8c7
            return CreateSubTask(TASK_FINISHED, ped);
        }
        if (!m_Cop->m_isTheDriver) {
            if (!NoPedOrNoHp(partner)) {
                return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
            }
            m_Cop->m_isTheDriver = true; // 0x68e906
            m_Cop->SetPartner(nullptr);
        }
        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
    }
    case TASK_SIMPLE_GANG_DRIVEBY: // 0x68e83e
        return CreateSubTask(
            m_Cop->m_isTheDriver
                ? TASK_COMPLEX_CAR_DRIVE_MISSION
                : TASK_SIMPLE_CAR_DRIVE,
            ped
        );
    case TASK_SIMPLE_CAR_DRIVE:
    case TASK_COMPLEX_CAR_DRIVE_MISSION: // 0x68e936
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(tt);
    }
}

// 0x68DC60
CTask* CTaskComplexKillCriminal::CreateFirstSubTask(CPed* ped) {
    if (!m_Criminal || m_Criminal->IsPlayer()) {
        return nullptr;
    }
    if (FindPlayerPed()->GetWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN || !g_LoadMonitor.IsAmbientCrimeEnabled()) { // 0x68dc96
        return nullptr;
    }
    if (!ped->IsCop()) {
        return nullptr;
    }
    if (const auto* const criminalVeh = m_Criminal->m_pVehicle) { // 0x68dcdb | NOTE: Also if the criminal isn't in it (anymore)
        if (criminalVeh->IsDriverAPlayer() || rng::any_of(criminalVeh->GetPassengers(), IsPlayer)) {
            return nullptr;
        }
    }

    m_Cop = ped->AsCop();

    if (m_Randomize && !m_Criminal->bWantedByPolice) { // 0x68dd63
        const auto* const criminalVeh = m_Criminal->GetVehicleIfInOne();
        if (!criminalVeh || !criminalVeh->vehicleFlags.bMadDriver || CGeneral::GetRandomNumberInRange(0, 3) != 0) {
            return nullptr;
        }
    }

    m_Cop->AddCriminalToKill(m_Criminal);
    if (m_Criminal->bInVehicle) {
        m_Criminal->GetIntelligence()->SetPedDecisionMakerType((int32)eDecisionMakerType::PED_EMPTY);
    }

    const auto AddHateEvent = [](CPed* to, CPed* hated, eTaskType response) {
        CEventAcquaintancePedHate event{ hated, response };
        to->GetEventGroup().Add(&event);
    };

    // 0x68e2de
    const auto Finish = [&, this](CTask* subTask) {
        if (ped->m_pVehicle && m_Cop->m_isTheDriver) {
            const auto& ap    = ped->m_pVehicle->m_autoPilot;
            m_OrigDrivingMode = ap.m_nCarDrivingStyle;
            m_OrigMission     = ap.m_nCarMission;
            m_OrigCruiseSpeed = ap.m_nCruiseSpeed;
            m_IsSetUp         = true;
        }

        if (const auto* const criminalVeh = m_Criminal->m_pVehicle; !criminalVeh) { // 0x68e403
            if (CGeneral::GetRandomNumber() & 1) {
                GivePistol(m_Criminal);
                AddHateEvent(m_Criminal, ped, TASK_COMPLEX_KILL_PED_ON_FOOT);
            }
        } else if (criminalVeh->vehicleFlags.bMadDriver) { // 0x68e331
            if (criminalVeh->m_nNumPassengers == 0) {
                if (CGeneral::GetRandomNumber() & 1) { // 0x68e3d9
                    GivePistol(m_Criminal);
                }
            } else {
                GivePistol(m_Criminal);
                for (auto* const passenger : criminalVeh->GetPassengers()) {
                    if (passenger) {
                        GivePistol(passenger);
                        m_Cop->AddCriminalToKill(passenger);
                    }
                }
            }
        }

        ped->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS() + NEEDED_FOR_MS; // 0x68e471
        ped->bCullExtraFarAway        = true;
        ped->m_fRemovalDistMultiplier = 0.3f;
        if (auto* const veh = ped->m_pVehicle) {
            veh->m_nExtendedRemovalRange                   = 255;
            veh->vehicleFlags.bNeverUseSmallerRemovalRange = true;
            if (ped->bInVehicle) {
                veh->vehicleFlags.bSirenOrAlarm = true;
            }
        }

        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_25, ped);

        return subTask;
    };

    if (!m_Cop->IsInVehicle()) { // 0x68e273
        if (NoPedOrNoHp(m_Cop->m_pCopPartner)) {
            m_Cop->m_isTheDriver = true;
        }
        return Finish(CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped));
    }

    auto* const veh = m_Cop->m_pVehicle;
    if (auto* const driver = veh->GetDriver(); driver == ped) { // 0x68de09
        m_Cop->m_isTheDriver = true;
        if (!m_Cop->m_pCopPartner) {
            for (auto* const passenger : veh->GetPassengers()) {
                if (passenger && passenger->IsCop()) {
                    m_Cop->SetPartner(passenger->AsCop());
                    break;
                }
            }
        }
        if (auto* const partner = m_Cop->m_pCopPartner) { // 0x68de87
            AddHateEvent(partner, m_Criminal, TASK_COMPLEX_KILL_CRIMINAL);
        }
    } else { // 0x68def3
        if (driver && driver->IsCop()) {
            m_Cop->SetPartner(driver->AsCop());
            driver->AsCop()->m_isTheDriver = true;
        }
        auto* const partner = m_Cop->m_pCopPartner;
        if (!partner) { // 0x68dfcd
            m_Cop->m_isTheDriver = true;
            return Finish(CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped));
        }
        if (partner->m_isTheDriver) { // 0x68df2d
            const auto* const tPartnerKillCriminal = partner->GetTaskManager().Find<CTaskComplexKillCriminal>(false);
            if (!tPartnerKillCriminal || tPartnerKillCriminal->m_Criminal != m_Criminal) { // 0x68df75
                AddHateEvent(partner, m_Criminal, TASK_COMPLEX_KILL_CRIMINAL);
                return nullptr;
            }
            m_Cop->m_isTheDriver     = false;
            m_Cop->bDontDragMeOutCar = true;
        }
    }

    if (!m_Criminal->IsInVehicle()) { // 0x68e225
        return Finish(CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped));
    }
    return Finish(CreateSubTask( // 0x68e056
        m_Cop->m_isTheDriver ? TASK_COMPLEX_CAR_DRIVE_MISSION : TASK_SIMPLE_CAR_DRIVE,
        ped
    ));
}

// 0x68E950
CTask* CTaskComplexKillCriminal::ControlSubTask(CPed* ped) {
    if (m_Criminal && !m_Criminal->CanBeCriminal()) {
        return nullptr;
    }
    if (FindPlayerPed()->GetWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN) { // 0x68e9c4
        if (FindPlayerWanted()->CanCopJoinPursuit(ped->AsCop()) && m_pSubTask->MakeAbortable(ped)) {
            return nullptr;
        }
    }
    if (!g_LoadMonitor.IsAmbientCrimeEnabled()) {
        return nullptr;
    }

    auto* const partner = m_Cop->m_pCopPartner; // NOTE: Also used after `SetPartner(nullptr)` below (it's dead then)
    auto        tt      = TASK_NONE;
    if (!m_Cop->m_isTheDriver && NoPedOrNoHp(partner)) { // 0x68ea4d
        m_Cop->m_isTheDriver = true;
        m_Cop->SetPartner(nullptr);
        if (ped->IsInVehicle()) {
            tt = TASK_COMPLEX_LEAVE_CAR;
        }
    }

    if (ped->m_fHealth <= 0.f) { // 0x68ea8f
        tt = TASK_FINISHED;
    } else if (NoPedOrNoHp(m_Criminal)) { // 0x68eaa6
        tt = GetSubTaskTypeWithoutCriminal(ped, partner, tt);
    } else {
        if (!m_Cop->m_isTheDriver) { // 0x68eac8
            tt = GetSubTaskTypeAsPassenger(ped, partner);
        } else { // 0x68ebb4
            if (ped->IsInVehicle()) {
                ped->m_pVehicle->vehicleFlags.bSirenOrAlarm = true;
            }
            tt = m_Criminal->IsInVehicle()
                ? GetSubTaskTypeForCriminalInCar(ped, partner, tt)
                : GetSubTaskTypeForCriminalOnFoot(ped, partner, tt);
        }
        tt = ProcessBikeDriveBy(ped, tt);
    }

    MakeCriminalPassengersDoDriveBy(ped);

    if (tt == TASK_NONE || !m_pSubTask->MakeAbortable(ped)) { // 0x68f691
        return m_pSubTask;
    }
    return CreateSubTask(tt, ped);
}

// NOTSA - Code at 0x68f476 - No criminal (alive) is left
eTaskType CTaskComplexKillCriminal::GetSubTaskTypeWithoutCriminal(CPed* ped, const CCopPed* partner, eTaskType tt) {
    const auto subTaskType = m_pSubTask->GetTaskType();
    if (subTaskType == TASK_COMPLEX_KILL_PED_ON_FOOT) {
        return tt;
    }
    if (auto* const next = FindNextCriminalToKill(ped, true); next && ChangeTarget(next)) {
        return tt;
    }
    if (subTaskType == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER || subTaskType == TASK_COMPLEX_ENTER_CAR_AS_DRIVER) {
        return tt;
    }
    if (!ped->bInVehicle) { // 0x68f4c9
        if (!ped->m_pVehicle || m_CantGetInCar) {
            return TASK_FINISHED;
        }
        return m_Cop->m_isTheDriver
            ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER
            : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
    }
    if (subTaskType == TASK_SIMPLE_CAR_DRIVE && !NoPedOrNoHp(partner) && !partner->bInVehicle) { // 0x68f506 - Wait for the partner to get in
        return tt;
    }
    return TASK_FINISHED;
}

// NOTSA - Code at 0x68ead9 - The partner (alive) is the driver
eTaskType CTaskComplexKillCriminal::GetSubTaskTypeAsPassenger(CPed* ped, CCopPed* partner) const {
    if (!partner->bInVehicle && !partner->GetTaskManager().Has<TASK_COMPLEX_ENTER_CAR_AS_DRIVER>(false)) {
        return TASK_COMPLEX_KILL_PED_ON_FOOT;
    }
    if (!ped->m_pVehicle || m_CantGetInCar) { // 0x68eb01
        return TASK_COMPLEX_KILL_PED_ON_FOOT;
    }
    if (!ped->bInVehicle) {
        return TASK_COMPLEX_ENTER_CAR_AS_PASSENGER;
    }
    if (partner->bInVehicle && (m_Criminal->GetPosition() - ped->m_pVehicle->GetPosition()).Magnitude() < DRIVEBY_RANGE) { // 0x68eb36 | NOTE: 3D
        return TASK_SIMPLE_GANG_DRIVEBY;
    }
    return TASK_SIMPLE_CAR_DRIVE;
}

// NOTSA - Code at 0x68ebf1 - We're the driver, the criminal is in a car
eTaskType CTaskComplexKillCriminal::GetSubTaskTypeForCriminalInCar(CPed* ped, const CCopPed* partner, eTaskType tt) {
    const auto* const criminalVeh   = m_Criminal->m_pVehicle;
    const auto        subTaskType   = m_pSubTask->GetTaskType();
    const auto        GetDistSqFrom = [this](const CEntity* e) {
        return (m_Criminal->GetPosition() - e->GetPosition()).SquaredMagnitude(); // NOTE: 3D
    };

    // 0x68ef4a - The criminals get out if their car is about to blow up, or if a cop on foot has reached it
    const auto Finish = [&, this](eTaskType finalType, bool isCopAtCar = false) {
        if (isCopAtCar || criminalVeh->GetHealth() < CRIMINAL_CAR_MIN_HEALTH) {
            MakeCriminalsLeaveCar(ped);
        }
        return finalType;
    };

    if (ped->IsInVehicle()) { // 0x68ec28
        switch (subTaskType) {
        case TASK_COMPLEX_CAR_DRIVE_MISSION:
        case TASK_SIMPLE_GANG_DRIVEBY:       { // 0x68ec8c
            if (criminalVeh->GetMoveSpeed().Magnitude() >= 0.12f) {
                m_TimeToGetOutOfCar = 1.f;
                return Finish(tt);
            }
            m_TimeToGetOutOfCar -= CTimer::GetTimeStep() * 0.02f;
            if (m_TimeToGetOutOfCar > 0.f || m_Criminal->GetEventHandler().GetCurrentEventType() == EVENT_ACQUAINTANCE_PED_HATE) {
                return Finish(tt);
            }
            if ((criminalVeh->GetPosition() - ped->m_pVehicle->GetPosition()).SquaredMagnitude() >= sq(LEAVE_CAR_RANGE)) { // 0x68ece8 | NOTE: 3D
                return Finish(tt);
            }
            if (auto* const next = FindNextCriminalToKill(ped, false)) {
                ChangeTarget(next);
            }
            return Finish(TASK_COMPLEX_KILL_PED_ON_FOOT);
        }
        case TASK_SIMPLE_CAR_DRIVE: // 0x68ec59
            return Finish(
                NoPedOrNoHp(partner) || partner->bInVehicle
                    ? TASK_COMPLEX_CAR_DRIVE_MISSION
                    : tt
            );
        default:
            return Finish(tt);
        }
    }

    if (criminalVeh->GetMoveSpeed().Magnitude() < 0.2f) { // 0x68ed83
        if (GetDistSqFrom(ped) < sq(6.f)) {
            return Finish(tt, true);
        }
        return Finish(tt, partner && !partner->bInVehicle && GetDistSqFrom(partner) < sq(6.f)); // 0x68edf5
    }

    if (!ped->m_pVehicle || m_CantGetInCar || subTaskType == TASK_COMPLEX_ENTER_CAR_AS_DRIVER || subTaskType == TASK_COMPLEX_ENTER_CAR_AS_PASSENGER) { // 0x68ee6a
        return Finish(tt);
    }
    // NOTE: The speed check is always true here (see above), and so is `m_isTheDriver`
    if (criminalVeh->GetMoveSpeed().Magnitude() >= 0.2f || (criminalVeh->GetPosition() - ped->m_pVehicle->GetPosition()).SquaredMagnitude() > sq(20.f)) { // 0x68eea9 | NOTE: 3D
        return Finish(
            m_Cop->m_isTheDriver
                ? TASK_COMPLEX_ENTER_CAR_AS_DRIVER
                : TASK_COMPLEX_ENTER_CAR_AS_PASSENGER
        );
    }
    return Finish(tt);
}

// NOTSA - Code at 0x68f0a9 - We're the driver, the criminal is on foot
eTaskType CTaskComplexKillCriminal::GetSubTaskTypeForCriminalOnFoot(CPed* ped, const CCopPed* partner, eTaskType tt) {
    if (ped->IsInVehicle()) {
        const auto* const veh    = ped->m_pVehicle;
        const float       distSq = (m_Criminal->GetPosition() - veh->GetPosition()).SquaredMagnitude(); // NOTE: 3D

        if (distSq > sq(LEAVE_CAR_RANGE)) { // 0x68f137
            return NoPedOrNoHp(partner) || partner->bInVehicle
                ? TASK_COMPLEX_CAR_DRIVE_MISSION
                : tt;
        }
        switch (veh->m_autoPilot.m_nCarMission) { // 0x68f174
        case MISSION_KILLPED_CLOSE:
        case MISSION_KILLPED_FARAWAY: // NOTE: A squared distance is compared to 5 on a bike
            return distSq < (veh->IsBike() ? 5.f : sq(4.f))
                ? TASK_COMPLEX_KILL_PED_ON_FOOT
                : tt;
        default:
            return TASK_COMPLEX_KILL_PED_ON_FOOT;
        }
    }

    if (m_CantGetInCar || !ped->m_pVehicle) { // 0x68f1b1
        return tt;
    }
    // NOTE: 3D, and a squared distance is compared to 250 for the car
    if ((m_Criminal->GetPosition() - ped->GetPosition()).SquaredMagnitude() <= sq(ON_FOOT_RANGE) && (ped->m_pVehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude() <= 250.f) {
        return tt;
    }
    if (auto* const next = FindNextCriminalToKill(ped, false); next && ChangeTarget(next)) { // 0x68f277
        return TASK_COMPLEX_KILL_PED_ON_FOOT;
    }
    return TASK_COMPLEX_ENTER_CAR_AS_DRIVER;
}

// NOTSA - Code at 0x68f2a1 - A cop on a bike shoots while riding, so the bike is steered from here
eTaskType CTaskComplexKillCriminal::ProcessBikeDriveBy(CPed* ped, eTaskType tt) const {
    auto* const veh = ped->m_pVehicle;
    if (!veh || !veh->IsBike()) {
        return tt;
    }

    const auto subTaskType = m_pSubTask->GetTaskType();
    const bool startDriveBy =
        subTaskType == TASK_COMPLEX_CAR_DRIVE_MISSION
        && (tt == TASK_NONE || tt == TASK_COMPLEX_CAR_DRIVE_MISSION)
        && (m_Criminal->GetPosition() - veh->GetPosition()).Magnitude() < DRIVEBY_RANGE; // NOTE: 3D

    if (!startDriveBy && tt != TASK_SIMPLE_GANG_DRIVEBY && subTaskType != TASK_SIMPLE_GANG_DRIVEBY) { // 0x68f343
        return tt;
    }

    auto& ap = veh->m_autoPilot;
    veh->SetStatus(STATUS_PHYSICS);
    if (auto* const criminalVeh = m_Criminal->GetVehicleIfInOne()) { // 0x68f37e
        ap.SetCarMission(MISSION_FOLLOWCAR_CLOSE);
        ap.m_nCruiseSpeed = (uint8)(criminalVeh->m_autoPilot.m_nCruiseSpeed + (int32)CHASE_CRUISE_SPEED_BONUS); // NOTE: Wraps around
        ap.m_TargetEntity = criminalVeh;
    } else { // 0x68f40e
        ap.SetCarMission(MISSION_KILLPED_CLOSE);
        ap.SetCruiseSpeed(KILL_PED_CRUISE_SPEED);
        ap.m_TargetEntity = reinterpret_cast<CVehicle*>(m_Criminal.Get());
    }
    ap.m_speed = (float)ap.m_nCruiseSpeed;
    ap.SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
    if (notsa::bugfixes::GenericUB) { // NOTE/BUG: Originally the reference isn't registered, the pointer is left dangling once the target is deleted
        CEntity::SafeRegisterRef(ap.m_TargetEntity);
    }

    return startDriveBy
        ? TASK_SIMPLE_GANG_DRIVEBY
        : tt;
}

// NOTSA - Code at 0x68ef56
void CTaskComplexKillCriminal::MakeCriminalsLeaveCar(CPed* ped) const {
    const auto* const criminalVeh = m_Criminal->m_pVehicle;

    // NOTE: Originally `TASK_COMPLEX_SMART_FLEE_ENTITY` is used if the address of the criminal's active weapon is null (Never)
    CEventAcquaintancePedHate event{
        ped,
        criminalVeh->GetHealth() < CRIMINAL_CAR_MIN_HEALTH
            ? TASK_COMPLEX_LEAVE_CAR
            : TASK_COMPLEX_KILL_PED_AND_REENTER_CAR
    };

    m_Criminal->GetEventGroup().Add(&event);
    m_Criminal->SetPedDefaultDecisionMaker();

    if (auto* const driver = criminalVeh->GetDriver(); driver && driver != m_Criminal && !driver->IsPlayer()) { // 0x68efc9 | NOTE: Keeps its decision maker
        driver->GetEventGroup().Add(&event);
    }

    for (auto* const passenger : criminalVeh->GetPassengers()) { // 0x68f00c
        if (passenger && passenger != m_Criminal && !passenger->IsPlayer()) {
            passenger->GetEventGroup().Add(&event);
            passenger->SetPedDefaultDecisionMaker();
        }
    }
}

// NOTSA - Code at 0x68f53c
void CTaskComplexKillCriminal::MakeCriminalPassengersDoDriveBy(CPed* ped) const {
    if (!m_Criminal || !m_Criminal->IsInVehicle()) {
        return;
    }
    const auto* const criminalVeh = m_Criminal->m_pVehicle;
    if (criminalVeh->m_nNumPassengers == 0 || (criminalVeh->GetPosition() - ped->GetPosition()).Magnitude() >= DRIVEBY_RANGE) { // NOTE: 3D
        return;
    }
    for (auto* const passenger : criminalVeh->GetPassengers()) { // 0x68f5d1
        if (passenger && passenger->bInVehicle && !passenger->GetTaskManager().Has<TASK_SIMPLE_GANG_DRIVEBY>(false)) {
            passenger->GetTaskManager().SetTask(
                new CTaskSimpleGangDriveBy{ ped, nullptr, DRIVEBY_ABORT_RANGE, DRIVEBY_FREQUENCY_PERC, eDrivebyStyle::AI_ALL_DIRN, false },
                TASK_PRIMARY_PRIMARY
            );
        }
    }
}

void CTaskComplexKillCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillCriminal, 0x870a00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x68BE70, "", {}, CPed*, bool);
    RH_ScopedDestructorInstall(0x68BF30);

    RH_ScopedInstall(CreateSubTask, 0x68C050);
    RH_ScopedInstall(FindNextCriminalToKill, 0x68C3C0);
    RH_ScopedInstall(ChangeTarget, 0x68C6E0);

    RH_ScopedVMTInstall(Clone, 0x68CE50);
    RH_ScopedVMTInstall(GetTaskType, 0x68BF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x68DAD0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68E4F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68DC60);
    RH_ScopedVMTInstall(ControlSubTask, 0x68E950);
}
