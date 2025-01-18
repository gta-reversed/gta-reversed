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
void CStuntJumpManager::AddOne(const CBoundingBox& start, const CBoundingBox& end, const CVector& cameraPosn, int32 eReward) {
    if (mp_poolStuntJumps) {
        CStuntJump* jump = mp_poolStuntJumps->New();
        if (jump) {
            jump->start = start;
            jump->end = end;
            jump->camera = cameraPosn;
            jump->reward = eReward;
            jump->done = false;
            jump->found = false;
            ++m_iNumJumps;
        }
    }
}

// 0x49C490
void CStuntJumpManager::Update() {

    if (!CStuntJumpManager::mp_poolStuntJumps || CReplay::Mode == MODE_PLAYBACK)
        return;

    CPlayerPed *PlayerPed = FindPlayerPed(-1);
    if (!PlayerPed)
        return;

    CPlayerInfo *playerInfo = PlayerPed->GetPlayerInfoForThisPlayerPed();
    CVehicle *playerVehicle = PlayerPed->m_pVehicle;
    if (!playerVehicle || !playerInfo)
        return;

    if (CStuntJumpManager::m_jumpState == eJumpState::START_POINT_INTERSECTED && CStuntJumpManager::m_bActive) {
        if (playerInfo->m_nPlayerState != PLAYERSTATE_PLAYING || (PlayerPed->bInVehicle) == 0 || !playerVehicle)
            return;

        constexpr std::array invalidVehicles = {VEHICLE_APPEARANCE_BOAT, VEHICLE_APPEARANCE_PLANE, VEHICLE_APPEARANCE_HELI};
        if (std::ranges::find(invalidVehicles, playerVehicle->GetVehicleAppearance()) != invalidVehicles.end())
            return;

        // Game sometimes miss successful jumps: it's somewhat rare but happens quite a lot
        // while speedrunning. This check here is suspicious, small bumps may cause fail the jump.
        //
        // lordmau5 added a grace period allowing the vehicle to be airborne if any wheel
        // touched ground on his MTA script to fix.
        if (playerVehicle->m_nNumEntitiesCollided > 0 || // Check that vehicle is not colliding when starting the jump
            (playerVehicle->m_vecMoveSpeed.Magnitude() * 50.0 < 20.0))
            return;

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
            return;
        }
    } else if (CStuntJumpManager::m_jumpState == eJumpState::IN_FLIGHT) {
        if (!CStuntJumpManager::mp_Active) {
            CStuntJumpManager::m_jumpState = eJumpState::START_POINT_INTERSECTED;
            return;
        }

        int time;
        if ((playerVehicle->m_nNumEntitiesCollided > 0 && CStuntJumpManager::m_iTimer >= 100) ||
            (playerInfo->m_nPlayerState == PLAYERSTATE_HAS_DIED ||
            !PlayerPed->bInVehicle ||
            (playerVehicle->m_nStatus == STATUS_WRECKED) ||
            (playerVehicle->vehicleFlags.bIsDrowning) ||
            (playerVehicle->physicalFlags.bSubmergedInWater)))
        {
            CStuntJumpManager::m_jumpState = eJumpState::END_POINT_INTERSECTED;
            time = 0;
        } else {
            time = CStuntJumpManager::m_iTimer;
        }

        if (mp_Active->end.IsPointWithin(playerVehicle->GetPosition())) {
            CStuntJumpManager::m_bHitReward = true;
        }

        CStuntJumpManager::m_iTimer = static_cast<uint32_t>(CTimer::ms_fTimeStep * 0.02 * 1000) + time;
        if (CStuntJumpManager::m_iTimer <= 1000 || time > 1000)
            return;

        if (CVehicle* pPedVeh = FindPlayerVehicle(-1, 0)) {
            if (CPed *pPedRand = pPedVeh->PickRandomPassenger()) {
                pPedRand->Say(CTX_GLOBAL_CAR_JUMP, 0, 1.0, 0, 0, 0);
            }
        }
    } else if (CStuntJumpManager::m_jumpState == eJumpState::END_POINT_INTERSECTED) {
        CStuntJumpManager::m_iTimer += static_cast<uint32_t>(CTimer::ms_fTimeStep * 0.02 * 1000);
        if (CStuntJumpManager::m_iTimer < 300)
            return;

        CTimer::ms_fTimeScale = 1.0;
        TheCamera.RestoreWithJumpCut();
        if (CStuntJumpManager::m_bHitReward) {
            if (!CStuntJumpManager::mp_Active->done) {
                CStuntJumpManager::mp_Active->done = true;
                ++CStuntJumpManager::m_iNumCompleted;
                CStats::IncrementStat(STAT_UNIQUE_JUMPS_DONE, 1.0);
                int eReward;
                if (CStuntJumpManager::m_iNumCompleted == CStuntJumpManager::m_iNumJumps) {
                    eReward = 10000;
                    // ALL UNIQUE STUNTS COMPLETED!
                    if (const auto tUsjAll = TheText.Get("USJ_ALL")) {
                        CHud::SetHelpMessage(tUsjAll, false, false, false);
                    }
                } else {
                    eReward = CStuntJumpManager::mp_Active->reward;
                }
                playerInfo->m_nMoney += eReward;
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE, 0.0, 1.0);

                // UNIQUE STUNT BONUS!
                if (const auto tUsj = TheText.Get("USJ")) {
                    CMessages::AddBigMessageQ(tUsj, 5'000, STYLE_MIDDLE_SMALLER_HIGHER);
                }

                // REWARD
                if (const auto tReward = TheText.Get("REWARD")) {
                    CMessages::AddBigMessageWithNumber(tReward, 6000, STYLE_WHITE_MIDDLE_SMALLER, eReward, -1, -1, -1, -1, -1);
                }
            }
        }
        CStuntJumpManager::m_jumpState = eJumpState::START_POINT_INTERSECTED;
        CStuntJumpManager::mp_Active = 0;
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
