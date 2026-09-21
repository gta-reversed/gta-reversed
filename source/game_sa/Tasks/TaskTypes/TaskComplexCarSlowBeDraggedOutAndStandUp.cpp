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
    RH_ScopedInstall(Clone, 0x64A190);
    RH_ScopedInstall(GetTaskType, 0x648680);
    RH_ScopedInstall(MakeAbortable, 0x6486F0);
    RH_ScopedInstall(CreateNextSubTask, 0x6488F0);
    RH_ScopedInstall(CreateFirstSubTask, 0x648A10);
    RH_ScopedInstall(ControlSubTask, 0x648B80);
}

// 0x648620
CTaskComplexCarSlowBeDraggedOutAndStandUp::CTaskComplexCarSlowBeDraggedOutAndStandUp(CVehicle* vehicle, int32 a3) : CTaskComplex() {
    m_Vehicle = vehicle;
    dword10 = a3;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648690
CTaskComplexCarSlowBeDraggedOutAndStandUp::~CTaskComplexCarSlowBeDraggedOutAndStandUp() {
    CEntity::SafeCleanUpRef(m_Vehicle);
}

// 0x648710
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_SET_PED_OUT:
        return new CTaskSimpleCarSetPedOut{ ped->m_pVehicle, (eTargetDoor)dword10, true };
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
        return new CTaskComplexGetUpAndStandStill{};
    case TASK_COMPLEX_LEAVE_CAR: {
        // Binary switches on the raw `m_pHandlingData->m_nAnimGroup` byte (tHandlingData is 0xE0 bytes, field at +0xDE):
        // 15 (ANIM_GROUP_COACHCARANIMS), 24 (ANIM_GROUP_TANKCARANIMS), 16 (ANIM_GROUP_BUSCARANIMS)
        const auto animGroup = m_Vehicle->GetAnimGroupId();
        if (animGroup == ANIM_GROUP_COACHCARANIMS) {
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, 8, 0, false, true };
        }
        if (animGroup == ANIM_GROUP_TANKCARANIMS) {
            return new CTaskComplexLeaveCar{ ped->m_pVehicle, 10, 0, false, true };
        }
        if (dword10 != 8 || animGroup != ANIM_GROUP_BUSCARANIMS) {
            return nullptr;
        }
        return new CTaskComplexLeaveCar{ ped->m_pVehicle, 8, 0, false, true };
    }
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        return new CTaskComplexCarSlowBeDraggedOut{ m_Vehicle, (eTargetDoor)dword10, false };
    default:
        return nullptr;
    }
}

// 0x6486F0
bool CTaskComplexCarSlowBeDraggedOutAndStandUp::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    return m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, event);
}

// 0x6488F0
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_SIMPLE_CAR_SET_PED_OUT:
    case TASK_COMPLEX_GET_UP_AND_STAND_STILL:
    case TASK_COMPLEX_LEAVE_CAR:
        return nullptr;
    case TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT:
        if (ped->IsAlive() && ped->m_fHealth > 0.0f) {
            return CreateSubTask(TASK_COMPLEX_GET_UP_AND_STAND_STILL, ped);
        }
        {
            CEventDeath event(false);
            ped->GetEventGroup().Add(&event, false);
        }
        return nullptr;
    default:
        return nullptr;
    }
}

// 0x648A10
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask(CPed* ped) {
    if (ped == FindPlayerPed(0)) {
        if (CVehicle* const vehicle = ped->m_pVehicle) {
            vehicle->m_vehicleAudio.PlayerAboutToExitVehicleAsDriver();
        }
    } else if (ped->m_pVehicle->IsDriver(ped)) {
        ped->SetRadioStation();
    }

    if (CVehicle* const vehicle = ped->m_pVehicle) {
        const auto animGroup = vehicle->GetAnimGroupId();
        if (animGroup == ANIM_GROUP_COACHCARANIMS
            || animGroup == ANIM_GROUP_TANKCARANIMS
            || (dword10 == 8 && animGroup == ANIM_GROUP_BUSCARANIMS)
        ) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }

        switch (animGroup) {
        case ANIM_GROUP_STDCARAMIMS:
        case ANIM_GROUP_VANCARANIMS:
        case ANIM_GROUP_RUSTPLANEANIMS:
        case ANIM_GROUP_KARTCARANIMS:
        case ANIM_GROUP_CONVCARANIMS:
        case ANIM_GROUP_STDTALLCARAMIMS:
        case ANIM_GROUP_HOVERCARANIMS:
        case ANIM_GROUP_STDCARUPRIGHT:
            if (dword10 == 10) {
                ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_PUNCH, 0.0f, 1.0f, nullptr);
            } else if (dword10 == 8) {
                ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_HEAD_BANG, 0.0f, 1.0f, nullptr);
            }
            break;
        case ANIM_GROUP_LOWCARAMIMS:
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_CAR_KICK, 0.0f, 1.0f, nullptr);
            break;
        case ANIM_GROUP_DOZERCARANIMS:
            ped->GetAE().AddAudioEvent(AE_PED_JACKED_DOZER, 0.0f, 1.0f, nullptr);
            break;
        default:
            break;
        }
    }
    return CreateSubTask(TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT, ped);
}

// 0x648B80
CTask* CTaskComplexCarSlowBeDraggedOutAndStandUp::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
