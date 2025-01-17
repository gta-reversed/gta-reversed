#include "StdInc.h"

#include "StuntJumpManager.h"
#include "Hud.h"

static constexpr uint16 STUNT_JUMP_COUNT = 256;

void CStuntJumpManager::InjectHooks() {
    RH_ScopedClass(CStuntJumpManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49CA50);
    RH_ScopedInstall(Shutdown, 0x49CBC0);
    RH_ScopedInstall(ShutdownForRestart, 0x49CB10);
    RH_ScopedInstall(Save, 0x5D5570);
    RH_ScopedInstall(Load, 0x5D5920);
    RH_ScopedInstall(AddOne, 0x49CB40);
    RH_ScopedInstall(Update, 0x49C490);
}

// 0x49CA50
void CStuntJumpManager::Init() {
    ZoneScoped;

    mp_poolStuntJumps = new CStuntJumpsPool(STUNT_JUMP_COUNT, "Stunt Jumps");
    m_bActive = true;
}

// 0x49CBC0
void CStuntJumpManager::Shutdown() {
    if (mp_poolStuntJumps)
        mp_poolStuntJumps->Flush();

    mp_poolStuntJumps = nullptr;
}

// 0x49CB10
void CStuntJumpManager::ShutdownForRestart() {
    mp_poolStuntJumps->Clear(); // 0x49C9A0
    mp_Active = nullptr;
    m_bHitReward = false;
    m_iTimer = 0;
    m_jumpState = eJumpState::START_POINT_INTERSECTED;
    m_iNumJumps = 0;
    m_iNumCompleted = 0;
}

// 0x5D5570
bool CStuntJumpManager::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(m_iNumJumps);
    for (auto i = 0; i < STUNT_JUMP_COUNT; i++) {
        auto* jump = mp_poolStuntJumps->GetAt(i);
        if (jump) {
            CGenericGameStorage::SaveDataToWorkBuffer(*jump);
        }
    }
    return true;
}

// 0x5D5920
bool CStuntJumpManager::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(m_iNumJumps);
    for (int32 i = 0; i < m_iNumJumps; i++) {
        auto& jump = *mp_poolStuntJumps->New();
        CGenericGameStorage::LoadDataFromWorkBuffer(jump);
    }
    return true;
}

// 0x49CB40
void CStuntJumpManager::AddOne(const CBoundingBox& start, const CBoundingBox& end, const CVector& cameraPosn, int32 reward) {
    if (mp_poolStuntJumps) {
        CStuntJump* jump = mp_poolStuntJumps->New();
        if (jump) {
            jump->start = start;
            jump->end = end;
            jump->camera = cameraPosn;
            jump->reward = reward;
            jump->done = false;
            jump->found = false;
            ++m_iNumJumps;
        }
    }
}

// 0x49C490
void CStuntJumpManager::Update() {
    ZoneScoped;

    if (!mp_poolStuntJumps || CReplay::Mode == MODE_PLAYBACK)
        return;

    auto* playerPed     = FindPlayerPed();
    auto* playerInfo    = playerPed->GetPlayerInfoForThisPlayerPed();
    auto* playerVehicle = playerPed->m_pVehicle;

    switch (m_jumpState) {
    case eJumpState::START_POINT_INTERSECTED: {
        if (!m_bActive || playerInfo->m_nPlayerState != PLAYERSTATE_PLAYING || 
            (playerPed->bInVehicle) == 0 || !playerVehicle)
            break;

        if (playerVehicle->m_pDriver != playerPed ||
            playerVehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT || 
            playerVehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE ||
            playerVehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI ||
            playerVehicle->m_nNumEntitiesCollided > 0)
            break;

        if (playerVehicle->m_vecMoveSpeed.Magnitude() * 50.0f < 20.0f)
            break;

        for (auto jumpIndex = 0; jumpIndex < STUNT_JUMP_COUNT; jumpIndex++) {
            CStuntJump* jump = mp_poolStuntJumps->GetAt(jumpIndex);
            if (!jump)
                continue;

            if (!jump->start.IsPointWithin(playerVehicle->GetPosition()))
                continue;

            m_jumpState = eJumpState::IN_FLIGHT;
            mp_Active = jump;
            m_iTimer = 0;
            m_bHitReward = false;

            if (!mp_Active->found) {
                mp_Active->found = true;
                CStats::IncrementStat(STAT_UNIQUE_JUMPS_FOUND, 1.0f);
            }

            CTimer::SetTimeScale(0.3f);
            TheCamera.SetCamPositionForFixedMode(mp_Active->camera, CVector(0.0f, 0.0f, 0.0f));
            TheCamera.TakeControl(playerVehicle, MODE_FIXED, eSwitchType::JUMPCUT, 1);
            break;
        }
        break;
    }
    case eJumpState::IN_FLIGHT: {
        if (!mp_Active)
            break;

        bool failed = false;
        if (playerVehicle->m_nNumEntitiesCollided > 0 && m_iTimer >= 100)
            failed = true;

        if (failed || playerInfo->m_nPlayerState == PLAYERSTATE_HAS_DIED ||
            !playerPed->bInVehicle ||
            (playerVehicle->m_nStatus == STATUS_WRECKED) ||
            (playerVehicle->vehicleFlags.bIsDrowning) ||
            (playerVehicle->physicalFlags.bSubmergedInWater))
        {
            failed = true;
        }

        if (mp_Active->end.IsPointWithin(playerVehicle->GetPosition())) {
            m_bHitReward = true;
        }

        if (failed) {
            m_jumpState = eJumpState::END_POINT_INTERSECTED;
            m_iTimer = 0;
        } else {
            const auto timeStep = failed ? 0 : m_iTimer;
            m_iTimer = (uint32)CTimer::GetTimeStepInMS() + timeStep;
            if (m_iTimer > 1'000 && timeStep <= 1'000) {
                if (const auto veh = FindPlayerVehicle()) {
                    if (const auto p = veh->PickRandomPassenger()) {
                        p->Say(CTX_GLOBAL_CAR_JUMP);
                    }
                }
            }
        }
        break;
    }
    case eJumpState::END_POINT_INTERSECTED: {
        m_iTimer += (uint32)CTimer::GetTimeStepInMS();
        if (m_iTimer < 300)
            return;

        CTimer::ResetTimeScale();
        TheCamera.RestoreWithJumpCut();

        if (!m_bHitReward || mp_Active->done) {
            m_jumpState = eJumpState::START_POINT_INTERSECTED;
            mp_Active = nullptr;
            break;
        }

        mp_Active->done = true;
        ++m_iNumCompleted;

        CStats::IncrementStat(STAT_UNIQUE_JUMPS_DONE, 1.0f);

        const auto reward = m_iNumCompleted == m_iNumJumps ? 10'000 : mp_Active->reward;
        playerInfo->m_nMoney += reward;

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 1.0f, 0.0f);

        if (const auto m = TheText.Get("USJ")) {
            // UNIQUE STUNT BONUS!
            CMessages::AddBigMessageQ(m, 5'000, STYLE_MIDDLE_SMALLER_HIGHER);
        }

        if (m_iNumCompleted == m_iNumJumps) {
            if (const auto m = TheText.Get("USJ_ALL")) {
                // ALL UNIQUE STUNTS COMPLETED!
                CHud::SetHelpMessage(m, false, false, false);
            }
        }

        // Generate random string for testing purposes
        if (const auto m = TheText.Get("REWARD")) {
            CMessages::AddBigMessageWithNumber(m, 6'000, STYLE_WHITE_MIDDLE_SMALLER, reward, -1, -1, -1, -1, -1);
        }

        m_jumpState = eJumpState::START_POINT_INTERSECTED;
        mp_Active   = nullptr;
        break;
        }
    }
}

// unused
// 0x49C370
void CStuntJumpManager::SetActive(bool active) {
    m_bActive = active;
}

// 0x0
void CStuntJumpManager::Render() {
    // NOP
}

// NOTSA
void ResetAllJumps() {
    for (auto jumpIndex = 0; jumpIndex < STUNT_JUMP_COUNT; jumpIndex++) {
        CStuntJump* jump = CStuntJumpManager::mp_poolStuntJumps->GetAt(jumpIndex);
        if (!jump)
            continue;

        jump->done = false;
        jump->found = false;
    }
}

// NOTSA
void StuntJumpTestCode() {
    CPad* pad = CPad::GetPad(0);
    if (pad->IsStandardKeyJustDown('1')) {
        DEV_LOG("ResetAllJumps");
        ResetAllJumps();
    }
    if (pad->IsStandardKeyJustDown('2')) {
        auto player = FindPlayerPed();
        if (player) {
            CVector posn{-2053.93848f, 236.598221f, 35.5952835f};
            player->SetPosn(posn);
            CCheat::VehicleCheat(MODEL_NRG500);
        }
    }
}
