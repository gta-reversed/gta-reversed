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
    m_bActive         = true;
}

// 0x49CBC0
void CStuntJumpManager::Shutdown() {
    if (mp_poolStuntJumps) {
        mp_poolStuntJumps->Flush();
    }

    mp_poolStuntJumps = nullptr;
}

// 0x49CB10
void CStuntJumpManager::ShutdownForRestart() {
    mp_poolStuntJumps->Clear(); // 0x49C9A0
    mp_Active       = nullptr;
    m_bHitReward    = false;
    m_iTimer        = 0;
    m_jumpState     = eJumpState::START_POINT_INTERSECTED;
    m_iNumJumps     = 0;
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
    for (auto i = m_iNumJumps; i --> 0;) {
        CGenericGameStorage::LoadDataFromWorkBuffer(*mp_poolStuntJumps->New());
    }
    return true;
}

// 0x49CB40
void CStuntJumpManager::AddOne(const CBoundingBox& start, const CBoundingBox& end, const CVector& camPos, int32 reward) {
    if (!mp_poolStuntJumps) {
        return;
    }
    const auto jump = new (mp_poolStuntJumps->New()) CStuntJump{
        .start  = start,
        .end    = end,
        .camera = camPos,
        .reward = reward,
        .done   = false,
        .found  = false,
    };
    if (jump) {
        ++m_iNumJumps;
    }
}

// 0x49C490
void CStuntJumpManager::Update() {
    if (!mp_poolStuntJumps || CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    const auto plyr = FindPlayerPed(-1);
    if (!plyr) {
        return;
    }

    const auto plyrInfo = plyr->GetPlayerInfoForThisPlayerPed();
    const auto plyrVeh  = plyr->m_pVehicle;
    if (!plyrVeh || !plyrInfo) {
        return;
    }

    switch (m_jumpState) {
    case eJumpState::START_POINT_INTERSECTED: { // 0x49C77A
        if (!m_bActive) {
            return;
        }
        if (plyrInfo->m_nPlayerState != PLAYERSTATE_PLAYING || !plyr->IsInVehicle()) {
            return;
        }
        if (notsa::contains({ VEHICLE_APPEARANCE_BOAT, VEHICLE_APPEARANCE_PLANE, VEHICLE_APPEARANCE_HELI }, plyrVeh->GetVehicleAppearance())) {
            return;
        }
        // 0x49C7DB:
        // Game sometimes miss successful jumps: it's somewhat rare but happens quite a lot
        // while speed-running. This check here is suspicious, small bumps may cause fail the jump.
        //
        // lordmau5 added a grace period allowing the vehicle to be airborne if any wheel
        // touched ground on his MTA script to fix.
        if (plyrVeh->m_nNumEntitiesCollided > 0) { 
            return;
        }
        if (!plyrVeh->IsDriver(plyr) || plyrVeh->m_vecMoveSpeed.Magnitude() * 50.f < 20.f) {
            return;
        }

        // Find a jump to activate
        for (auto& j : mp_poolStuntJumps->GetAllValid()) {
            if (!j.start.IsPointWithin(plyrVeh->GetPosition())) {
                continue;
            }

            m_jumpState  = eJumpState::IN_FLIGHT;
            mp_Active    = &j;
            m_iTimer     = 0;
            m_bHitReward = false;

            if (!mp_Active->found) {
                mp_Active->found = true;
                CStats::IncrementStat(STAT_UNIQUE_JUMPS_FOUND, 1.0f);
            }

            CTimer::SetTimeScale(0.3f);
            TheCamera.SetCamPositionForFixedMode(mp_Active->camera, CVector{ 0.f, 0.f, 0.f });
            TheCamera.TakeControl(plyrVeh, MODE_FIXED, eTransMode::TRANS_JUMP_CUT, 1);

            break;
        }
        break;
    }
    case eJumpState::IN_FLIGHT: { // 0x49C4E6
        if (!mp_Active) {
            m_jumpState = eJumpState::START_POINT_INTERSECTED;
            break;
        }

        const auto failed = plyrVeh->m_nNumEntitiesCollided != 0 && m_iTimer >= 100;
        if (!failed && mp_Active->end.IsPointWithin(plyrVeh->GetPosition())) {
            m_bHitReward = true;
        }

        const auto ended = failed
            || plyrInfo->m_nPlayerState != PLAYERSTATE_PLAYING
            || !plyr->bInVehicle
            || plyrVeh->m_nStatus == STATUS_WRECKED
            || plyrVeh->vehicleFlags.bIsDrowning
            || plyrVeh->physicalFlags.bSubmergedInWater;
        if (ended) {
            m_jumpState = eJumpState::END_POINT_INTERSECTED;
        }

        const auto time = ended ? 0 : m_iTimer;
        m_iTimer        = (uint32)CTimer::GetTimeStepInMS() + time;
        if (m_iTimer <= 1'000 || time > 1'000) {
            if (const auto plyrVeh = FindPlayerVehicle(-1, 0)) {
                if (const auto psgr = plyrVeh->PickRandomPassenger()) {
                    psgr->Say(CTX_GLOBAL_CAR_JUMP, 0, 1.0, 0, 0, 0);
                }
            }
        }

        break;
    }
    case eJumpState::END_POINT_INTERSECTED: { // 0x49C4ED
        m_iTimer += (uint32)CTimer::GetTimeStepInMS();
        if (m_iTimer < 300) {
            return;
        }

        CTimer::SetTimeScale(1.f);
        TheCamera.RestoreWithJumpCut();

        if (m_bHitReward && !mp_Active->done) {
            mp_Active->done = true;

            ++m_iNumCompleted;

            CStats::IncrementStat(STAT_UNIQUE_JUMPS_DONE, 1.f);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PART_MISSION_COMPLETE);

            const auto all    = m_iNumCompleted == m_iNumJumps;
            const auto reward = all
                ? 10'000
                : mp_Active->reward;
            plyrInfo->m_nMoney += reward;
            if (all) {
                if (const auto txt = TheText.Get("USJ_ALL")) { // ALL UNIQUE STUNTS COMPLETED!
                    CHud::SetHelpMessage(txt, false, false, false);
                }
            }
            if (const auto txt = TheText.Get("USJ")) { // UNIQUE STUNT BONUS!
                CMessages::AddBigMessageQ(txt, 5'000, STYLE_MIDDLE_SMALLER_HIGHER);
            }
            if (const auto txt = TheText.Get("REWARD")) { // REWARD
                CMessages::AddBigMessageWithNumber(txt, 6'000, STYLE_WHITE_MIDDLE_SMALLER, reward);
            }
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
    for (auto& j : CStuntJumpManager::mp_poolStuntJumps->GetAllValid()) {
        j.done  = false;
        j.found = false;
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
            CVector posn{ -2053.93848f, 236.598221f, 35.5952835f };
            player->SetPosn(posn);
            CCheat::VehicleCheat(MODEL_NRG500);
        }
    }
}
