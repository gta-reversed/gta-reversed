#include "StdInc.h"

#include "TaskComplexProstituteSolicit.h"
#include "IKChainManager_c.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexCarDrive.h"
#include <TaskComplexTurnToFaceEntityOrCoord.h>
#include "SeekEntity/TaskComplexSeekEntity.h"
#include "SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h"

// 0x661A60
CTaskComplexProstituteSolicit::CTaskComplexProstituteSolicit(CPed* client) :
    m_Client{client}
{
    assert(client->IsPlayer());
}

// 0x661AF0
CTaskComplexProstituteSolicit::~CTaskComplexProstituteSolicit() {
    if (const auto plyr = FindPlayerPed()) {
        plyr->m_pPlayerData->m_CurrProstitute = nullptr;
        m_SexCamModeEnabled                   = false;
    }
}

// 0x661B80
bool CTaskComplexProstituteSolicit::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    const bool aborted = m_pSubTask->MakeAbortable(ped, priority, event);
    if (aborted) {
        m_SexCamModeEnabled = false;
    }
    return aborted;
}

// 0x661D30
void CTaskComplexProstituteSolicit::GetRidOfPlayerProstitute() {
    if (const CPed* prostitute = FindPlayerPed()->m_pPlayerData->m_CurrProstitute) {
        if (const auto t = prostitute->GetTaskManager().Find<CTaskComplexProstituteSolicit>(false)) {
            t->m_HadEnough = true;
            t->m_ShagTimeLeft = 0;
        }
    }
}

// 0x666360
CTask* CTaskComplexProstituteSolicit::CreateSubTask(eTaskType taskType, CPed* prostitute) {
    switch (taskType) {
    case TASK_COMPLEX_CAR_DRIVE: { // 6 - 0x666462
        m_HavingSex                   = false;
        m_WaitingToStopInSecludedSpot = true;
        return new CTaskComplexCarDrive{ m_Client->m_pVehicle };
    }
    case TASK_SIMPLE_STAND_STILL: { // 4 - 0x66642D
        return new CTaskSimpleStandStill(5'000, false, false, 8.f);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: { // 5 - 0x6663F3
        return new CTaskComplexEnterCarAsPassenger(m_Client->m_pVehicle, 8, false);
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0 - 0x6663BB
        return new CTaskComplexLeaveCar(m_Client->m_pVehicle, 0, 0, true, false);
    }
    case TASK_COMPLEX_SEEK_ENTITY: { // 1, 2 - 0x6664F8
        const auto doorA              = CCarEnterExit::GetPositionToOpenCarDoor(m_Client->m_pVehicle, TARGET_DOOR_DRIVER),
                   doorB              = CCarEnterExit::GetPositionToOpenCarDoor(m_Client->m_pVehicle, TARGET_DOOR_FRONT_RIGHT);
        const auto SqDistToProstitute = [&](CVector p) {
            return (p - prostitute->GetPosition()).SquaredMagnitude();
        };
        return new CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>(
            m_Client->m_pVehicle,
            50'000,
            1'000,
            1.f,
            2.f,
            2.f,
            false,
            false,
            CEntitySeekPosCalculatorXYOffset{
                m_Client->m_pVehicle->GetMatrix().Inverted().TransformPoint(
                    SqDistToProstitute(doorA) <= SqDistToProstitute(doorB) ? doorA : doorB
                )
            }
        );
    }
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: { // 3 - 0x6664BF
        return new CTaskComplexTurnToFaceEntityOrCoord{ m_Client, 0.5f, 0.2f };
    }
    default: NOTSA_UNREACHABLE();
    }
}

// 0x661BB0
bool CTaskComplexProstituteSolicit::IsTaskValid(CPed* prostitute, CPed* player) {
    if (!player || FindPlayerPed() != player || !player->IsInVehicle() || player->bIsBeingArrested) {
        return false;
    }
    if (const auto playersProstitute = player->m_pPlayerData->m_CurrProstitute) {
        if (playersProstitute != prostitute) {
            return false;
        }
    }
    if (player->m_pVehicle->GetVehicleAppearance() != VEHICLE_APPEARANCE_AUTOMOBILE || player->m_pVehicle->IsUpsideDown() || player->m_pVehicle->IsOnItsSide()) {
        return false;
    }
    if (player->GetTaskManager().GetSimplestActiveTask()->GetTaskType() != TASK_SIMPLE_CAR_DRIVE) {
        return false;
    }
    if (player->m_pVehicle->m_pDriver != player) {
        return false;
    }
    if (const auto v = prostitute->m_pVehicle) {
        if (v != player->m_pVehicle || v->m_nNumPassengers != 1) {
            return false;
        }
    } else if (player->m_pVehicle->m_nNumPassengers != 0) {
        return false;
    }
    if (player->m_pVehicle->m_nMaxPassengers == 0 || player->m_pVehicle->m_pHandlingData->m_bTandemSeats) {
        return false;
    }
    if ((player->GetPosition() - prostitute->GetPosition()).SquaredMagnitude() > 100.0f) {
        return false;
    }
    if (CTheScripts::IsPlayerOnAMission() || CGameLogic::IsCoopGameGoingOn()) {
        return false;
    }
    return true;
}

// 0x6666A0
CTask* CTaskComplexProstituteSolicit::CreateFirstSubTask(CPed* ped) {
    if (!IsTaskValid(ped, m_Client)) {
        m_HadEnough = true;
        return nullptr;
    }

    m_InitialVehiclePos = m_Client->m_pVehicle->GetPosition();

    const auto pd = FindPlayerPed()->m_pPlayerData;
    pd->m_CurrProstitute = ped;
    pd->m_LastProstitute = ped;

    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x666780
CTask* CTaskComplexProstituteSolicit::CreateNextSubTask(CPed* ped) {
    if (!m_Client) {
        return nullptr;
    }

    if (!IsTaskValid(ped, m_Client)) {
        m_HadEnough = true;
    }

    switch (const auto tt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: { // 0x666932
        ped->Say(CTX_GLOBAL_SOLICIT);
        CMessages::AddMessageQ(TheText.Get("PROS_04"), 5000, 1, true); // You want a good time, honey?
        return CreateSubTask(TASK_SIMPLE_STAND_STILL, ped);
    }
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x6669AF
        g_ikChainMan.LookAt("TaskProzzy", ped, m_Client, 5000, BONE_UNKNOWN, nullptr, false, 0.25f, 500, 3, false);
        return CreateSubTask(TASK_COMPLEX_TURN_TO_FACE_ENTITY, ped);
    }
    case TASK_COMPLEX_CAR_DRIVE: {
        return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x666878
        if (!m_AgreedToSex) {
            return CreateSubTask(TASK_FINISHED, ped);
        }

        if (CCheat::IsActive(CHEAT_PROSTITUTES_PAY_YOU)) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
        }

        if (FindPlayerPed()->GetPlayerInfoForThisPlayerPed()->m_nMoney >= 20) {
            return CreateSubTask(TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, ped);
        }

        CMessages::ClearMessages(false);
        CMessages::AddMessageQ(TheText.Get("PROS_06"), 2'000, 1, true); // You've got money right?
        CMessages::AddMessageQ(TheText.Get("PROS_09"), 3'000, 1, true); // Stop wasting my time!

        return CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_LEAVE_CAR: { // 0x666801
        g_ikChainMan.LookAt("TaskProzzy", ped, this->m_Client, 2'500, BONE_UNKNOWN, nullptr, true);
        return CTaskComplexProstituteSolicit::CreateSubTask(TASK_FINISHED, ped);
    }
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER: { // 0x66682F
        ped->Say(CTX_GLOBAL_SOLICIT_THANKS);
        m_InitialVehiclePos = m_Client->m_pVehicle->GetPosition();
        return CreateSubTask(TASK_COMPLEX_CAR_DRIVE, ped);
    }
    default: NOTSA_UNEXPECTED_TASK_TYPE(tt);
    }
}

// 0x6669D0
CTask* CTaskComplexProstituteSolicit::ControlSubTask(CPed* ped) {
    m_SexCamModeEnabled = m_HavingSex;

    if (!IsTaskValid(ped, m_Client)) {
        m_HadEnough         = true;
        m_SexCamModeEnabled = false;
    }

    const auto subTaskType = m_pSubTask->GetTaskType();

    if (m_HadEnough) { // 0x666A4D
        if (m_ShagTimeLeft) {
            m_ShagTimeLeft -= (int16)(CTimer::GetTimeStepInMS());
            if (m_ShagTimeLeft < 0) {
                CAEPedSpeechAudioEntity::SetCJMood(MOOD_WR, 120'000);
                m_Client->Say(CTX_GLOBAL_AFTER_SEX); // I told you I'll make your day
                m_ShagTimeLeft = 0;
            }
        } else if (subTaskType != TASK_COMPLEX_LEAVE_CAR && m_pSubTask->MakeAbortable(ped)) {
            return CreateSubTask(TASK_COMPLEX_LEAVE_CAR, ped);
        }
        return m_pSubTask;
    }

    const auto plyrWanted = FindPlayerWanted();
    const auto veh = m_Client->m_pVehicle;

    switch (subTaskType) {
    case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
    case TASK_COMPLEX_SEEK_ENTITY: { // 0x666B98
        if ((m_InitialVehiclePos - veh->GetPosition()).SquaredMagnitude() >= sq(4.f)) {
            m_HadEnough = true;
        }
        break;
    }
    case TASK_COMPLEX_CAR_DRIVE: { // 0x666BC5
        if (TheCamera.m_nWhoIsInControlOfTheCamera == 1) {
            m_ShagTimeLeft         = 8'000;
            m_SexCamModeEnabled    = false;
            m_NoMoreHealth         = true;
            m_WaitAfterEnteringCar = true;
            break;
        }

        // 0x666C13
        const auto isVehStationary = (veh->GetMoveSpeed() * 50.f).SquaredMagnitude() < sq(0.75f);
        if (!isVehStationary || m_WaitAfterEnteringCar) {
            m_LastTimeCarMoving    = CTimer::GetTimeInMS();
            m_WaitAfterEnteringCar = false;
        }

        // 0x666C5C - Scan for nearby peds/cops
        if (m_NextTimeToScanForPeds <= CTimer::GetTimeInMS()) {
            m_CopNearby              = false;
            m_PedsNearby             = false;
            m_PedsReallyNear         = false;
            m_NextTimeToScanForPeds += 1'000;

            for (auto& nearbyPed : ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>()) { // 0x666C94
                if (&nearbyPed == m_Client || nearbyPed.m_nPedType == PED_TYPE_PROSTITUTE) { // 0x666CA4
                    continue;
                }

                const auto nearbyPedDistSq = CVector::DistSqr(nearbyPed.GetPosition(), ped->GetPosition()); // 0x666CE9
                m_PedsReallyNear |= nearbyPedDistSq <= sq(7.5f);
                m_PedsNearby     |= nearbyPedDistSq <= sq(20.f);

                if (nearbyPed.IsCop() && m_HavingSex && plyrWanted && plyrWanted->GetWantedLevel() < 1) {
                    CWorld::pIgnoreEntity = veh; // NOTE: Using this instead of setting flags on the vehicle entity
                    m_CopNearby |= CWorld::GetIsLineOfSightClear(
                        nearbyPed.GetPosition(),
                        m_Client->GetPosition(),
                        true,
                        true,
                        false,
                        true,
                        false,
                        true,
                        false
                    );
                    CWorld::pIgnoreEntity = nullptr;
                }
            }
        }

        if (m_WaitingToStopInSecludedSpot) { // 0x666DB4
            if (isVehStationary && (CTimer::GetTimeInMS() - m_LastTimeCarMoving) > 4'000) {
                if (!m_PedsNearby) { // 0x666DE3 (inverted) - Found a secluded spot with no peds around...
                    m_WaitingToStopInSecludedSpot = false;
                    m_SecondsCounter              = CTimer::GetTimeInMS();
                    CMessages::AddMessageQ(TheText.Get("PROS_02"), 2'000, true, true); // This seems an ok place.
                } else if (!m_SecludedMessageShown) { // 0x666E26 - Still not in a secluded spot
                    m_SecludedMessageShown = true;
                    CMessages::AddMessageQ(TheText.Get("PROS_01"), 2'000, true, true); // We need to find somewhere more secluded
                }
            }
        } else if (m_HavingSex) { // 0x666E53
            const auto random = rand();

            if (m_FirstTime) { // 0x666E57 - sex started, 15'000 for next phase
                m_FirstTime    = false;
                m_ShagTimeLeft = 15'000;
                CStats::IncrementStat(STAT_NUMBER_OF_PROSTITUTES_VISITED);
            }

            // 0x666EAF
            const auto copsBeenNotified = m_CopNearby && plyrWanted && plyrWanted->GetWantedLevel() < 1;
            if (copsBeenNotified) {
                plyrWanted->SetWantedLevel(1);
            }

            // 0x666EE8 - Not in a secluded location anymore?
            if (const auto isPadActivated = CPad::GetPad()->GetAccelerate() || CPad::GetPad()->GetBrake(); isPadActivated || m_PedsReallyNear || copsBeenNotified) {
                m_LastTimeCarMoving = CTimer::GetTimeInMS();
                m_HavingSex         = false;

                if ((isPadActivated && random < 0x1FFFFFFF) || m_ShagTimeLeft < 3'000) { // 0x6671FA - BUG: `rand()` will always yield a value smaller than `0x1FFFFFFF`
                    m_HadEnough    = true;
                    m_ShagTimeLeft = 0;
                    if (isPadActivated) {
                        CMessages::AddMessageQ(TheText.Get("PROS_09"), 3000); // Stop wasting my time!
                    }
                } else {
                    m_ShagTimeLeft = 8'000;
                    if (!isPadActivated) {
                        CMessages::AddMessageQ(TheText.Get("PROS_01"), 3000); // We need to find somewhere more secluded.
                    }
                }

                break;
            }

            // 0x666EFE - Handle shaking
            m_ShaggingFreq -= (int16)(CTimer::GetTimeStepInMS());
            if (m_ShaggingFreq <= 0) {
                m_ShaggingFreq = m_ShagTimeLeft > 10'000
                    ? 850
                    : m_ShagTimeLeft > 5'000
                        ? 450
                        : m_ShagTimeLeft > 1'000
                            ? 120
                            : 850;
                if (m_ShagTimeLeft <= 1'000) {
                    CPad::GetPad()->StartShake(1'000, 120, 0);
                }

                const auto forceZ = std::max(std::min(veh->m_fMass / 15.f, 150.f), m_Client->m_fMass);
                const auto turnPt = ((random % 2) ? m_Client->AsPed() : ped->AsPed())->GetPosition2D();
                veh->ApplyTurnForce(
                    CVector{ 0.f, 0.f, m_ShagTimeLeft <= 1'000 ? forceZ * 0.5f : forceZ },
                    CVector{ turnPt - veh->GetPosition2D(), 0.f }
                );

                veh->m_vehicleAudio.AddAudioEvent(AE_SUSPENSION_BOUNCE, 0.f);
                if (m_DoSexAudio && random > 0xFFFFFFF) { // 0x66707F - BUG: `rand()` will never yield a value larger than `0xFFFFFFF`
                    ped->Say(random >= 0xFF ? CTX_GLOBAL_HAVING_SEX : CTX_GLOBAL_GIVING_HEAD, 0, 0.5f); // I guess she rarely gives head...
                }
            }

            // 0x6670BA - Time logic
            m_ShagTimeLeft -= (int16)(CTimer::GetTimeStepInMS());
            if (m_ShagTimeLeft <= 0) {
                m_ShagTimeLeft = 3'000;
                m_HadEnough    = true;
            }

            // 0x6670F3 - Money logic
            if (CTimer::GetTimeInMS() - m_SecondsCounter > 1000) {
                m_SecondsCounter = CTimer::GetTimeInMS();

                const auto pi = m_Client->AsPlayer()->GetPlayerInfoForThisPlayerPed();
                if (CCheat::IsActive(CHEAT_PROSTITUTES_PAY_YOU)) { // You're CJ after all...
                    pi->m_nMoney += 2;
                } else if (pi->m_nMoney < 2) { // Not enough money?
                    pi->m_nMoney = 0;
                    m_ShagTimeLeft = 0;
                    m_HadEnough = true;
                    m_HavingSex = false;
                    CMessages::ClearMessages(0);
                    CMessages::AddMessageQ(TheText.Get("PROS_06"), 2'000); // Youve got money right ?
                    CMessages::AddMessageQ(TheText.Get("PROS_09"), 3'000); // Stop wasting my time!
                } else { // Got enough money
                    pi->m_nMoney       -= 2;
                    ped->m_nMoneyCount += 1; // 50% goes to inheritor, clever R*!
                }
                if (!m_NoMoreHealth) {
                    pi->AddHealth(2);
                }
            }
        }
        break;
    }
    case TASK_SIMPLE_STAND_STILL: { // 0x666B09
        if (CPad::GetPad()->ConversationYesJustDown()) { // 0x666B09
            m_AgreedToSex = true;
            if (m_Client) {
                m_Client->Say(CTX_GLOBAL_SOLICIT_PRO_YES);
            }
            return CreateNextSubTask(ped);
        } else if (CPad::GetPad()->ConversationNoJustDown()) { // 0x666B4A
            m_HadEnough = true;
            if (m_Client) {
                m_Client->Say(CTX_GLOBAL_SOLICIT_PRO_NO);
            }
        }
        break;
    }
    }
    return m_pSubTask;
}

void CTaskComplexProstituteSolicit::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexProstituteSolicit, 0x86fb88, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x661A60);
    RH_ScopedInstall(Destructor, 0x661AF0);

    RH_ScopedGlobalInstall(IsTaskValid, 0x661BB0, { .reversed = false });
    RH_ScopedGlobalInstall(GetRidOfPlayerProstitute, 0x661D30, { .reversed = false });

    RH_ScopedInstall(CreateSubTask, 0x666360, { .reversed = false });
    RH_ScopedVMTInstall(Clone, 0x6622F0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x661AE0, { .reversed = false });
    RH_ScopedVMTInstall(MakeAbortable, 0x661B80, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x666780, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6666A0, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x6669D0, { .reversed = false });
}
