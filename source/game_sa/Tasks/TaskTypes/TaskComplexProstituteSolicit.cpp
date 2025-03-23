#include "StdInc.h"

#include "TaskComplexProstituteSolicit.h"
#include "IKChainManager_c.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexCarDrive.h"
// #include "TaskComplexTurnToFaceEntityOrCoord.h"

// 0x661A60
CTaskComplexProstituteSolicit::CTaskComplexProstituteSolicit(CPed* client) : CTaskComplex() {
    m_nLastSavedTime = 0;
    m_nNextTimeToCheckForSecludedPlace = 0;
    m_nLastPaymentTime = 0;
    m_nCurrentTimer = 0;
    m_pClient = client;
    m_nVehicleMovementTimer = 850;
    b07 = true;
    b08 = true;
    b10 = true;
    m_pClient->RegisterReference(m_pClient);
}

// 0x661AF0
CTaskComplexProstituteSolicit::~CTaskComplexProstituteSolicit() {
    auto player = FindPlayerPed();
    if (!player)
        return;

    CEntity::ClearReference(player->m_pPlayerData->m_pCurrentProstitutePed);
    if (bMoveCameraDown) {
        bMoveCameraDown = false;
    }
}

// 0x661B80
bool CTaskComplexProstituteSolicit::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (aborted) {
        bMoveCameraDown = false;
    }
    return aborted;
}

// 0x661D30
void CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute() {
    auto* prostitute = FindPlayerPed()->m_pPlayerData->m_pCurrentProstitutePed;
    if (!prostitute)
        return;

    const auto intel = prostitute->GetIntelligence();
    auto* task = intel->FindTaskByType(TASK_COMPLEX_PROSTITUTE_SOLICIT);
    if (!task)
        return;

    auto* t = static_cast<CTaskComplexProstituteSolicit*>(task);
    t->bTaskCanBeFinished = true;
    t->m_nCurrentTimer = 0;
}

// 0x666360
CTask* CTaskComplexProstituteSolicit::CreateSubTask(eTaskType taskType, CPed* prostitute) {
    return plugin::CallMethodAndReturn<CTask*, 0x666360, CTaskComplexProstituteSolicit*, eTaskType, CPed*>(this, taskType, prostitute);

    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE: // 6
        bSearchingForSecludedPlace = true;
        return new CTaskComplexCarDrive(m_pClient->m_pVehicle);

    case TASK_SIMPLE_STAND_STILL: // 4
        return new CTaskSimpleStandStill(5000, false, false, 8.0f);

    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: // 5
        return new CTaskComplexEnterCarAsPassenger(m_pClient->m_pVehicle, 8, false);

    case TASK_COMPLEX_LEAVE_CAR: // 0
        return new CTaskComplexLeaveCar(m_pClient->m_pVehicle, 0, 0, true, false);
/*
    case TASK_COMPLEX_SEEK_ENTITY: { // 1, 2
        CMatrix out;
        Invert(m_pClient->m_pVehicle->m_matrix, &out);

        CVector v24, v25;
        CCarEnterExit::GetPositionToOpenCarDoor(&v24, m_pClient->m_pVehicle, 10);
        CCarEnterExit::GetPositionToOpenCarDoor(&v25, m_pClient->m_pVehicle, 8);

        p_m_pos = &ped->m_matrix->m_pos;
        if (!ped->m_matrix)
            p_m_pos = &ped->m_placement;
        v26 = p_m_pos->m_vPosn.x - v24.x;
        if (v14 | v15) {
            v16 = &v25;
            v17 = &v27;
        } else {
            v16 = &v24;
            v17 = &v28;
        }
        v18 = v17->TransformPoint(&out, v16);
        x = v18->x;
        y = v18->y;
        z = v18->z;

        auto* v23 = CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>(m_pClient->m_pVehicle, 50000, 1000, 1.0f, 2.0f, 2.0f, 0, 0);
        v23[1].dword10 = x;
        v23[1].dword14 = y;
        v23[1].dword18 = z;
        return v23;
    }

    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: // 3
        return CTaskComplexTurnToFaceEntityOrCoord(m_pClient, 0.5f, 0.2f);
*/
    default:
        return nullptr;
    }
}

// 0x661BB0
bool CTaskComplexProstituteSolicit::IsTaskValid(CPed* prostitute, CPed* ped) {
    if (FindPlayerPed() != ped)
        return false;

    if (!ped)
        return false;

    if (!ped->IsInVehicle())
        return false;

    if (ped->bIsBeingArrested)
        return false;

    if (ped->m_pPlayerData->m_pCurrentProstitutePed && ped->m_pPlayerData->m_pCurrentProstitutePed != prostitute)
        return false;

    if (ped->m_pVehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE)
        return false;

    if (ped->m_pVehicle->IsUpsideDown())
        return false;

    if (ped->m_pVehicle->IsOnItsSide())
        return false;

    auto task = ped->GetTaskManager().GetSimplestActiveTask();
    if (task->GetTaskType() != TASK_SIMPLE_CAR_DRIVE)
        return false;

    if (ped->m_pVehicle->m_pDriver != ped)
        return false;

    if (prostitute->m_pVehicle) {
        if (prostitute->m_pVehicle != ped->m_pVehicle || prostitute->m_pVehicle->m_nNumPassengers != 1)
            return false;
    } else if (ped->m_pVehicle->m_nNumPassengers) {
        return false;
    }

    if (!ped->m_pVehicle->m_nMaxPassengers || ped->m_pVehicle->m_pHandlingData->m_bTandemSeats)
        return false;

    CVector out = ped->GetPosition() - prostitute->GetPosition();
    if (out.SquaredMagnitude() > 100.0f || CTheScripts::IsPlayerOnAMission() || CGameLogic::IsCoopGameGoingOn()) {
        return false;
    }
    return true;
}

// 0x6666A0
CTask* CTaskComplexProstituteSolicit::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6666A0, CTaskComplexProstituteSolicit*, CPed*>(this, ped);

    if (!IsTaskValid(ped, m_pClient)) {
        bTaskCanBeFinished = true;
        return nullptr;
    }

    m_vecVehiclePosn = m_pClient->m_pVehicle->GetPosition();

    auto player = FindPlayerPed();
    m_pClient->m_pPlayerData->m_pCurrentProstitutePed = ped;
    player->m_pPlayerData->m_pCurrentProstitutePed->RegisterReference(player->m_pPlayerData->m_pCurrentProstitutePed);

    if (m_pClient->m_pPlayerData->m_pLastProstituteShagged != ped) {
        CEntity::SafeCleanUpRef(m_pClient->m_pPlayerData->m_pLastProstituteShagged);
        m_pClient->m_pPlayerData->m_pLastProstituteShagged = ped;
        m_pClient->m_pPlayerData->m_pLastProstituteShagged->RegisterReference(m_pClient->m_pPlayerData->m_pLastProstituteShagged);
    }

    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x666780
CTask* CTaskComplexProstituteSolicit::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x666780, CTaskComplexProstituteSolicit*, CPed*>(this, ped);

    if (!m_pClient) {
        return nullptr;
    }

    if (!IsTaskValid(ped, m_pClient)) {
        bTaskCanBeFinished = true;
    }

    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY:
        ped->Say(CTX_GLOBAL_SOLICIT);
        CMessages::AddMessageQ(TheText.Get("PROS_04"), 5000, 1, true); // You want a good time, honey?
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);

    case TASK_COMPLEX_SEEK_ENTITY:
        g_ikChainMan.LookAt("TaskProzzy", ped, m_pClient, 5000, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
        return CreateSubTask(TASK_COMPLEX_TURN_TO_FACE_ENTITY, ped);

    case TASK_COMPLEX_CAR_DRIVE:
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    //default:
    //    return nullptr;
    }

    auto taskId = m_pSubTask->GetTaskType();

    auto v6 = taskId - 203;
    if (!v6) {
        if (!bPlayerHasAcceptedSexProposition)
            return CreateSubTask(TASK_FINISHED, ped);

        if (CCheat::IsActive(CHEAT_PROSTITUTES_PAY_YOU)) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
        }

        auto player = FindPlayerPed();
        if (player->GetPlayerInfoForThisPlayerPed()->m_nMoney >= 20) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
        }
        CMessages::ClearMessages(false);
        CMessages::AddMessageQ(TheText.Get("PROS_06"), 2000, 1, true); // You've got money right?
        CMessages::AddMessageQ(TheText.Get("PROS_09"), 3000, 1, true); // Stop wasting my time!
        return CreateSubTask(TASK_FINISHED, ped);
    }

    auto v7 = v6 - 497;
    if (v7) {
        if (v7 == 4) {
            g_ikChainMan.LookAt("TaskProzzy", ped, m_pClient, 2500, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
            return CreateSubTask(TASK_FINISHED, ped);
        }
        return nullptr;
    }

    ped->Say(CTX_GLOBAL_SOLICIT_THANKS);
    m_vecVehiclePosn = m_pClient->m_pVehicle->GetPosition();
    return CreateSubTask(TASK_COMPLEX_CAR_DRIVE, ped);
}

// 0x6669D0
CTask* CTaskComplexProstituteSolicit::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6669D0, CTaskComplexProstituteSolicit*, CPed*>(this, ped);
}
