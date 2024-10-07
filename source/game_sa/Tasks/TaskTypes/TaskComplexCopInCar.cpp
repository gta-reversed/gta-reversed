#include "StdInc.h"

#include "TaskComplexCopInCar.h"
#include "TaskSimpleCarDrive.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexPolicePursuit.h"
#include "TaskComplexWanderCop.h"
#include "TaskComplexCarDrive.h"
#include "TaskComplexCarDriveMission.h"
#include "eCarDrivingStyle.h"

void CTaskComplexCopInCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexCopInCar, 0x870A2C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x68C7F0);
    RH_ScopedInstall(Destructor, 0x68C8C0);
    RH_ScopedInstall(CreateSubTask, 0x68C9E0);
    RH_ScopedVMTInstall(Clone, 0x68CEC0);
    RH_ScopedVMTInstall(GetTaskType, 0x68C8B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x68C940);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x68FA50);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x68FA10);
    RH_ScopedVMTInstall(ControlSubTask, 0x68FD50);
}

// 0x68C7F0
CTaskComplexCopInCar::CTaskComplexCopInCar(CVehicle* vehicle, CPed* cop, CPed* suspect, bool bSuspectDeadOrLost) : CTaskComplex(),
    m_Vehicle{ vehicle },
    m_Cop{ cop },
    m_Suspect{ suspect },
    m_bSuspectDeadOrLost{ bSuspectDeadOrLost }
{
    m_flag0x4 = true;
    CEntity::SafeRegisterRef(m_Vehicle);
    CEntity::SafeRegisterRef(m_Cop);
    CEntity::SafeRegisterRef(m_Suspect);
}

// 0x68C8C0
CTaskComplexCopInCar::~CTaskComplexCopInCar() {
    CEntity::SafeCleanUpRef(m_Vehicle);
    CEntity::SafeCleanUpRef(m_Cop);
    CEntity::SafeCleanUpRef(m_Suspect);
}
 
// 0x68C9E0
CTask* CTaskComplexCopInCar::CreateSubTask(eTaskType taskType, CPed* copPed) {
    switch (taskType) {
    case TASK_SIMPLE_CAR_DRIVE:
        return new CTaskSimpleCarDrive(m_Vehicle);
    case TASK_COMPLEX_LEAVE_CAR: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_KNOCK_OFF_BIKE);
        return new CTaskComplexLeaveCar(m_Vehicle, 0, 0, true, false);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
        return new CTaskComplexEnterCarAsDriver(m_Vehicle);
    }
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleStandStill(1000, true, false, 8.f);
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: {
        copPed->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
        return new CTaskComplexEnterCarAsPassenger(m_Vehicle, 0, false);
    }
    case TASK_COMPLEX_POLICE_PURSUIT:
        return new CTaskComplexPolicePursuit();
    case TASK_COMPLEX_WANDER:
        return new CTaskComplexWanderCop(PEDMOVE_WALK, (uint8)CGeneral::GetRandomNumberInRange(0, 8));
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        const auto targetEntity = m_Suspect->bInVehicle ? (CEntity*)m_Suspect->m_pVehicle : (CEntity*)m_Suspect;
        if (m_Suspect->IsPlayer()) {
            return new CTaskComplexCarDriveMission(
                m_Vehicle,
                targetEntity,
                m_Suspect->bInVehicle ? (eCarMission)CCarAI::FindPoliceCarMissionForWantedLevel() : MISSION_POLICE_BIKE, // CCarAI::FindPoliceBikeMissionForWantedLevel()?
                (eCarDrivingStyle)CCarAI::FindPoliceCarSpeedForWantedLevel(m_Vehicle), // TODO: This really doesn't add up.. How does this work?
                10.f
            );
        } else {
            const auto GetDrivingMission = [this] {
                if (m_Suspect->bInVehicle) {
                    return CGeneral::GetRandomNumber() % 4 < 2 ? MISSION_BLOCKPLAYER_FARAWAY : MISSION_RAMPLAYER_FARAWAY;
                }
                return MISSION_37;
            };

            // FindPoliceCarSpeedForWantedLevel - wanted level 3
            auto style = m_Vehicle->m_pHandlingData->m_transmissionData.m_fMaxVelocity * 60.0f * 0.9f; // TODO: This really doesn't add up.. How does this work?
            return new CTaskComplexCarDriveMission(
                m_Vehicle,
                targetEntity,
                GetDrivingMission(),
                (eCarDrivingStyle)style,
                10.f
            );
        }
    }
    case TASK_FINISHED:
        return nullptr;

    default:
        NOTSA_UNREACHABLE();
        return nullptr;
    }
}

// 0x68C940


bool CTaskComplexCopInCar::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask) {
        return true;
    }

    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }

    if (!event || event->GetEventType() != EVENT_DAMAGE)
        return true;

    const auto dmgEvent = (CEventDamage*)event;
    if (!dmgEvent->m_damageResponse.m_bHealthZero)
        return true;

    if (!dmgEvent->m_bAddToEventGroup || !ped->bInVehicle)
        return true;

    if (ped->m_pVehicle == m_Vehicle && m_Vehicle) {
        if (m_Vehicle->IsDriver(ped)) {
            m_Vehicle->m_nStatus = STATUS_ABANDONED;
            m_Vehicle->m_autoPilot.m_nCarMission = MISSION_NONE;
            m_Vehicle->m_autoPilot.m_nCruiseSpeed = 0;
        }
    }

    return true;
}

// 0x68FA50


CTask* CTaskComplexCopInCar::CreateNextSubTask(CPed* ped) {
    // Ped is a CopPed*

    if (!m_Suspect || m_Suspect->IsStateDead()) {
        return CreateSubTask(TASK_FINISHED, ped);
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_CAR_DRIVE_MISSION: {
        m_flag0x2 = true;
        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
    }
    case TASK_COMPLEX_POLICE_PURSUIT: {
        if (!FindPlayerWanted()->m_nWantedLevel) {
            return CreateSubTask(TASK_FINISHED, ped);
        }

        assert(ped->m_nPedType == PED_TYPE_COP);
        if (FindPlayerWanted()->CanCopJoinPursuit(ped->AsCop()) && static_cast<CTaskComplexPolicePursuit*>(m_pSubTask)->m_nFlags & 4) { // todo: flags
            // 0x68FBC6 - Inverted
            if (m_Suspect->bIsBeingArrested) {
                return CreateSubTask(TASK_FINISHED, ped);
            }

            if (m_Suspect->bInVehicle && m_Vehicle != m_Suspect->m_pVehicle) {
                if (m_Vehicle && m_bSuspectDeadOrLost) {
                    if (ped->bInVehicle) {
                        return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
                    } else if ((m_Vehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude() < 4.f * 4.f) {
                        return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped);
                    }
                }
            }
        }

        m_Timer2.m_nStartTime = CTimer::GetTimeInMS();
        m_Timer2.m_nInterval = 3000;
        m_Timer2.m_bStarted = true;
        return CreateSubTask(TASK_COMPLEX_WANDER, ped);
    }
    case TASK_SIMPLE_CAR_DRIVE: {
        m_Vehicle->m_autoPilot.m_nCarMission = MISSION_NONE;
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 0x68FA89
    case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:    // 0x68FA94
    {
        if (ped->IsInVehicle()) {
            ped->m_pVehicle->ChangeLawEnforcerState(true);
            m_flag0x4 = false;
            m_bIsSuspectInCar = ped->bInVehicle;
            return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
        } else {
            return CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped);
        }
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x68FAA0
        return CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped);
    }
    default:
        return CreateSubTask(TASK_FINISHED, ped);
    }
}

// 0x68FA10


CTask* CTaskComplexCopInCar::CreateFirstSubTask(CPed* ped) {
    ped->GetIntelligence()->SetPedDecisionMakerType(DM_EVENT_SHOT_FIRED);
    if (!m_Cop) {
        m_bSuspectDeadOrLost = true; // TODO/NOTE: Weird???
    }
    return CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped);
}

// 0x68FD50

// 0x0
CTask* CTaskComplexCopInCar::ControlSubTask(CPed* ped) {
    if (!m_Suspect || m_Suspect->IsStateDead()) { // Inverted
        return m_pSubTask->MakeAbortable(ped) ? CreateSubTask(TASK_FINISHED, ped) : m_pSubTask; // Inverted
    }

    // 0x68FD7E
    if (m_Vehicle && !m_Vehicle->m_pDriver && m_Vehicle->m_autoPilot.m_nCarMission != MISSION_NONE) {
        m_Vehicle->m_autoPilot.m_nCarMission = MISSION_NONE;
    }

    if (!m_bSuspectDeadOrLost && (!m_Cop || m_Cop->IsStateDead())) { // 0x68FDA7
        m_bSuspectDeadOrLost = true;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_POLICE_PURSUIT: { // 0x690061
        if (!m_Suspect->IsInVehicle()) {
            return m_pSubTask;
        }

        if (m_Suspect->m_pVehicle == m_Vehicle) {
            return m_pSubTask;
        }

        if (!m_Vehicle) {
            return m_pSubTask;
        }

        if (m_bSuspectDeadOrLost) {
            return m_pSubTask;
        }

        if (m_Vehicle->m_fHealth <= 0.f) {
            return m_pSubTask;
        }

        if (m_Vehicle->m_pFire) {
            return m_pSubTask;
        }

        if (m_Vehicle->IsOnItsSide()) {
            return m_pSubTask;
        }

        if (m_Suspect->m_pVehicle->m_vecMoveSpeed.SquaredMagnitude() * CTimer::GetTimeStep() * 50.f <= 1.f) {
            if ((m_Suspect->m_pVehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude() <= 10.f * 10.f) {
                return m_pSubTask;
            }
        }

        if ((m_Vehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude() >= 4.f * 4.f) {
            return m_pSubTask;
        }

        return m_pSubTask->MakeAbortable(ped)
                   ? CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_DRIVER, ped)
                   : m_pSubTask;
    }
    case TASK_COMPLEX_WANDER: {
        return m_Timer2.IsOutOfTime() && m_pSubTask->MakeAbortable(ped)
                   ? CreateSubTask(TASK_COMPLEX_POLICE_PURSUIT, ped)
                   : m_pSubTask; // Inverted
    }
    case TASK_COMPLEX_CAR_DRIVE_MISSION: { // 0x68FDDE
        if ((bool)m_bIsSuspectInCar != m_Suspect->bInVehicle) {
            if (m_pSubTask->MakeAbortable(ped)) {
                m_bIsSuspectInCar = m_Suspect->bInVehicle;
                return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
            }
        }
        return m_flag0x2 && m_pSubTask->MakeAbortable(ped)
                   ? CreateSubTask(TASK_SIMPLE_CAR_DRIVE, ped)
                   : m_pSubTask; // Inverted
    }
    case TASK_SIMPLE_CAR_DRIVE: { // 0x68FE65
        if (!m_flag0x2) {
            return m_pSubTask;
        }

        if (!m_bSuspectDeadOrLost) {
            if (m_pSubTask->MakeAbortable(ped)) {
                m_flag0x2 = false;
                return CreateNextSubTask(ped);
            }
            return m_pSubTask;
        }

        if (!m_Timer1.m_bStarted) {
            m_Timer1.Start(m_flag0x4 ? CGeneral::GetRandomNumberInRange(0, 2000) + 3000 : CGeneral::GetRandomNumberInRange(0, 1250) + 250);
            return m_pSubTask;
        }

        if (   !m_Suspect->IsInVehicle()
            || m_Suspect->m_pVehicle == m_Vehicle
            || (m_Suspect->m_pVehicle->m_vecMoveSpeed.SquaredMagnitude() * CTimer::GetTimeStep() * 50.f <= 1.f
            && (m_Suspect->m_pVehicle->GetPosition() - ped->GetPosition()).SquaredMagnitude() <= 10.f * 10.f) // Same code used above
            || !m_pSubTask->MakeAbortable(ped) // This is really weird.. Here they check if its not abortable, then below check if its abortable..
        ) {
            if (m_Timer1.IsOutOfTime()) {
                if (m_pSubTask->MakeAbortable(ped)) {
                    m_flag0x2 = false;
                    m_Timer1.m_bStarted = false;
                    return CreateNextSubTask(ped);
                }
                return m_pSubTask;
            }
        }

        m_flag0x2 = false;
        m_flag0x4 = false;
        m_bIsSuspectInCar = ped->bInVehicle;
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE_MISSION, ped);
    }
    default:
        break;
    }
    return m_pSubTask;
}

