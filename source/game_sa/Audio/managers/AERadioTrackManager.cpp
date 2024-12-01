#include "StdInc.h"

#include "AERadioTrackManager.h"
#include "RadioStreamsPC.h"

#include "AEAudioHardware.h"
#include "AEUserRadioTrackManager.h"
#include "AEAudioUtility.h"
#include "AEAudioEnvironment.h"

CAERadioTrackManager& AERadioTrackManager = *(CAERadioTrackManager*)0x8CB6F8;

uint8& CAERadioTrackManager::m_nStatsLastHitTimeOutHours = *(uint8*)0xB62C58;
uint8& CAERadioTrackManager::m_nStatsLastHitGameClockHours = *(uint8*)0xB62C59;
uint8& CAERadioTrackManager::m_nStatsLastHitGameClockDays = *(uint8*)0xB62C5A;
uint8& CAERadioTrackManager::m_nStatsStartedCrash1 = *(uint8*)0xB62C5B;
uint8& CAERadioTrackManager::m_nStatsStartedCat2 = *(uint8*)0xB62C5C;
uint8& CAERadioTrackManager::m_nStatsStartedBadlands = *(uint8*)0xB62C5D;
uint8& CAERadioTrackManager::m_nStatsPassedVCrash2 = *(uint8*)0xB62C5E;
uint8& CAERadioTrackManager::m_nStatsPassedTruth2 = *(uint8*)0xB62C5F;
uint8& CAERadioTrackManager::m_nStatsPassedSweet2 = *(uint8*)0xB62C60;
uint8& CAERadioTrackManager::m_nStatsPassedStrap4 = *(uint8*)0xB62C61;
uint8& CAERadioTrackManager::m_nStatsPassedSCrash1 = *(uint8*)0xB62C62;
uint8& CAERadioTrackManager::m_nStatsPassedRiot1 = *(uint8*)0xB62C63;
uint8& CAERadioTrackManager::m_nStatsPassedRyder2 = *(uint8*)0xB62C64;
uint8& CAERadioTrackManager::m_nStatsPassedMansion2 = *(uint8*)0xB62C65;
uint8& CAERadioTrackManager::m_nStatsPassedLAFin2 = *(uint8*)0xB62C66;
uint8& CAERadioTrackManager::m_nStatsPassedFarlie3 = *(uint8*)0xB62C67;
uint8& CAERadioTrackManager::m_nStatsPassedDesert10 = *(uint8*)0xB62C68;
uint8& CAERadioTrackManager::m_nStatsPassedDesert8 = *(uint8*)0xB62C69;
uint8& CAERadioTrackManager::m_nStatsPassedDesert5 = *(uint8*)0xB62C6A;
uint8& CAERadioTrackManager::m_nStatsPassedDesert3 = *(uint8*)0xB62C6B;
uint8& CAERadioTrackManager::m_nStatsPassedDesert1 = *(uint8*)0xB62C6C;
uint8& CAERadioTrackManager::m_nStatsPassedCat1 = *(uint8*)0xB62C6D;
uint8& CAERadioTrackManager::m_nStatsPassedCasino10 = *(uint8*)0xB62C6E;
uint8& CAERadioTrackManager::m_nStatsPassedCasino6 = *(uint8*)0xB62C6F;
uint8& CAERadioTrackManager::m_nStatsPassedCasino3 = *(uint8*)0xB62C70;
uint8& CAERadioTrackManager::m_nStatsCitiesPassed = *(uint8*)0xB62C71;
uint8& CAERadioTrackManager::m_nSpecialDJBanterIndex = *(uint8*)0xB62C72;
uint8& CAERadioTrackManager::m_nSpecialDJBanterPending = *(uint8*)0xB62C73;

void CAERadioTrackManager::InjectHooks() {
    RH_ScopedClass(CAERadioTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Load, 0x5D40E0, { .enabled = LOAD_HOOKS_ENABLED });
    RH_ScopedInstall(Save, 0x5D3EE0, { .enabled = SAVE_HOOKS_ENABLED });
    RH_ScopedInstall(Initialise, 0x5B9390);
    RH_ScopedInstall(Service, 0x4EB9A0, { .reversed = false });
    RH_ScopedInstall(DisplayRadioStationName, 0x4E9E50);
    RH_ScopedInstall(CheckForStationRetune, 0x4EB660, { .reversed = false });
    RH_ScopedInstall(CheckForPause, 0x4EA590);
    RH_ScopedInstall(IsVehicleRadioActive, 0x4E9800, { .reversed = false });
    RH_ScopedInstall(AddDJBanterIndexToHistory, 0x4E97B0);
    RH_ScopedInstall(AddAdvertIndexToHistory, 0x4E9760);
    RH_ScopedInstall(AddIdentIndexToHistory, 0x4E9720);
    RH_ScopedInstall(AddMusicTrackIndexToHistory, 0x4E96C0);
    RH_ScopedOverloadedInstall(StartRadio, "manual", 0x4EB3C0, void (CAERadioTrackManager::*)(eRadioID, int8, float, uint8));
    RH_ScopedOverloadedInstall(StartRadio, "with-settings", 0x4EB550, void (CAERadioTrackManager::*)(tVehicleAudioSettings*));
    RH_ScopedInstall(CheckForStationRetuneDuringPause, 0x4EB890);
    RH_ScopedInstall(TrackRadioStation, 0x4EAC30, { .reversed = false });
    RH_ScopedInstall(ChooseTracksForStation, 0x4EB180);
    RH_ScopedInstall(CheckForTrackConcatenation, 0x4EA930, { .reversed = false });
    RH_ScopedInstall(QueueUpTracksForStation, 0x4EA670, { .reversed = false });
    RH_ScopedInstall(ChooseDJBanterIndex, 0x4EA2D0, { .reversed = false });
    RH_ScopedInstall(ChooseDJBanterIndexFromList, 0x4E95E0, { .reversed = false });
    RH_ScopedInstall(ChooseAdvertIndex, 0x4E9570, { .reversed = false });
    RH_ScopedInstall(ChooseIdentIndex, 0x4E94C0, { .reversed = false });
    RH_ScopedInstall(ChooseMusicTrackIndex, 0x4EA270, { .reversed = false });
    RH_ScopedInstall(ChooseTalkRadioShow, 0x4E8E40, { .reversed = false });
    RH_ScopedInstall(CheckForMissionStatsChanges, 0x4E8410);
    RH_ScopedInstall(StartTrackPlayback, 0x4EA640);
    RH_ScopedInstall(UpdateRadioVolumes, 0x4EA010, { .reversed = false });
    RH_ScopedInstall(PlayRadioAnnouncement, 0x4E8400);
    RH_ScopedInstall(GetCurrentRadioStationID, 0x4E83F0);
    RH_ScopedInstall(GetRadioStationListenTimes, 0x4E83E0);
    RH_ScopedInstall(GetRadioStationName, 0x4E9E10);
    RH_ScopedInstall(GetRadioStationNameKey, 0x4E8380);
    RH_ScopedInstall(HasRadioRetuneJustStarted, 0x4E8370);
    RH_ScopedInstall(StopRadio, 0x4E9820, { .reversed = false });
    RH_ScopedInstall(IsRadioOn, 0x4E8350, { .reversed = true });
    RH_ScopedInstall(InitialiseRadioStationID, 0x4E8330);
    RH_ScopedInstall(SetBassEnhanceOnOff, 0x4E9DB0);
    RH_ScopedInstall(SetBassSetting, 0x4E82F0);
    RH_ScopedInstall(SetRadioAutoRetuneOnOff, 0x4E82E0);
    RH_ScopedInstall(RetuneRadio, 0x4E8290, { .reversed = true });
    RH_ScopedInstall(ResetStatistics, 0x4E8200);
    RH_ScopedInstall(Reset, 0x4E7F80, { .reversed = true });
}

// Code from 0x5B9390
CAERadioTrackManager::CAERadioTrackManager(int32 hwClientHandle) :
    m_HwClientHandle{ hwClientHandle },
    m_nUserTrackPlayMode{ AEUserRadioTrackManager.GetUserTrackPlayMode() }
{
    // All constant value inits are done using member init lists

    // NOTSA: SA gets the list via CStats::GetFullFavoriteRadioStationList() but this way is much more clear.
    rng::copy(CStats::FavoriteRadioStationList, m_aListenTimes);

    for (auto i = 0u; i < RADIO_COUNT; i++) {
        m_nMusicTrackIndexHistory[i].Reset();
        m_nDJBanterIndexHistory[i].Reset();
        m_nAdvertIndexHistory[i].Reset();
        m_nIdentIndexHistory[i].Reset();
    }

    // [1st radio, off]
    m_RequestedSettings = m_ActiveSettings = tRadioSettings{CAEAudioUtility::GetRandomRadioStation()};
}

// 0x5B9390
bool CAERadioTrackManager::Initialise(int32 channelId) {
    *this = CAERadioTrackManager{};
    return true;
}

// 0x4E8330
void CAERadioTrackManager::InitialiseRadioStationID(eRadioID id) {
    m_RequestedSettings.m_nCurrentRadioStation = m_ActiveSettings.m_nCurrentRadioStation = id;
}

// 0x4E7F80
void CAERadioTrackManager::Reset() {
    m_bInitialised = false;
    m_bDisplayStationName = false;
    rng::copy(CStats::FavoriteRadioStationList, m_aListenTimes);

    rng::for_each(m_nDJBanterIndexHistory, &DJBanterIndexHistory::Reset);
    rng::for_each(m_nAdvertIndexHistory, &AdvertIndexHistory::Reset);
    rng::for_each(m_nIdentIndexHistory, &IdentIndexHistory::Reset);
    rng::for_each(m_nMusicTrackIndexHistory, &MusicTrackHistory::Reset);
    rng::for_each(m_aRadioState, [](auto& s) { s.Reset(); });

    m_RequestedSettings = m_ActiveSettings = tRadioSettings{CAEAudioUtility::GetRandomRadioStation()};
    m_nStationsListed = m_nStationsListDown = 0;
    m_nTimeRadioStationRetuned = m_nTimeToDisplayRadioName = 0;
    m_prev = field_60 = 0;
    m_nRetuneStartedTime = 0;
    m_bEnabledInPauseMode = false;
    m_nSavedGameClockDays = m_nSavedGameClockHours = -1;
    m_bRadioAutoSelect = m_bBassEnhance = true;
    m_nSavedRadioStationId = m_iRadioStationMenuRequest = m_iRadioStationScriptRequest = RADIO_INVALID;
    m_nSpecialDJBanterPending = 3; // todo: enum
    m_nSpecialDJBanterIndex = -1;
    m_bPauseMode = m_bRetuneJustStarted = false;
    m_f80 = m_f84 = 0.0f;
    ResetStatistics();
}

// 0x4E8200
void CAERadioTrackManager::ResetStatistics() {
    m_nStatsCitiesPassed = 0;
    m_nStatsLastHitGameClockDays = -1;
    m_nStatsLastHitGameClockHours = -1;
    m_nStatsLastHitTimeOutHours = -1;
    m_nStatsPassedCasino3 = false;
    m_nStatsPassedCasino6 = false;
    m_nStatsPassedCasino10 = false;
    m_nStatsPassedCat1 = false;
    m_nStatsPassedDesert1 = false;
    m_nStatsPassedDesert3 = false;
    m_nStatsPassedDesert5 = false;
    m_nStatsPassedDesert8 = false;
    m_nStatsPassedDesert10 = false;
    m_nStatsPassedFarlie3 = false;
    m_nStatsPassedLAFin2 = false;
    m_nStatsPassedMansion2 = false;
    m_nStatsPassedRyder2 = false;
    m_nStatsPassedRiot1 = false;
    m_nStatsPassedSCrash1 = false;
    m_nStatsPassedStrap4 = false;
    m_nStatsPassedSweet2 = false;
    m_nStatsPassedTruth2 = false;
    m_nStatsPassedVCrash2 = false;
    m_nStatsStartedBadlands = false;
    m_nStatsStartedCat2 = false;
    m_nStatsStartedCrash1 = false;
}

// 0x4E8350
bool CAERadioTrackManager::IsRadioOn() const {
    return m_nMode != eRadioTrackMode::RADIO_STOPPED || m_bInitialised || m_nStationsListed || m_nStationsListDown;
}

// 0x4E8370
bool CAERadioTrackManager::HasRadioRetuneJustStarted() const {
    return m_bRetuneJustStarted;
}

// 0x4E83E0
int32* CAERadioTrackManager::GetRadioStationListenTimes() {
    return m_aListenTimes;
}

// 0x4E83F0
eRadioID CAERadioTrackManager::GetCurrentRadioStationID() const {
    return m_RequestedSettings.m_nCurrentRadioStation == RADIO_INVALID ? RADIO_OFF : m_RequestedSettings.m_nCurrentRadioStation;
}

// 0x4E82E0
void CAERadioTrackManager::SetRadioAutoRetuneOnOff(bool enable) {
    m_bRadioAutoSelect = enable;
}

// 0x4E82F0
void CAERadioTrackManager::SetBassSetting(int8 nBassSet, float fBassGrain) {
    m_RequestedSettings.m_fBassGain = m_ActiveSettings.m_fBassGain = fBassGrain;
    m_RequestedSettings.m_nBassSet = m_ActiveSettings.m_nBassSet = nBassSet;
    AEAudioHardware.SetBassSetting(m_bBassEnhance ? nBassSet : 0, fBassGrain);
}

// 0x4E9DB0
void CAERadioTrackManager::SetBassEnhanceOnOff(bool enable) {
    m_bBassEnhance = enable;
    if (m_nMode == eRadioTrackMode::RADIO_PLAYING) {
        m_RequestedSettings.m_nBassSet = m_ActiveSettings.m_nBassSet;
        m_RequestedSettings.m_fBassGain = m_ActiveSettings.m_fBassGain;
        if (enable) {
            AEAudioHardware.SetBassSetting(m_ActiveSettings.m_nBassSet, m_ActiveSettings.m_fBassGain);
        } else {
            AEAudioHardware.SetBassSetting(0, m_ActiveSettings.m_fBassGain);
        }
    }
}

// 0x4E8290
void CAERadioTrackManager::RetuneRadio(eRadioID id) {
    const auto retunedStation = [id] {
        if (id == RADIO_USER_TRACKS && !AEUserRadioTrackManager.m_nUserTracksCount) {
            return RADIO_OFF;
        } else {
            return id;
        }
    }();

    if (CTimer::GetIsPaused()) {
        m_iRadioStationMenuRequest = retunedStation;
        m_nRetuneStartedTime = CTimer::GetTimeInMSPauseMode();
    } else {
        m_iRadioStationScriptRequest = retunedStation;
    }
}

// 0x4E9E50
void CAERadioTrackManager::DisplayRadioStationName() {
    if (CTimer::GetIsPaused())
        return;

    if (TheCamera.m_bWideScreenOn)
        return;

    if (!FindPlayerVehicle())
        return;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (m_bDisplayStationName && IsVehicleRadioActive()) {
        m_nTimeToDisplayRadioName = CTimer::GetTimeInMS() + 2500;
        m_bDisplayStationName = false;
    }

    if (CTimer::GetTimeInMS() < m_nTimeToDisplayRadioName) {
        int station = m_nStationsListed + m_RequestedSettings.m_nCurrentRadioStation;
        if (station) {
            if (station >= RADIO_COUNT) {
                station -= RADIO_COUNT - 1;
            } else if (station <= 0) {
                station += RADIO_COUNT - 1;
            }

            CFont::SetFontStyle(eFontStyle::FONT_MENU);
            CFont::SetJustify(false);
            CFont::SetBackground(false, false);
            CFont::SetScale(SCREEN_SCALE_X(0.6f), SCREEN_SCALE_Y(0.9f));
            CFont::SetProportional(true);
            CFont::SetOrientation(eFontAlignment::ALIGN_CENTER);
            CFont::SetRightJustifyWrap(0.0f);
            CFont::SetEdge(1);
            CFont::SetDropColor(CRGBA(0, 0, 0, 255));
            CFont::SetColor(HudColour.GetRGB(m_nStationsListed || m_nStationsListDown ? HUD_COLOUR_DARK_GRAY : HUD_COLOUR_GOLD));
            CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_SCALE_Y(22.0f), GetRadioStationName((eRadioID)station));
            CFont::DrawFonts();
        }
    }
}

// 0x4E9E10
const GxtChar* CAERadioTrackManager::GetRadioStationName(eRadioID id) {
    if (id <= 0) {
        NOTSA_UNREACHABLE();
        return nullptr;
    }

    char key[8];
    GetRadioStationNameKey(id, key);
    return TheText.Get(key);
}

// 0x4E8380
void CAERadioTrackManager::GetRadioStationNameKey(eRadioID id, char* outStr) {
    switch (id) {
    case RADIO_OFF:
        *std::format_to_n(outStr, 7u, "FEA_NON").out = '\0';
        break;
    case RADIO_USER_TRACKS:
        *std::format_to_n(outStr, 7u, "FEA_MP3").out = '\0';
        break;
    default:
        assert(0 <= id && id < RADIO_USER_TRACKS);
        *std::format_to_n(outStr, 7u, "FEA_R{:d}", (int32)id - 1).out = '\0';
        break;
    }
}

// 0x4E9800
bool CAERadioTrackManager::IsVehicleRadioActive() {
    if (const auto opts = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio()) {
        switch (opts->m_nRadioType) {
        case RADIO_CIVILIAN:
        case RADIO_EMERGENCY:
        case RADIO_UNKNOWN:
            return true;
        default:
            break;
        }
    }
    return false;
}

// 0x4E8410
void CAERadioTrackManager::CheckForMissionStatsChanges() {
    if (m_nSpecialDJBanterPending != 3) {
        int32 days = CClock::GetGameClockDays() - m_nStatsLastHitGameClockDays;
        if (days < 0) {
            auto month = CClock::GetGameClockMonth() - 1;
            if (month < 0) {
                month += 12;
            }
            days += CClock::daysInMonth[month];
        }
        if (CClock::GetGameClockHours() + 24 * days - m_nStatsLastHitGameClockHours >= m_nStatsLastHitTimeOutHours) {
            m_nSpecialDJBanterPending = 3;
        }
    }

    const auto statsCitiesPassed = CStats::GetStatValue<uint8>(STAT_CITY_UNLOCKED);
    if (m_nStatsCitiesPassed < statsCitiesPassed) {
        m_nStatsCitiesPassed = statsCitiesPassed;
        if (statsCitiesPassed == 1 || statsCitiesPassed == 2) {
            m_nStatsLastHitGameClockDays = CClock::GetGameClockDays();
            m_nStatsLastHitGameClockHours = CClock::GetGameClockHours();
            m_nStatsLastHitTimeOutHours = 24;
            m_nSpecialDJBanterPending = 1;
            m_nSpecialDJBanterIndex = m_nStatsCitiesPassed - 1;
        }
    }

    const auto Update = [](uint8& inputStat, const eStats stat, const auto specialDJBanterIndex) {
        const auto statValue = CStats::GetStatValue<uint8>(stat);
        if (inputStat < statValue) {
            inputStat = statValue;
            if (inputStat == 1) {
                m_nStatsLastHitGameClockDays = CClock::GetGameClockDays();
                m_nStatsLastHitGameClockHours = CClock::GetGameClockHours();
                m_nStatsLastHitTimeOutHours = 24 * 7;
                m_nSpecialDJBanterPending = 2;
                m_nSpecialDJBanterIndex = specialDJBanterIndex;
            }
        }
    };

    Update(m_nStatsPassedCasino3, STAT_LEAST_FAVORITE_RADIO_STATION, 0);
    Update(m_nStatsPassedCasino6, STAT_CURRENT_WEAPON_SKILL, 1);
    Update(m_nStatsPassedCasino10, STAT_WEAPON_SKILL_LEVELS, 2);
    Update(m_nStatsPassedCat1, STAT_LOCAL_LIQUOR_STORE_MISSION_ACCOMPLISHED, 3);
    Update(m_nStatsPassedDesert1, STAT_PLAYING_TIME, 4);
    Update(m_nStatsPassedDesert3, STAT_PILOT_RANKING, 5);
    Update(m_nStatsPassedDesert5, STAT_STRONGEST_GANG, 6);
    Update(m_nStatsPassedDesert8, STAT_2ND_STRONGEST_GANG, 7);
    Update(m_nStatsPassedDesert10, STAT_3RD_STRONGEST_GANG, 8);
    Update(m_nStatsPassedFarlie3, STAT_MIKE_TORENO_MISSION_ACCOMPLISHED, 9);
    Update(m_nStatsPassedLAFin2, STAT_LEAST_FAVORITE_GANG, 10);
    Update(m_nStatsPassedMansion2, STAT_A_HOME_IN_THE_HILLS_MISSION_ACCOMPLISHED, 11);
    Update(m_nStatsPassedRyder2, STAT_RYDERS_MISSION_ROBBING_UNCLE_SAM_ACCOMPLISHED, 12);
    Update(m_nStatsPassedRiot1, STAT_RIOT_MISSION_ACCOMPLISHED, 13);
    Update(m_nStatsPassedSCrash1, STAT_GANG_STRENGTH, 14);
    Update(m_nStatsPassedStrap4, STAT_TERRITORY_UNDER_CONTROL, 15);
    Update(m_nStatsPassedSweet2, STAT_DRIVE_THRU_MISSION_ACCOMPLISHED, 16);
    Update(m_nStatsPassedTruth2, STAT_ARE_YOU_GOING_TO_SAN_FIERRO_MISSION_ACCOMPLISHED, 17);
    Update(m_nStatsPassedVCrash2, STAT_HIGH_NOON_MISSION_ACCOMPLISHED, 18);
    Update(m_nStatsStartedBadlands, STAT_THE_GREEN_SABRE_MISSION_ACCOMPLISHED, 19);
    Update(m_nStatsStartedCat2, STAT_MAYBE_CATALINA_MEETING, 20);
    Update(m_nStatsStartedCrash1, STAT_MAYBE_WU_ZI_MEETING, 21);
}

// 0x4EA930
void CAERadioTrackManager::CheckForTrackConcatenation() {
    plugin::CallMethod<0x4EA930, CAERadioTrackManager*>(this);
    /*
    const auto utPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode();
    if (m_ActiveSettings.m_nCurrentRadioStation == RADIO_USER_TRACKS && utPlayMode != 0) {
        if (utPlayMode == 2 && m_ActiveSettings.m_iTrackPlayTime != -4) { // ???
            AEUserRadioTrackManager.SetUserTrackIndex(m_ActiveSettings.m_aTrackQueue.front());

            m_ActiveSettings.m_aTrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
            m_ActiveSettings.m_aTrackTypes[1] = TYPE_USER_TRACK;
            m_ActiveSettings.m_aTrackIndexes[1] = m_ActiveSettings.m_aTrackQueue[1];

            AEAudioHardware.PlayTrack(
                m_ActiveSettings.m_aTrackQueue[0],
                m_ActiveSettings.m_aTrackQueue[1],
                0u,
                m_ActiveSettings.m_nTrackFlags,
                m_ActiveSettings.m_aTrackTypes[0] == TYPE_USER_TRACK,
                m_ActiveSettings.m_aTrackTypes[1] == TYPE_USER_TRACK // always true?
            );
        }
        m_nUserTrackPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode();
    }

    const auto nextTrack = m_ActiveSettings.m_aTrackQueue[1];
    if (AEAudioHardware.GetActiveTrackID() == nextTrack && nextTrack >= 0) {
        m_ActiveSettings.SwitchToNextTrack();

        if (m_ActiveSettings.m_aTrackQueue[1] == -1) {
            const auto radioId = m_ActiveSettings.m_nCurrentRadioStation;
            if (radioId == RADIO_USER_TRACKS) {
                if (!FrontEndMenuManager.m_nRadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
                    m_ActiveSettings.m_aTrackQueue
                }
            }
        }
    }
    */
}

// 0x4EB660
void CAERadioTrackManager::CheckForStationRetune() {
    plugin::CallMethod<0x4EB660, CAERadioTrackManager*>(this);
}

// 0x4EB890
void CAERadioTrackManager::CheckForStationRetuneDuringPause() {
    if (m_ActiveSettings.m_nCurrentRadioStation == RADIO_EMERGENCY_AA && IsRadioOn() || m_iRadioStationMenuRequest <= RADIO_INVALID)
        return;

    if (m_iRadioStationMenuRequest != RADIO_OFF) {
        if (m_ActiveSettings.m_nCurrentRadioStation == RADIO_OFF) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON);
            m_ActiveSettings.m_nCurrentRadioStation = RADIO_INVALID;
        } else {
            AudioEngine.StopRadio(nullptr, true);
        }

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        if (CTimer::GetTimeInMSPauseMode() > m_nRetuneStartedTime + 700u) {
            StartRadio((eRadioID)m_iRadioStationMenuRequest, m_ActiveSettings.m_nBassSet, m_ActiveSettings.m_fBassGain, 0);
            m_iRadioStationMenuRequest = RADIO_INVALID;
        }
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        StartRadio(RADIO_OFF, m_ActiveSettings.m_nBassSet, m_ActiveSettings.m_fBassGain, 0);
        m_iRadioStationMenuRequest = RADIO_INVALID;
    }
}

// 0x4EA640
void CAERadioTrackManager::StartTrackPlayback() {
    AEAudioHardware.SetChannelFlags(m_HwClientHandle, 0, 55);
    AEAudioHardware.StartTrackPlayback();
    UpdateRadioVolumes();
}

// 0x4EA010
void CAERadioTrackManager::UpdateRadioVolumes() {
    plugin::CallMethod<0x4EA010, CAERadioTrackManager*>(this);

    /*
    const auto fEffectsScalingFactor = AEAudioHardware.GetEffectsMasterScalingFactor();
    const auto fMusicScalingFactor = AEAudioHardware.GetMusicMasterScalingFactor();

    auto volume = -4.0f;
    if (!CTimer::GetIsPaused() || !m_bEnabledInPauseMode) {
        if (CTimer::GetIsSlowMotionActive()) {
            volume = -100.0f;
        }
        else if (TheCamera.m_bWideScreenOn) {
            volume = -16.0f;
        }
        else if (fEffectsScalingFactor > 0.0f && fMusicScalingFactor > 0.0f) {
            if (!CAEPedSpeechAudioEntity::s_bForceAudible) {
                auto audioEvent = 0;
                while (true) {
                    if (!AudioEngine.IsMissionAudioSampleFinished(audioEvent) && AudioEngine.GetMissionAudioEvent(audioEvent) != 0xFFFF) {
                        auto missionAudioPosition = AudioEngine.GetMissionAudioPosition(audioEvent);
                        if (!missionAudioPosition)
                            break;

                        CAEAudioEnvironment::GetPositionRelativeToCamera(v9, missionAudioPosition);
                        if (CVector::Magnitude(&v9) <= 15.0f)
                            break;
                    }
                    if (++audioEvent >= 2) {
                        if (m_f80 >= 0.0f)
                            goto LABEL_22;

                        auto v4 = m_f84 + m_f80;
                        if (v4 >= 0.0f)
                            v4 = 0.0f;
                        m_f80 = v4;
                        goto LABEL_21;
                    }
                }
            }

            volumea = fEffectsScalingFactor;
            if (__FYL2X__(volumea / fMusicScalingFactor, 0.30102999566398119802) * 20.0f - 9.0f >= 0.0f) {
                v4 = 0.0f;
            } else {
                volumeb = fEffectsScalingFactor;
                v4 = __FYL2X__(volumeb / fMusicScalingFactor, 0.30102999566398119802) * 20.0f - 9.0f;
            }
            m_f80 = v4;
            m_f84 = -0.02 * v4;
        LABEL_21:
            volume = v4 - 4.0f;
        }
    LABEL_22:
        if (AudioEngine.IsAmbienceRadioActive())
            volume = volume - 20.0f;
    }

    if (m_bBassEnhance && m_ActiveSettings.m_nBassSet) {
        switch (m_ActiveSettings.m_nBassSet) {
        case 1:
            volume -= 2.0f;
            break;
        case 2:
            volume += 1.5f;
            break;
        }
    }

    AEAudioHardware.SetChannelVolume(m_nChannel, 0, volume, 0);
    */
}

// 0x4E8400
void CAERadioTrackManager::PlayRadioAnnouncement(uint32) {
    // NOP
}

// 0x4EB550
void CAERadioTrackManager::StartRadio(tVehicleAudioSettings* settings) {
    // plugin::CallMethod<0x4EB550, CAERadioTrackManager*, tVehicleAudioSettings*>(this, settings);

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (settings->m_nRadioType == RADIO_EMERGENCY) {
        StartRadio(RADIO_EMERGENCY_AA, settings->m_nBassSetting, settings->m_fBassEq, 0);
        return;
    }

    if (settings->m_nRadioType != RADIO_CIVILIAN)
        return;

    const bool needsRetune = [&] {
       if (!m_bRadioAutoSelect)
           return false;

       const auto savedId = m_nSavedRadioStationId;
       if (savedId < 0 || savedId == settings->m_nRadioID || savedId == RADIO_OFF || savedId == RADIO_EMERGENCY_AA)
           return false;

       if (CTimer::GetTimeInMS() > m_nSavedTimeMs + 60'000)
           return false;

       const auto savedHours = m_nSavedGameClockHours;
       auto savedDays = m_nSavedGameClockDays;
       if (savedHours < 0 || savedDays < 0)
           return false;

       if (savedDays > CClock::GetGameClockDays()) {
           const auto month = CClock::GetGameClockMonth();
           savedDays += CClock::daysInMonth[month == 0 ? 11 : month - 1]; // prev month
       }

       if (CClock::GetGameClockHours() + 24 * savedDays - savedHours > 5)
           return false;

       return true;
    }();

    if (needsRetune) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        StartRadio((eRadioID)m_nSavedRadioStationId, settings->m_nBassSetting, settings->m_fBassEq, 0);
    } else {
        StartRadio(settings->m_nRadioID, settings->m_nBassSetting, settings->m_fBassEq, 0);
    }
}

// 0x4EB3C0
void CAERadioTrackManager::StartRadio(eRadioID id, int8 bassValue, float bassGain, uint8 a5) {
    // plugin::CallMethod<0x4EB3C0, CAERadioTrackManager*, int8, int8, float, uint8>(this, id, bassValue, bassGain, a5);
    id = std::min(id, RADIO_OFF);

    if (CTimer::GetIsPaused()) {
        m_bEnabledInPauseMode = true;

        if (IsRadioOn() && id == m_ActiveSettings.m_nCurrentRadioStation) {
            m_aRadioState[id].m_iTimeInPauseModeInMs = CTimer::GetTimeInMSPauseMode();
            return;
        }
    }

    if (id != RADIO_OFF && CAudioEngine::IsAmbienceTrackActive()) {
        if (!CTimer::GetIsPaused() && CAudioEngine::DoesAmbienceTrackOverrideRadio()) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            return;
        }
        AudioEngine.StopAmbienceTrack(false);
    }

    m_RequestedSettings.m_nCurrentRadioStation = id;
    m_RequestedSettings.m_nBassSet = bassValue;
    m_RequestedSettings.m_fBassGain = bassGain;

    if (id == RADIO_OFF) {
        m_RequestedSettings.Reset();
    } else if (m_aRadioState[id].m_iTimeInMs < 0 || !TrackRadioStation(id, a5)) {
        ChooseTracksForStation(m_RequestedSettings.m_nCurrentRadioStation);
        m_RequestedSettings.m_iTrackPlayTime = CAEAudioUtility::GetRandomNumberInRange(0, 300'000);
    }

    m_bInitialised = true;
    switch (m_nMode) {
    case eRadioTrackMode::RADIO_STARTING:
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
    case eRadioTrackMode::RADIO_PLAYING:
        m_nMode = eRadioTrackMode::RADIO_STOPPING;
        break;
    default:
        break;
    }
    m_aRadioState[m_RequestedSettings.m_nCurrentRadioStation].m_iTimeInPauseModeInMs = -1;
}

// 0x4EAC30
bool CAERadioTrackManager::TrackRadioStation(eRadioID id, uint8 a2) {
    return plugin::CallMethodAndReturn<bool, 0x4EAC30, CAERadioTrackManager*, int8, uint8>(this, id, a2);
}

// 0x4EA670
bool CAERadioTrackManager::QueueUpTracksForStation(eRadioID id, int8* iTrackCount, int8 radioState, tRadioSettings& settings) {
    return plugin::CallMethodAndReturn<bool, 0x4EA670, CAERadioTrackManager*, int8, int8*, int8, tRadioSettings&>(this, id, iTrackCount, radioState, settings);
}

// 0x4E9820
void CAERadioTrackManager::StopRadio(tVehicleAudioSettings* settings, bool duringPause) {
    return plugin::CallMethod<0x4E9820, CAERadioTrackManager*, tVehicleAudioSettings*, bool>(this, settings, duringPause);
}

// 0x4E94C0
int32 CAERadioTrackManager::ChooseIdentIndex(eRadioID id) {
    return plugin::CallAndReturn<int32, 0x4E94C0, CAERadioTrackManager*, int8>(this, id);
}

// 0x4E9570
int32 CAERadioTrackManager::ChooseAdvertIndex(eRadioID id) {
    return plugin::CallAndReturn<int32, 0x4E9570, CAERadioTrackManager*, int8>(this, id);
}

// 0x4EA270
int8 CAERadioTrackManager::ChooseMusicTrackIndex(eRadioID id) {
    return plugin::CallAndReturn<int8, 0x4EA270, CAERadioTrackManager*, int8>(this, id);
}

// 0x4EA2D0
int32 CAERadioTrackManager::ChooseDJBanterIndex(eRadioID id) {
    return plugin::CallAndReturn<int32, 0x4EA2D0, CAERadioTrackManager*, int8>(this, id);
}

// 0x4E95E0
int32 CAERadioTrackManager::ChooseDJBanterIndexFromList(eRadioID id, int32** list) {
    return plugin::CallMethodAndReturn<int32, 0x4E95E0, CAERadioTrackManager*, eRadioID, int32**>(this, id, list);
}

// 0x4EB180
void CAERadioTrackManager::ChooseTracksForStation(eRadioID id) {
    int8 trackCount = 0;

    m_RequestedSettings.Reset();
    if (!CAEAudioUtility::ResolveProbability(0.95f)) {
        if (id) {
            if (CAEAudioUtility::ResolveProbability(0.5f))
                QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);

            if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings))
                QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);

            if (id == RADIO_USER_TRACKS) {
                QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
                return;
            }
        } else {
            QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    if (id == RADIO_USER_TRACKS) {
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        if (!FrontEndMenuManager.m_nRadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_ADVERT, m_RequestedSettings);
        }
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.9f)) {
        QueueUpTracksForStation(id, &trackCount, TYPE_TRACK, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    QueueUpTracksForStation(id, &trackCount, TYPE_OUTRO, m_RequestedSettings);
    AddMusicTrackIndexToHistory(id, m_RequestedSettings.m_aTrackIndexes[trackCount - 1]);

    if (id == RADIO_EMERGENCY_AA) {
        QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f))
        QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);

    if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings))
        QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);
}

// 0x4E8E40
int8 CAERadioTrackManager::ChooseTalkRadioShow() {
    return plugin::CallAndReturn<int8, 0x4E8E40>();
}

// 0x4E96C0
void CAERadioTrackManager::AddMusicTrackIndexToHistory(eRadioID id, int8 trackIndex) {
    if (trackIndex >= 0 && m_nMusicTrackIndexHistory[id].indices[0] != trackIndex) {
        m_nMusicTrackIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id]++;
    }
}

// 0x4E9720
void CAERadioTrackManager::AddIdentIndexToHistory(eRadioID id, int8 trackIndex) {
    if (m_nIdentIndexHistory[id].indices[0] != trackIndex)
        m_nIdentIndexHistory[id].PutAtFirst(trackIndex);
}

// 0x4E9760
void CAERadioTrackManager::AddAdvertIndexToHistory(eRadioID id, int8 trackIndex) {
    if (m_nAdvertIndexHistory[id].indices[0] != trackIndex) {
        m_nAdvertIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id] = 0;
    }
}

// 0x4E97B0
void CAERadioTrackManager::AddDJBanterIndexToHistory(eRadioID id, int8 trackIndex) {
    if (m_nDJBanterIndexHistory[id].indices[0] != trackIndex) {
        m_nDJBanterIndexHistory[id].PutAtFirst(trackIndex);
        m_nTracksInARow[id] = 0;
    }
}

// 0x4EA590
void CAERadioTrackManager::CheckForPause() {
    if (CTimer::GetIsPaused()) {
        m_bPauseMode = true;
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, m_bEnabledInPauseMode ? 1.0f : 0.0f);

        return;
    }

    // todo: See CAEVehicleAudioEntity::Terminate:437 m_nRadioType.
    tVehicleAudioSettings* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();

    const bool isRadioTypeOrdinary = settings && [radioType = settings->m_nRadioType]{
        switch (radioType) {
        case RADIO_CIVILIAN:
        case RADIO_EMERGENCY:
        case RADIO_UNKNOWN:
            return true;
        default:
            return false;
        }
    }();

    if (isRadioTypeOrdinary || CAudioEngine::IsAmbienceRadioActive()) {
        m_bPauseMode = false;
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, 1.0f);
    } else {
        StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        m_bPauseMode = false;
    }
}

// 0x4EB9A0
void CAERadioTrackManager::Service(int32 playTime) {
    plugin::CallMethod<0x4EB9A0, CAERadioTrackManager*, int32>(this, playTime);
}

// 0x5D40E0
void CAERadioTrackManager::Load() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : m_nMusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(historyIndex);
        }

        for (auto& identIndex : m_nIdentIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(identIndex);
        }

        for (auto& advertIndex : m_nAdvertIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : m_nDJBanterIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsCitiesPassed);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino6);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCasino10);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedCat1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert5);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert8);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedDesert10);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedFarlie3);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedLAFin2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedMansion2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedRyder2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedRiot1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedSCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedStrap4);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedSweet2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedTruth2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsPassedVCrash2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedBadlands);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedCat2);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsStartedCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitGameClockDays);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitGameClockHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nStatsLastHitTimeOutHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nSpecialDJBanterPending);
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nSpecialDJBanterIndex);
}

// 0x5D3EE0
void CAERadioTrackManager::Save() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : m_nMusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(historyIndex);
        }

        for (auto& identIndex : m_nIdentIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(identIndex);
        }

        for (auto& advertIndex : m_nAdvertIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : m_nDJBanterIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsCitiesPassed);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino6);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCasino10);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedCat1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert5);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert8);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedDesert10);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedFarlie3);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedLAFin2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedMansion2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedRyder2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedRiot1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedSCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedStrap4);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedSweet2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedTruth2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsPassedVCrash2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedBadlands);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedCat2);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsStartedCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitGameClockDays);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitGameClockHours);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nStatsLastHitTimeOutHours);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nSpecialDJBanterPending);
    CGenericGameStorage::SaveDataToWorkBuffer(m_nSpecialDJBanterIndex);
}
