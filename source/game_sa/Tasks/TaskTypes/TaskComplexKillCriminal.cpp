#include "StdInc.h"

#include "TaskComplexKillCriminal.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexCarDriveMission.h"


void CTaskComplexKillCriminal::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillCriminal, 0x870a00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68BE70);
    RH_ScopedInstall(Destructor, 0x68BF30);

    RH_ScopedInstall(CreateSubTask, 0x68C050, {.reversed = false});
    RH_ScopedInstall(FindNextCriminalToKill, 0x68C3C0, {.reversed = false});
    RH_ScopedInstall(ChangeTarget, 0x68C6E0, {.reversed = false});

    RH_ScopedVMTInstall(Clone, 0x68CE50, {.reversed = false});
    RH_ScopedVMTInstall(GetTaskType, 0x68BF20, {.reversed = false});
    RH_ScopedVMTInstall(MakeAbortable, 0x68DAD0, {.reversed = false});
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68E4F0, {.reversed = false});
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68DC60, {.reversed = false});
    RH_ScopedVMTInstall(ControlSubTask, 0x68E950, {.reversed = false});
}

// 0x68BE70
CTaskComplexKillCriminal::CTaskComplexKillCriminal(CPed* criminal, bool randomize) :
    m_randomize{randomize},
    m_criminal{criminal}
{
    if ([&, this]{
        if (!m_criminal) {
            return false;
        }
        if (m_criminal->IsCreatedByMission()) {
            return true;
        }
        switch (m_criminal->m_nPedType) {
        case PED_TYPE_MEDIC:
        case PED_TYPE_FIREMAN:
        case PED_TYPE_MISSION1:
            return true;
        }
        return false;
    }()) {
        m_criminal = nullptr;
    }
    CEntity::SafeRegisterRef(m_criminal);
}

// NOTSA (For 0x68CE50)
CTaskComplexKillCriminal::CTaskComplexKillCriminal(const CTaskComplexKillCriminal&) :
    CTaskComplexKillCriminal{m_criminal, false}
{
}

// 0x68BF30
CTaskComplexKillCriminal::~CTaskComplexKillCriminal() {
    CEntity::SafeCleanUpRef(m_Criminal);

    if (m_Cop) {
        CEntity::SafeCleanUpRef(m_Cop);

        m_Cop->m_nTimeTillWeNeedThisPed = CTimer::GetTimeInMS();
        m_Cop->bCullExtraFarAway = false;
        m_Cop->m_fRemovalDistMultiplier = 1.f;
        if (const auto v = m_Cop->m_pVehicle) {
            v->m_nExtendedRemovalRange = false;
            v->vehicleFlags.bNeverUseSmallerRemovalRange = false;
            if (v->IsDriver(m_Cop)) {
                const auto ap = &v->m_autoPilot;
                ap->SetCarMission(MISSION_CRUISE);
                ap->SetDrivingStyle(DRIVING_STYLE_AVOID_CARS);
                ap->SetCruiseSpeed(10);
                if (v->GetStatus() != STATUS_SIMPLE) {
                    CCarCtrl::JoinCarWithRoadSystem(v);
                }
                veh->vehicleFlags.bSirenOrAlarm = false;
            }
        }
        CEntity::CleanUpOldReference(m_cop);
    }
}

// 0x68C050
CTask* CTaskComplexKillCriminal::CreateSubTask(eTaskType tt, CPed* ped, bool force) {
    if (!force && m_pSubTask && m_pSubTask->GetTaskType() == tt) {
        return m_pSubTask;        
    }

    switch (tt) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        ped->SetCurrentWeapon(WEAPON_PISTOL);
        return new CTaskComplexKillPedOnFoot{ m_criminal };
    }
    case TASK_FINISHED: {
        if (m_criminal) {
            m_criminal->SetPedDefaultDecisionMaker();
        }
        return nullptr;
    }
    case TASK_SIMPLE_GANG_DRIVEBY:
        return new CTaskSimpleGangDriveBy{
            m_criminal,
            nullptr,
            70.f,
            70,
            eDrivebyStyle::AI_ALL_DIRN,
            false
        };
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
        return new CTaskComplexEnterCarAsPassenger{ ped->m_pVehicle };
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver{ ped->m_pVehicle };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 0, 0, true, false };
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive{ ped->m_pVehicle };
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        const auto oveh = ped->m_pVehicle; // (o)ur (veh)icle
        if (!oveh) {
            return nullptr;
        }

        const auto CreateDriveMission = [&, this](eCarMission mission, float cruiseSpeed, CEntity* traget) {
            return new CTaskComplexCarDriveMission{
                ped->m_pVehicle,
                traget,
                mission,
                DRIVING_STYLE_AVOID_CARS,
                cruiseSpeed
            };
        };

        if (const auto cveh = m_criminal->GetVehicleIfInOne()) {
            return CreateDriveMission(
                oveh->IsBike()
                    ? MISSION_35
                    : MISSION_BLOCKCAR_CLOSE,
                (float)m_criminal->m_pVehicle->m_autoPilot.m_nCruiseSpeed + 10.f,
                cveh
            );
        } else {
            return CreateDriveMission(MISSION_37, 20.f, m_criminal);
        }
    }
    }
}

// 0x68C3C0
CPed* CTaskComplexKillCriminal::FindNextCriminalToKill(CPed* ped, bool any) {
    const auto Filter = [this](CPed* ped) {
        return ped && ped->m_fHealth > 0.f;
    };
    const auto [closest, distSq] = notsa::SpatialQuery(
        m_cop->m_apCriminalsToKill | rng::views::filter(Filter),
        m_cop->GetPosition(),
        m_criminal,
        !any && Filter(m_criminal)
            ? m_criminal
            : nullptr
    );
    return closest;
}

// 0x68C6E0
bool CTaskComplexKillCriminal::ChangeTarget(CPed* newTarget) { // TODO: Figure out if `newTarget` is actually the new target, or it's just he ped that is the owner of this task
    if (newTarget == m_criminal) {
        return true;
    }

    if (!newTarget || newTarget->m_fHealth <= 0.f) {
        return false;
    }

    if (m_criminal && m_criminal->bInVehicle) {
        return false;
    }

    if (CTask::DynCast<CTaskComplexKillPedOnFoot>(m_pSubTask) && !m_pSubTask->MakeAbortable(newTarget, ABORT_PRIORITY_URGENT, nullptr)) {
        return false;
    }

    if (!notsa::contains(m_cop->m_apCriminalsToKill, newTarget)) { // 0x68c760
        return false;
    }

    CEntity::ChangeEntityReference(m_criminal, newTarget);

    // Propagate change to partner
    if (const auto partner = m_cop->m_pCopPartner) {
        if (partner->bInVehicle) {
            if (const auto partnersTask = partner->GetIntelligence()->FindTaskByType(TASK_COMPLEX_KILL_CRIMINAL)) {
                static_cast<CTaskComplexKillCriminal*>(partnersTask)->ChangeTarget(newTarget);
            }
        }
    }

    m_finished = false;

    return true;
}

// 0x68DAD0
bool CTaskComplexKillCriminal::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    const auto SubTaskMakeAbortable = [&, this] {
        return m_pSubTask->MakeAbortable(ped, priority, event);
    };

    if (!event) {
        return SubTaskMakeAbortable();
    }

    if (m_criminal && event->GetSourceEntity() == m_criminal) {
        if (notsa::contains())
    }
}

// 0x68E4F0
CTask* CTaskComplexKillCriminal::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68E4F0, CTaskComplexKillCriminal*, CPed*>(this, ped);
}

// 0x68DC60
CTask* CTaskComplexKillCriminal::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68DC60, CTaskComplexKillCriminal*, CPed*>(this, ped);
}

// 0x68E950
CTask* CTaskComplexKillCriminal::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x68E950, CTaskComplexKillCriminal*, CPed*>(this, ped);
}
