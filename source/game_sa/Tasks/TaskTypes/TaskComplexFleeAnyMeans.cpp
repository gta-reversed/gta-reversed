#include "StdInc.h"
#include "TaskComplexFleeAnyMeans.h"
#include "TaskComplexFleeShooting.h"
#include "TaskComplexWanderFlee.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskComplexGoToPointShooting.h"

void CTaskComplexFleeAnyMeans::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFleeAnyMeans, 0x86f7f0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65CC60);
    RH_ScopedInstall(Destructor, 0x65CD10);

    RH_ScopedInstall(CreateSubTask, 0x65D2E0);

    RH_ScopedVMTInstall(Clone, 0x65D030);
    RH_ScopedVMTInstall(GetTaskType, 0x65CD00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65D540);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65D730);
    RH_ScopedVMTInstall(ControlSubTask, 0x65D780);
}

// 0x65CC60
CTaskComplexFleeAnyMeans::CTaskComplexFleeAnyMeans(
    CEntity* entity,
    bool attackWhileFeeling,
    float safeDist,
    int32 fleeTime,
    int32 shootTime,
    int32 shootRecoverTime,
    float stealCarDist,
    int32 posChangePeriod,
    float posChangeTolerance
) :
    CTaskComplexSmartFleeEntity{ entity, false, safeDist, fleeTime, posChangePeriod, posChangeTolerance },
    m_minDistFromFleeEntityToStealCar{ stealCarDist },
    m_shootWhileFleeing{ attackWhileFeeling },
    m_shootRecoverTime{shootRecoverTime},
    m_shootTime{shootTime}
{
    /* Fleed entity is registered in SmartFlee already */
}

CTaskComplexFleeAnyMeans::CTaskComplexFleeAnyMeans(const CTaskComplexFleeAnyMeans& o) :
    CTaskComplexFleeAnyMeans{
        o.fleeFrom,
        o.m_shootWhileFleeing,
        o.m_safeDistance,
        o.m_time,
        o.m_shootTime,
        o.m_shootRecoverTime,
        o.m_minDistFromFleeEntityToStealCar,
        o.m_posCheckPeriod,
        o.m_posChangeTolerance
    }
{
}

// 0x65CD10
CTaskComplexFleeAnyMeans::~CTaskComplexFleeAnyMeans() {
#ifdef FIX_BUGS
    CEntity::SafeCleanUpRef(m_stealVeh);
#endif
    /* Fleed entity is claned up in SmartFlee already */
}

// 0x65D2E0
CTask* CTaskComplexFleeAnyMeans::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_FLEE_SHOOTING:
        return new CTaskComplexFleeShooting{
            fleeFrom,
            m_pedScream,
            m_safeDistance,
            m_time,
            m_shootTime,
            m_shootRecoverTime,
            m_posCheckPeriod,
            m_posChangeTolerance
        };
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderFlee{
            PEDMOVE_SPRINT,
            CGeneral::RandomNodeHeading()
        };
    case TASK_COMPLEX_STEAL_CAR:
        return new CTaskComplexStealCar{ m_stealVeh };
    case TASK_COMPLEX_CAR_DRIVE_MISSION:
        return new CTaskComplexCarDriveMission{
            ped->m_pVehicle,
            nullptr,
            MISSION_CRUISE,
            DRIVING_STYLE_PLOUGH_THROUGH,
            40.f
        };
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING:
        return new CTaskComplexGoToPointShooting{
            PEDMOVE_RUN,
            m_stealVeh->GetPosition(),
            m_stealVeh,
            {},
            m_stealVeh->GetModelInfo()->GetColModel()->GetBoundRadius() + 0.35f,
            4.f
        };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x65D540
CTask* CTaskComplexFleeAnyMeans::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_STEAL_CAR: {
        m_stealVeh = nullptr;
        [[fallthrough]];
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION:
        return ped->IsInVehicle()
            ? CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped)
            : CreateFirstSubTask(ped);
    case TASK_COMPLEX_GO_TO_POINT_SHOOTING:
        return m_stealVeh
            ? CreateSubTask(TASK_COMPLEX_STEAL_CAR, ped)
            : CreateFirstSubTask(ped);
    case TASK_COMPLEX_WANDER:
    case TASK_COMPLEX_FLEE_SHOOTING:
        return CreateSubTask(TASK_FINISHED, ped);
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x65D730
CTask* CTaskComplexFleeAnyMeans::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        return ped->IsInVehicle()
            ? TASK_COMPLEX_CAR_DRIVE_MISSION
            : m_shootWhileFleeing
                ? TASK_COMPLEX_FLEE_SHOOTING
                : TASK_COMPLEX_WANDER;
    }(), ped);
}

// 0x65D780
CTask* CTaskComplexFleeAnyMeans::ControlSubTask(CPed* ped) {
    if (!fleeFrom) {
        return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)
            ? nullptr
            : m_pSubTask;
    }

    // Moved here from below
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_FLEE_SHOOTING:
    case TASK_COMPLEX_WANDER:
        break;
    default:
        return m_pSubTask;
    }

    // Now, try stealing a car
    
    // Check if there's anything nearby
    const auto closestVeh = ped->GetIntelligence()->GetVehicleScanner().GetClosestVehicleInRange();
    if (!closestVeh) {
        return m_pSubTask;
    }

    const auto& pedPos = ped->GetPosition();

    // Check if we're far enough from the entity we're fleeing from to enter a car
    if (sq(m_minDistFromFleeEntityToStealCar) >= (pedPos - fleeFrom->GetPosition()).SquaredMagnitude()) {
        return m_pSubTask;
    }

    // Check if vehicle is in front of the ped ([-90, 90] deg)
    if (const auto pedToVeh = closestVeh->GetPosition() - pedPos; 0.f >= pedToVeh.Normalized().Dot(ped->GetForward())) {
        return m_pSubTask;
    }

    // Register it
    CEntity::ChangeEntityReference(m_stealVeh, closestVeh);

    // Add `VehicleDamageWeapon` all occupants of the stolen vehicle (if we're attacking)
    if (m_shootWhileFleeing) {
        const auto AddEventForVehicleOccupant = [&](CPed* occupant) {
            if (occupant && !occupant->IsPlayer()) {
                occupant->GetIntelligence()->m_eventGroup.Add(
                    CEventVehicleDamageWeapon{ closestVeh, ped, WEAPON_PISTOL }
                );
            }
        };
        AddEventForVehicleOccupant(closestVeh->m_pDriver);
        rng::for_each(closestVeh->GetPassengers(), AddEventForVehicleOccupant);
    } else if (!closestVeh->CanPedStepOutCar()) {
        return m_pSubTask;
    }

    // Finally, try aborting the current task (`COMPLEX_FLEE_SHOOTING` or `COMPLEX_WANDER`)
    if (m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) {
        return CreateSubTask(
            m_shootWhileFleeing
                ? TASK_COMPLEX_GO_TO_POINT_SHOOTING
                : TASK_COMPLEX_STEAL_CAR,
            ped
        );
    }

    return m_pSubTask;
}
