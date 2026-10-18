#include "StdInc.h"

#include "TaskComplexCarSlowBeDraggedOutAndStandUp.h"
#include "TaskComplexCarSlowBeDraggedOut.h"
#include "TaskComplexGetUpAndStandStill.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarSetPedOut.h"

void CTaskComplexCarSlowBeDraggedOutAndStandUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCarSlowBeDraggedOutAndStandUp, 0x86EF80, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648620);
    RH_ScopedInstall(Destructor, 0x648690);

    RH_ScopedInstall(CreateSubTask, 0x648710);

    RH_ScopedVMTInstall(Clone, 0x64A190);
    RH_ScopedVMTInstall(GetTaskType, 0x648680);
    RH_ScopedVMTInstall(MakeAbortable, 0x6486F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6488F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x648A10);
    RH_ScopedVMTInstall(ControlSubTask, 0x648B80);
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 door) :
    CTaskComplex() {
    m_Vehicle = vehicle;
    m_Door    = static_cast<eTargetDoor>(door);
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_LEAVE_CAR: {
        // Peds can't be dragged out of these vehicles, so they just get out (See `CTaskSimpleCarSlowDragPedOut::ProcessPed`)
        switch (m_Vehicle->GetAnimGroupId()) {
        case ANIM_GROUP_COACHCARANIMS:
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_RIGHT, 0, false, true };
        case ANIM_GROUP_TANKCARANIMS:
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_DRIVER, 0, false, true };
        case ANIM_GROUP_BUSCARANIMS: {
            if (m_Door == TARGET_DOOR_FRONT_RIGHT) {
                return new CTaskComplexLeaveCar{ ped->m_pVehicle, TARGET_DOOR_FRONT_RIGHT, 0, false, true };
            }
            break;
        }
        }
        return nullptr;
    }
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return new CTaskSimpleCarSetPedOut{ ped->m_pVehicle, m_Door, true };
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return new CTaskComplexGetUpAndStandStill{};
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return new CTaskComplexCarSlowBeDraggedOut{ m_Vehicle, m_Door, false };
    case TASK_FINISHED:
        return nullptr;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return priority == ABORT_PRIORITY_IMMEDIATE
        && m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT: {
        if (!ped->IsAlive() || ped->m_fHealth <= 0.0f) {
            CEventDeath event{ false };
            ped->GetEventGroup().Add(&event, false);
            return nullptr;
        }
        return new CTaskComplexGetUpAndStandStill{};
    }
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL: // The original calls `ped->IsPlayer()` here, but doesn't use the result
    default:
        return nullptr;
    }
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    if (ped == FindPlayerPed(0)) {
        if (ped->m_pVehicle) {
            ped->m_pVehicle->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    } else if (ped->m_pVehicle->IsDriver(ped)) {
        ped->SetRadioStation();
    }

    // Peds can't be dragged out of these vehicles, so they just get out (See `CreateSubTask`)
    if (ped->m_pVehicle) {
        switch (ped->m_pVehicle->GetAnimGroupId()) {
        case ANIM_GROUP_COACHCARANIMS:
        case ANIM_GROUP_TANKCARANIMS:
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        case ANIM_GROUP_BUSCARANIMS: {
            if (m_Door == TARGET_DOOR_FRONT_RIGHT) {
                return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
            }
            break;
        }
        }
    }

    // Play the sound of being jacked
    switch (ped->m_pVehicle->GetAnimGroupId()) {
    case ANIM_GROUP_STDCARAMIMS:
    case ANIM_GROUP_VANCARANIMS:
    case ANIM_GROUP_RUSTPLANEANIMS:
    case ANIM_GROUP_KARTCARANIMS:
    case ANIM_GROUP_CONVCARANIMS:
    case ANIM_GROUP_STDTALLCARAMIMS:
    case ANIM_GROUP_HOVERCARANIMS:
    case ANIM_GROUP_STDCARUPRIGHT: {
        if (m_Door == TARGET_DOOR_DRIVER) {
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_PUNCH);
        } else if (m_Door == TARGET_DOOR_FRONT_RIGHT) {
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_HEAD_BANG);
        }
        break;
    }
    case ANIM_GROUP_LOWCARAMIMS:
        ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_KICK);
        break;
    case ANIM_GROUP_DOZERCARANIMS:
        ped->GetAE().AddAudioEvent(AE_PED_JACKED_DOZER);
        break;
    }

    return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
