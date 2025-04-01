#include "StdInc.h"
#include "InterestingEvents.h"
#include "TaskComplexRoadRage.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleShakeFist.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexSeekEntityStandard.h"
#include "TaskComplexDestroyCar.h"
#include "TaskComplexKillPedOnFoot.h"

void CTaskComplexRoadRage::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexRoadRage, 0x86d9f0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6220A0);
    RH_ScopedInstall(Destructor, 0x622110);

    RH_ScopedInstall(CreateSubTask, 0x629080);

    RH_ScopedVMTInstall(Clone, 0x623670);
    RH_ScopedVMTInstall(GetTaskType, 0x622100);
    RH_ScopedVMTInstall(MakeAbortable, 0x622170);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62E270);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E170);
    RH_ScopedVMTInstall(ControlSubTask, 0x62E3F0);
}

// 0x6220A0
CTaskComplexRoadRage::CTaskComplexRoadRage(CPed* rageWith) :
    m_rageWith{rageWith}
{
    CEntity::SafeRegisterRef(m_rageWith);
}

CTaskComplexRoadRage::CTaskComplexRoadRage(const CTaskComplexRoadRage& o) :
    CTaskComplexRoadRage{o.m_rageWith}
{
}

// 0x622110
CTaskComplexRoadRage::~CTaskComplexRoadRage() {
    CEntity::SafeCleanUpRef(m_rageWith);
}

// 0x629080
CTask* CTaskComplexRoadRage::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        return new CTaskComplexTurnToFaceEntityOrCoord{ m_rageWith };
    case TASK_COMPLEX_LEAVE_CAR:
        return new CTaskComplexLeaveCar{ ped->m_pVehicle,eTargetDoor::TARGET_DOOR_FRONT_LEFT, 0, true, false };
    case TASK_SIMPLE_SHAKE_FIST:
        return new CTaskSimpleShakeFist{};
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        return new CTaskComplexEnterCarAsDriver{ ped->m_pVehicle };
    case TASK_COMPLEX_SEEK_ENTITY:
        return new CTaskComplexSeekEntityStandard{
            ped->m_pVehicle,
            20'000,
            1'000,
            m_rageWith->m_pVehicle->GetModelInfo()->GetColModel()->GetBoundRadius() + 1.f,
            2.f,
            2.f,
            true,
            true
        };
    case TASK_COMPLEX_DESTROY_CAR:
        return new CTaskComplexDestroyCar{ m_rageWith->m_pVehicle };
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return new CTaskComplexKillPedOnFoot{ m_rageWith, -1, 0, 0, 0, true };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x622170
bool CTaskComplexRoadRage::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x62E270
CTask* CTaskComplexRoadRage::CreateNextSubTask(CPed* ped) {
    if (!m_rageWith) {
        return nullptr;
    }
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        return CreateSubTask(TASK_SIMPLE_SHAKE_FIST, ped);
    case TASK_COMPLEX_DESTROY_CAR:
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return CreateSubTask(TASK_FINISHED, ped);
    case TASK_SIMPLE_SHAKE_FIST:
        return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
    case TASK_COMPLEX_LEAVE_CAR:
        return CreateSubTask(
            ped->bInVehicle // Ternary inverted
                ? TASK_FINISHED
                : TASK_COMPLEX_TURN_TO_FACE_ENTITY,
            ped
        ); 
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
        if (ped->m_pVehicle) {
            ped->m_pVehicle->m_autoPilot.SetCarMission(MISSION_CRUISE);
            ped->m_pVehicle->m_autoPilot.SetCruiseSpeed(10);
        }
        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY:
        return CreateSubTask(
            m_rageWith->IsInVehicle() && !ped->IsCurrentlyUnarmed()
                ? TASK_COMPLEX_DESTROY_CAR
                : TASK_COMPLEX_KILL_PED_ON_FOOT,
            ped
        );
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x62E170
CTask* CTaskComplexRoadRage::CreateFirstSubTask(CPed* ped) {
    if (!m_rageWith || m_rageWith->IsCop() || !ped->IsInVehicle()) {
        return nullptr;
    }

    // Maybe give ped a weapon
    if (ped->IsGangster()) { // Inverted
        ped->GiveDelayedWeapon(WEAPON_PISTOL, 2000u);
        ped->SetCurrentWeapon(WEAPON_PISTOL);
    } else if (CGeneral::DoCoinFlip()) {
        const auto wtype = CGeneral::RandomChoice(std::array{ WEAPON_BASEBALLBAT, WEAPON_GOLFCLUB, WEAPON_SHOVEL });
        ped->GiveDelayedWeapon(wtype, 1);
        ped->SetCurrentWeapon(wtype);     
    }

    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_18, ped);

    return CreateSubTask(m_rageWith->bInVehicle ? TASK_COMPLEX_LEAVE_CAR : TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
}

// 0x62E3F0
CTask* CTaskComplexRoadRage::ControlSubTask(CPed* ped) {
    if (!m_rageWith) {
        m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr);
        return m_pSubTask;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_DESTROY_CAR: { // Case positions swapped (to allow fallthru)
        if (!m_rageWith->bInVehicle) {
            return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
        }
        [[fallthrough]];
    }
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        if (ped->m_pVehicle && (ped->GetPosition() - m_rageWith->GetPosition()).SquaredMagnitude() <= sq(20.f)) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
        }
        break;
    }
    }

    return m_pSubTask;
}
