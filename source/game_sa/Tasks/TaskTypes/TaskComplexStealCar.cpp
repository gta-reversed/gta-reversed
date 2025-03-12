#include "StdInc.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexCarDriveMission.h"
#include "TaskSimplePause.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleSetPedAsAutoDriver.h"

void CTaskComplexStealCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexStealCar, 0x86e7fc, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63B7D0);
    RH_ScopedInstall(Destructor, 0x63B850);

    RH_ScopedInstall(CreateSubTask, 0x640F30);

    RH_ScopedVMTInstall(Clone, 0x63D980);
    RH_ScopedVMTInstall(GetTaskType, 0x63B840);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6440B0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x644210);
    RH_ScopedVMTInstall(ControlSubTask, 0x644250);
}

// 0x63B7D0
CTaskComplexStealCar::CTaskComplexStealCar(CVehicle* toSteal) :
    m_veh{toSteal}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskComplexStealCar::CTaskComplexStealCar(const CTaskComplexStealCar& o) :
    CTaskComplexStealCar{ o.m_veh }
{
}

// 0x63B850
CTaskComplexStealCar::~CTaskComplexStealCar() {
    CEntity::SafeCleanUpRef(m_veh);
    CEntity::SafeCleanUpRef(m_originalriver);
}

// 0x640F30
CTask* CTaskComplexStealCar::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE:
        return new CTaskComplexCarDriveMissionFleeScene{ m_veh };
    case TASK_SIMPLE_PAUSE:
        return new CTaskSimplePause{0};
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
        CEntity::SafeCleanUpRef(m_originalriver);

        if (m_veh) {
            m_originalriver = m_veh->m_pDriver;
            CEntity::SafeRegisterRef(m_originalriver);
        }

        return new CTaskComplexEnterCarAsDriver{ m_veh };
    }
    case TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER:
        return new CTaskSimpleSetPedAsAutoDriver{ m_veh };
    case TASK_FINISHED: {
        if (!ped->bInVehicle) {
            ped->m_bUsesCollision = true;
        }
        return nullptr;
    }
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6440B0
CTask* CTaskComplexStealCar::CreateNextSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (!m_veh) {
            return TASK_FINISHED;
        }

        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
            if (ped->IsInVehicle()) {
                if (ped->m_nPedType == PED_TYPE_CRIMINAL) {
                    ped->Say(CTX_GLOBAL_MUGGING);
                }
                if (m_originalriver && !m_originalriver->IsPlayer()) {
                    m_originalriver->Say(CTX_GLOBAL_MUGGED);
                }
                return TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER;
            }
            return TASK_FINISHED;
        }
        case TASK_SIMPLE_SET_PED_AS_AUTO_DRIVER: {
            if (ped->m_nPedType == PED_TYPE_CRIMINAL) {
                ped->bWantedByPolice = true;
                ped->m_pVehicle->vehicleFlags.bMadDriver = true;
            }
            return TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE;
        }
        case TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE: {
            if (ped->bInVehicle) {
                ped->m_pVehicle->m_autoPilot.SetCarMission(MISSION_CRUISE, 10);
            }
            return TASK_FINISHED;
        }
        case TASK_SIMPLE_PAUSE:
            return TASK_FINISHED;
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x644210
CTask* CTaskComplexStealCar::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([this, ped] {
        if (m_veh) { // Inverted
            m_enterTimer.Start(10'000);
            return TASK_COMPLEX_ENTER_CAR_AS_DRIVER;
        }
        return TASK_FINISHED;
    }(), ped);
}

// 0x644250
CTask* CTaskComplexStealCar::ControlSubTask(CPed* ped) {
    if (const auto enter = CTask::DynCast<CTaskComplexEnterCarAsDriver>(m_pSubTask)) {
        if (m_enterTimer.IsOutOfTime()) {
            if (m_pSubTask->MakeAbortable(ped)) {
                return CreateSubTask(TASK_SIMPLE_PAUSE, ped);
            }
        }
    }
    return m_pSubTask;
}
