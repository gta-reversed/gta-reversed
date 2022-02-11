#include "StdInc.h"

#include "TaskComplexWanderCop.h"

#include "TaskSimpleStandStill.h"
#include "TaskComplexPolicePursuit.h"

void CTaskComplexWanderCop::InjectHooks() {
    RH_ScopedClass(CTaskComplexWanderCop);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x460C80);
    RH_ScopedInstall(Clone_Reversed, 0x460CE0);
    RH_ScopedInstall(CreateNextSubTask_Reversed, 0x674860);
    RH_ScopedInstall(CreateFirstSubTask_Reversed, 0x674750);
    RH_ScopedInstall(ControlSubTask_Reversed, 0x674D80);
    RH_ScopedInstall(ScanForStuff_Reversed, 0x6702B0);
    RH_ScopedInstall(LookForCarAlarms, 0x66B1B0);
    RH_ScopedInstall(LookForStolenCopCars, 0x66B290);
    RH_ScopedInstall(LookForCriminals, 0x66B300);
    RH_ScopedInstall(ShouldPursuePlayer, 0x66B160);
}

// 0x460C80
CTaskComplexWanderCop::CTaskComplexWanderCop(int32 moveState, uint8 dir) : CTaskComplexWander(moveState, dir, true, 0.5f) {
    m_pTaskComplexMoveGoToPointAndStandStill = nullptr;
    m_nTimePassedSinceLastLookedForCriminals = 0;
    m_nTimePassedSinceLastLookedForCarAlarmsAndStolenCopCars = 0;
    m_pLastCriminalPedLookedFor = nullptr;
}

// 0x460D60
CTaskComplexWanderCop::~CTaskComplexWanderCop() {
    delete (CTask*)m_pTaskComplexMoveGoToPointAndStandStill;
}

// 0x460C80
CTaskComplexWanderCop* CTaskComplexWanderCop::Constructor(int32 moveState, uint8 dir) {
    this->CTaskComplexWanderCop::CTaskComplexWanderCop(moveState, dir);
    return this;
}

// 0x460CE0
CTask* CTaskComplexWanderCop::Clone() {
    return new CTaskComplexWanderCop(m_nMoveState, m_nDir);
}

// 0x674860
CTask* CTaskComplexWanderCop::CreateNextSubTask(CPed* ped) {
    return CreateNextSubTask_Reversed(ped);
}

// 0x674750
CTask* CTaskComplexWanderCop::CreateFirstSubTask(CPed* ped) {
    return CreateFirstSubTask_Reversed(ped);
}

// 0x674D80
CTask* CTaskComplexWanderCop::ControlSubTask(CPed* ped) {
    return ControlSubTask_Reversed(ped);
}

// 0x460D50
int32 CTaskComplexWanderCop::GetWanderType() {
    return CTaskComplexWanderCop::GetWanderType_Reversed();
}

// 0x6702B0
void CTaskComplexWanderCop::ScanForStuff(CPed* ped) {
    return ScanForStuff_Reversed(ped);
}

CTask* CTaskComplexWanderCop::Clone_Reversed() {
    return new CTaskComplexWanderCop(m_nMoveState, m_nDir);
}

CTask* CTaskComplexWanderCop::CreateNextSubTask_Reversed(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::CreateNextSubTask(ped);

    if (ped->AsCop()->m_bDontPursuit)
        return nullptr;

    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT) {
        m_nSubTaskCreatedTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nSubTaskCreatedTimer.m_nInterval = 3000;
        m_nSubTaskCreatedTimer.m_bStarted = true;
        if (m_pTaskComplexMoveGoToPointAndStandStill) {
            return ((CTask*)m_pTaskComplexMoveGoToPointAndStandStill)->Clone();
        } else {
            return CTaskComplexWander::CreateFirstSubTask(ped);
        }
    } else {
        auto* gotoStandStill = (CTask*)m_pTaskComplexMoveGoToPointAndStandStill;
        if (gotoStandStill && m_pSubTask->GetTaskType() == gotoStandStill->GetTaskType()) {
            return nullptr;
        } else {
            if (m_pTaskComplexMoveGoToPointAndStandStill) {
                return ((CTask*)m_pTaskComplexMoveGoToPointAndStandStill)->Clone();
            } else {
                return CTaskComplexWander::CreateNextSubTask(ped);
            }
        }
    }
    return nullptr;
}

CTask* CTaskComplexWanderCop::CreateFirstSubTask_Reversed(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::CreateFirstSubTask(ped);

    if (ped->AsCop()->m_bDontPursuit) {
        return new CTaskSimpleStandStill(100000, true, false, 8.0f);
    }

    if (!ShouldPursuePlayer(ped)) {
        if (m_pTaskComplexMoveGoToPointAndStandStill) {
            return ((CTask*)m_pTaskComplexMoveGoToPointAndStandStill)->Clone();
        }
        return CTaskComplexWander::CreateFirstSubTask(ped);
    }

    auto pTaskComplexPolicePursuit = (CTaskComplexPolicePursuit*)CTask::operator new(24);
    if (!pTaskComplexPolicePursuit) {
        return nullptr;
    }
    pTaskComplexPolicePursuit->Constructor();
    return (CTask*)pTaskComplexPolicePursuit;
}

CTask* CTaskComplexWanderCop::ControlSubTask_Reversed(CPed* ped) {
    if (ped->m_nPedType != PED_TYPE_COP)
        return CTaskComplexWander::ControlSubTask(ped);

    if (!ShouldPursuePlayer(ped)) {
        if (m_pSubTask->GetTaskType() == TASK_COMPLEX_POLICE_PURSUIT) {
            return m_pSubTask;
        }
        if (m_pTaskComplexMoveGoToPointAndStandStill) {
            return m_pSubTask;
        }

        return CTaskComplexWander::ControlSubTask(ped);
    }
    if (m_nSubTaskCreatedTimer.m_bStarted && !m_nSubTaskCreatedTimer.IsOutOfTime() || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr)) {
        return m_pSubTask;
    }

    auto pTaskComplexPolicePursuit = (CTaskComplexPolicePursuit*)CTask::operator new(24);
    if (pTaskComplexPolicePursuit) {
        pTaskComplexPolicePursuit->Constructor();
        return pTaskComplexPolicePursuit;
    }
    return nullptr;
}

void CTaskComplexWanderCop::ScanForStuff_Reversed(CPed* ped) {
    if (!m_nScanForStuffTimer.m_bStarted) {
        m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
        m_nScanForStuffTimer.m_nInterval = 50;
        m_nScanForStuffTimer.m_bStarted = true;
    }
    if (GetTaskType() != TASK_COMPLEX_POLICE_PURSUIT && m_nScanForStuffTimer.m_bStarted) {
        if (m_nScanForStuffTimer.m_bStopped) {
            m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_nScanForStuffTimer.m_bStopped = false;
        }

        if (CTimer::GetTimeInMS() >= (m_nScanForStuffTimer.m_nStartTime + m_nScanForStuffTimer.m_nInterval)) {
            m_nScanForStuffTimer.m_nInterval = 50;
            m_nScanForStuffTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_nScanForStuffTimer.m_bStarted = true;

            if (CTimer::GetTimeInMS() >= m_nTimePassedSinceLastLookedForCarAlarmsAndStolenCopCars) {
                LookForCarAlarms(ped);
                LookForStolenCopCars(ped);
            }

            if (CTimer::GetTimeInMS() >= m_nTimePassedSinceLastLookedForCriminals) {
                LookForCriminals(ped);
            }
        }
    }
}

// 0x66B1B0
void CTaskComplexWanderCop::LookForCarAlarms(CPed* ped) {
    CVehicle* vehicle = FindPlayerVehicle();
    if (!vehicle || !vehicle->IsAutomobile())
        return;

    if (!vehicle->m_nAlarmState || vehicle->m_nAlarmState == -1 || vehicle->m_nStatus == STATUS_WRECKED)
        return;

    CVector distance = vehicle->GetPosition() - ped->GetPosition();
    if (distance.SquaredMagnitude() < 400.0f)
        FindPlayerPed()->SetWantedLevelNoDrop(1);
}

// 0x66B290
void CTaskComplexWanderCop::LookForStolenCopCars(CPed* ped) {
    CPlayerPed* player = FindPlayerPed();

    CWanted* wanted = nullptr;
    if (player->m_pPlayerData) {
        wanted = player->m_pPlayerData->m_pWanted;
    }

    if (wanted && !wanted->m_nWantedLevel && player->m_pVehicle) {
        if (player->m_pVehicle->vehicleFlags.bIsLawEnforcer) {
            player->SetWantedLevelNoDrop(1);
        }
    }
}

// 0x66B300
void CTaskComplexWanderCop::LookForCriminals(CPed* ped) {
    CPed* criminalPed = nullptr;
    for (auto& entity : ped->GetIntelligence()->m_entityScanner.m_apEntities) {
        criminalPed = (CPed*)entity;
        if (!criminalPed)
            continue;

        int32 pedType = criminalPed->m_nPedType;
        if (pedType >= PED_TYPE_GANG1 && pedType <= PED_TYPE_GANG10 || pedType == PED_TYPE_CRIMINAL && criminalPed != m_pLastCriminalPedLookedFor) {
            CTask* activeTask = criminalPed->GetTaskManager().GetActiveTask();
            if (activeTask && activeTask->GetTaskType() == GetTaskType()) {
                CVector distance = (criminalPed->GetPosition() - ped->GetPosition());
                if (10.0f * 10.0f > distance.SquaredMagnitude()) {
                    const float dot = DotProduct(distance, ped->GetForward());
                    if (dot > 0.0f && CWorld::GetIsLineOfSightClear(ped->GetPosition(), criminalPed->GetPosition(), true, false, false, true, false, false, false))
                        break;
                }
            }
        }
    }

    if (!criminalPed)
        return;

    CEventPedToChase eventPedToChase(criminalPed);
    ped->GetEventGroup().Add(&eventPedToChase, false);

    CEventPedToFlee eventPedToFlee(ped);
    criminalPed->GetEventGroup().Add(&eventPedToFlee, false);

    // 30 seconds wait for next check
    m_nTimePassedSinceLastLookedForCriminals = CTimer::GetTimeInMS() + 30000;
    m_pLastCriminalPedLookedFor = criminalPed;
}

// 0x66B160
bool CTaskComplexWanderCop::ShouldPursuePlayer(CPed* ped) {
    CWanted* wanted = FindPlayerWanted();
    if (wanted->m_nWantedLevel > 0) {
        if (!m_pSubTask || m_pSubTask->GetTaskType() != TASK_COMPLEX_POLICE_PURSUIT) {
            if (wanted->CanCopJoinPursuit(ped->AsCop())) {
                return true;
            }
        }
    }
    return false;
}
