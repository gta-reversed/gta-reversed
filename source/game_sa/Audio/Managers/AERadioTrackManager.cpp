#include "StdInc.h"

#include "AERadioTrackManager.h"
#include "RadioStreamsPC.h"

#include "AEAudioHardware.h"
#include "AEUserRadioTrackManager.h"
#include "AEAudioUtility.h"
#include "AEAudioEnvironment.h"

auto& AERadioTrackManager = StaticRef<CAERadioTrackManager>(0x8CB6F8);

// The tables below are the original game's per-station sound index ranges/counts.
// They aren't named anywhere else in the project yet, hence why they're declared here.
// A `first` index of `NO_SOUND_INDEX` means that the station has no sounds of that category.
static constexpr int32 NO_SOUND_INDEX = 0x782; // 1922

static auto& ms_aIdentIndexRanges           = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8FB0); // Stations (EAA + the 11 radios)
static auto& ms_aDJBanterIndexRanges        = StaticRef<std::pair<int32, int32>[RADIO_COUNT]>(0x8C8CB0);
static auto& ms_aSpecialDJBanterIndexRanges = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8C50);
static auto& ms_aForcedDJBanterIndexRanges  = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8BF0);
static auto& ms_anSpecialDJBanterIndices    = StaticRef<int32[RADIO_COUNT][22]>(0x8CB280);
static auto& ms_anAdvertIndexRange          = StaticRef<std::pair<int32, int32>>(0x8C8B88);
static auto& ms_aAdvertIndicesInQueue       = StaticRef<std::array<int32, 23>[RADIO_COUNT]>(0x8CADD0);
static auto& ms_anMusicTrackCounts          = StaticRef<int32[RADIO_COUNT]>(0x8C9010);
static auto& ms_anStationTrackLengths       = StaticRef<int32[RADIO_COUNT]>(0x8CAD50);
static auto& ms_nPreviousTrackPlayTime      = StaticRef<int32>(0x8CBA68);
static auto& ms_fRetuneDelaySelector        = StaticRef<float>(0xB6F14C); // Selects the retune delay in `CheckForStationRetune`

// Per station (`0`-`11`) and track index: The ID of the track, and the (min, max) track IDs
// used for intros/outros.
static auto& ms_aStationTrackIds       = StaticRef<int32[RADIO_COUNT - 2][31]>(0x8C9040);
static auto& ms_aStationIntroTrackIds  = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2][31]>(0x8C9610);
static auto& ms_aStationOutroTrackIds  = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2][31]>(0x8CA1B0);
static auto& ms_anTalkRadioShowLengths = StaticRef<int32[31]>(0x8CAD50); // Indexed by the talk show index (See `StopRadio`)

// DJ banter index ranges selected by the current weather forecast/time of day
// NOTE: The names of these are descriptive, the original project doesn't name them either.
static auto& ms_aDJBanterIndexRangesTimeA = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8D70);
static auto& ms_aDJBanterIndexRangesTimeB = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8DD0);
static auto& ms_aDJBanterIndexRangesRiots = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8E30);
static auto& ms_aDJBanterIndexRangesRainy = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8E90);
static auto& ms_aDJBanterIndexRangesClear = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8EF0);
static auto& ms_aDJBanterIndexRangesFoggy = StaticRef<std::pair<int32, int32>[RADIO_COUNT - 2]>(0x8C8F50);

void CAERadioTrackManager::InjectHooks() {
    RH_ScopedClass(CAERadioTrackManager);
    RH_ScopedCategory("Audio/Managers");

    RH_ScopedInstall(Load, 0x5D40E0);
    RH_ScopedInstall(Save, 0x5D3EE0);
    RH_ScopedInstall(Initialise, 0x5B9390);
    RH_ScopedInstall(Service, 0x4EB9A0);
    RH_ScopedInstall(DisplayRadioStationName, 0x4E9E50);
    RH_ScopedInstall(CheckForStationRetune, 0x4EB660);
    RH_ScopedInstall(CheckForPause, 0x4EA590);
    RH_ScopedInstall(IsVehicleRadioActive, 0x4E9800);
    RH_ScopedInstall(AddDJBanterIndexToHistory, 0x4E97B0);
    RH_ScopedInstall(AddAdvertIndexToHistory, 0x4E9760);
    RH_ScopedInstall(AddIdentIndexToHistory, 0x4E9720);
    RH_ScopedInstall(AddMusicTrackIndexToHistory, 0x4E96C0);
    RH_ScopedOverloadedInstall(StartRadio, "manual", 0x4EB3C0, void (CAERadioTrackManager::*)(eRadioID, eBassSetting, float, bool));
    RH_ScopedOverloadedInstall(StartRadio, "with-settings", 0x4EB550, void (CAERadioTrackManager::*)(const tVehicleAudioSettings&));
    RH_ScopedInstall(CheckForStationRetuneDuringPause, 0x4EB890);
    RH_ScopedInstall(TrackRadioStation, 0x4EAC30);
    RH_ScopedInstall(ChooseTracksForStation, 0x4EB180);
    RH_ScopedInstall(CheckForTrackConcatenation, 0x4EA930);
    RH_ScopedInstall(QueueUpTracksForStation, 0x4EA670);
    RH_ScopedInstall(ChooseDJBanterIndex, 0x4EA2D0);
    RH_ScopedInstall(ChooseDJBanterIndexFromList, 0x4E95E0);
    RH_ScopedInstall(ChooseAdvertIndex, 0x4E9570);
    RH_ScopedInstall(ChooseIdentIndex, 0x4E94C0);
    RH_ScopedInstall(ChooseMusicTrackIndex, 0x4EA270);
    RH_ScopedInstall(ChooseTalkRadioShow, 0x4E8E40);
    RH_ScopedInstall(CheckForMissionStatsChanges, 0x4E8410);
    RH_ScopedInstall(StartTrackPlayback, 0x4EA640);
    RH_ScopedInstall(UpdateRadioVolumes, 0x4EA010);
    RH_ScopedInstall(PlayRadioAnnouncement, 0x4E8400);
    RH_ScopedInstall(GetCurrentRadioStationID, 0x4E83F0);
    RH_ScopedInstall(GetRadioStationListenTimes, 0x4E83E0);
    RH_ScopedInstall(GetRadioStationName, 0x4E9E10);
    RH_ScopedInstall(GetRadioStationNameKey, 0x4E8380);
    RH_ScopedInstall(HasRadioRetuneJustStarted, 0x4E8370);
    RH_ScopedInstall(StopRadio, 0x4E9820);
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

    rng::copy(CStats::GetFullFavoriteRadioStationList(), m_aListenTimes.begin());

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
    m_RequestedSettings.StationID = m_ActiveSettings.StationID = id;
}

// 0x4E7F80
void CAERadioTrackManager::Reset() {
    m_bInitialised = false;
    m_bDisplayStationName = false;
    rng::copy(CStats::GetFullFavoriteRadioStationList(), m_aListenTimes.begin());

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
    return m_aListenTimes.data();
}

// 0x4E83F0
eRadioID CAERadioTrackManager::GetCurrentRadioStationID() const {
    return m_RequestedSettings.StationID == RADIO_INVALID ? RADIO_OFF : m_RequestedSettings.StationID;
}

// 0x4E82E0
void CAERadioTrackManager::SetRadioAutoRetuneOnOff(bool enable) {
    m_bRadioAutoSelect = enable;
}

// 0x4E82F0
void CAERadioTrackManager::SetBassSetting(eBassSetting bassSetting, float bassGrain) {
    m_RequestedSettings.BassGain = m_ActiveSettings.BassGain = bassGrain;
    m_RequestedSettings.BassSetting = m_ActiveSettings.BassSetting = bassSetting;
    AEAudioHardware.SetBassSetting(m_bBassEnhance ? bassSetting : eBassSetting::NORMAL, bassGrain);
}

// 0x4E9DB0
void CAERadioTrackManager::SetBassEnhanceOnOff(bool enable) {
    m_bBassEnhance = enable;
    if (m_nMode == eRadioTrackMode::RADIO_PLAYING) {
        m_RequestedSettings.BassSetting = m_ActiveSettings.BassSetting;
        m_RequestedSettings.BassGain = m_ActiveSettings.BassGain;
        if (enable) {
            AEAudioHardware.SetBassSetting(m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain);
        } else {
            AEAudioHardware.SetBassSetting(eBassSetting::NORMAL, m_ActiveSettings.BassGain);
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
        int station = m_nStationsListed + m_RequestedSettings.StationID;
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
        switch (opts->RadioType) {
        case AE_RT_CIVILIAN:
        case AE_RT_EMERGENCY:
        case AE_RT_UNKNOWN:
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
    int8 trackCount = 1; // Also used as the index of the next free slot in the track queue

    if (m_ActiveSettings.StationID == RADIO_USER_TRACKS) {
        const auto utPlayMode = m_nUserTrackPlayMode;
        if (utPlayMode != AEUserRadioTrackManager.GetUserTrackPlayMode()) {
            if ((utPlayMode == 2 || AEUserRadioTrackManager.GetUserTrackPlayMode() == 2)
                && m_ActiveSettings.PlayTime != -4
            ) {
                AEUserRadioTrackManager.SetUserTrackIndex(m_ActiveSettings.TrackQueue[0]);

                m_ActiveSettings.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                m_ActiveSettings.TrackTypes[1]   = TYPE_USER_TRACK;
                m_ActiveSettings.TrackIndices[1] = (int8)m_ActiveSettings.TrackQueue[1];

                trackCount = 2;

                AEAudioHardware.PlayTrack(
                    m_ActiveSettings.TrackQueue[0],
                    m_ActiveSettings.TrackQueue[1],
                    0u,
                    m_ActiveSettings.TrackFlags,
                    m_ActiveSettings.TrackTypes[0] == TYPE_USER_TRACK,
                    m_ActiveSettings.TrackTypes[1] == TYPE_USER_TRACK
                );
            }
            m_nUserTrackPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode();
        }
    }

    const auto nextTrack = m_ActiveSettings.TrackQueue[1];
    if (AEAudioHardware.GetActiveTrackID() != nextTrack || nextTrack < 0) {
        return;
    }

    m_ActiveSettings.SwitchToNextTrack();

    if (m_ActiveSettings.TrackQueue[1] == -1) {
        if (m_ActiveSettings.StationID == RADIO_USER_TRACKS) {
            if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
                m_ActiveSettings.TrackQueue[trackCount] = ChooseAdvertIndex(RADIO_USER_TRACKS);
                m_ActiveSettings.TrackTypes[trackCount] = TYPE_ADVERT;
                trackCount++;
            }
            m_ActiveSettings.TrackQueue[trackCount]   = AEUserRadioTrackManager.SelectUserTrackIndex();
            m_ActiveSettings.TrackTypes[trackCount]   = TYPE_USER_TRACK;
            m_ActiveSettings.TrackIndices[trackCount] = (int8)m_ActiveSettings.TrackQueue[trackCount];
            trackCount++;
            m_ActiveSettings.TrackQueue[trackCount]   = AEUserRadioTrackManager.SelectUserTrackIndex();
            m_ActiveSettings.TrackTypes[trackCount]   = TYPE_USER_TRACK;
            m_ActiveSettings.TrackIndices[trackCount] = (int8)m_ActiveSettings.TrackQueue[trackCount];
        } else {
            const auto station   = (int32)m_ActiveSettings.StationID;
            const auto firstType = m_ActiveSettings.TrackTypes[0];
            if (firstType == TYPE_INTRO || firstType == TYPE_TRACK || firstType == TYPE_OUTRO) {
                if (station == RADIO_EMERGENCY_AA) {
                    QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, TYPE_DJ_BANTER, m_ActiveSettings);
                } else if ((int8)m_nTracksInARow[station] < 2 && CAEAudioUtility::ResolveProbability(0.5f)) {
                    if (CAEAudioUtility::ResolveProbability(0.5f)) {
                        QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_INDENT, m_ActiveSettings);
                    }
                    QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_INTRO, m_ActiveSettings);
                } else {
                    if (CAEAudioUtility::ResolveProbability(0.5f)) {
                        QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_INDENT, m_ActiveSettings);
                    }
                    if (!QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_DJ_BANTER, m_ActiveSettings)) {
                        QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_ADVERT, m_ActiveSettings);
                    }
                }
            } else {
                QueueUpTracksForStation((eRadioID)station, &trackCount, TYPE_INTRO, m_ActiveSettings);
            }
        }
    }

    AEAudioHardware.PlayTrack(
        m_ActiveSettings.TrackQueue[0],
        m_ActiveSettings.TrackQueue[1],
        0u,
        m_ActiveSettings.TrackFlags,
        m_ActiveSettings.TrackTypes[0] == TYPE_USER_TRACK,
        m_ActiveSettings.TrackTypes[1] == TYPE_USER_TRACK
    );
}

// 0x4EB660
void CAERadioTrackManager::CheckForStationRetune() {
    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA) {
        return;
    }

    m_bRetuneJustStarted = false;

    if ((m_nMode == eRadioTrackMode::RADIO_STARTING
         || m_nMode == eRadioTrackMode::RADIO_WAITING_TO_PLAY
         || m_nMode == eRadioTrackMode::RADIO_PLAYING
         || m_bInitialised
         || m_nStationsListed
         || m_nStationsListDown
         || m_ActiveSettings.StationID == RADIO_OFF
        )
        && AudioEngine.GetCutsceneTrackStatus() == 0
    ) {
        const auto settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        if (settings
            && notsa::contains({AE_RT_CIVILIAN, AE_RT_EMERGENCY, AE_RT_UNKNOWN}, settings->RadioType)
            && CReplay::Mode != MODE_PLAYBACK
        ) {
            if (settings->RadioType != AE_RT_CIVILIAN) {
                return;
            }

            // Retune by script or by player input
            bool stationChanged = true;
            if (m_iRadioStationScriptRequest < 0) {
                if (CPad::GetPad(0)->NextStationJustUp()) {
                    m_nStationsListDown        = m_nStationsListed;
                    m_nStationsListed++;
                    m_nTimeRadioStationRetuned = CTimer::GetTimeInMS();
                } else if (CPad::GetPad(0)->LastStationJustUp()) {
                    m_nStationsListDown        = m_nStationsListed;
                    m_nStationsListed--;
                    m_nTimeRadioStationRetuned = CTimer::GetTimeInMS();
                } else {
                    stationChanged = false;
                }
            } else {
                m_nStationsListDown          = m_nStationsListed;
                m_nStationsListed            = m_iRadioStationScriptRequest - (int32)m_ActiveSettings.StationID;
                m_iRadioStationScriptRequest = -1;
                m_nTimeRadioStationRetuned   = CTimer::GetTimeInMS();
            }
            if (stationChanged) {
                m_bDisplayStationName = true;
                m_bRetuneJustStarted  = true;
            }

            if (m_nStationsListed == 0 && m_nStationsListDown == 0) {
                return;
            }

            // Wrap the listed station around the radio stations
            auto station = (int8)((uint8)m_ActiveSettings.StationID + (uint8)m_nStationsListed);
            if (station > 0) {
                if (station > 13) {
                    station -= 13;
                }
            } else {
                station += 13;
            }

            if (station == RADIO_OFF
                || (station == RADIO_USER_TRACKS && !AEUserRadioTrackManager.m_nUserTracksCount)
            ) {
                StopRadio(nullptr, false);
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF, 0.0f, 1.0f);
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.0f, 1.0f);
            } else {
                if (m_ActiveSettings.StationID == RADIO_OFF) {
                    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON, 0.0f, 1.0f);
                    m_ActiveSettings.StationID = RADIO_INVALID;
                } else {
                    StopRadio(nullptr, false);
                }
                AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START, 0.0f, 1.0f);

                // The new station only starts playing after a delay
                const auto retuneDelay = ms_fRetuneDelaySelector <= 0.9f ? 2000 : 4000;
                if (CTimer::GetTimeInMS() <= m_nTimeRadioStationRetuned + 1500u
                    || CTimer::GetTimeInMS() <= m_nRetuneStartedTime + retuneDelay
                ) {
                    return;
                }
            }

            StartRadio((eRadioID)station, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, false);
            m_nStationsListed   = 0;
            m_nStationsListDown = 0;
        }
    }
}

// 0x4EB890
void CAERadioTrackManager::CheckForStationRetuneDuringPause() {
    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA && IsRadioOn() || m_iRadioStationMenuRequest <= RADIO_INVALID)
        return;

    if (m_iRadioStationMenuRequest != RADIO_OFF) {
        if (m_ActiveSettings.StationID == RADIO_OFF) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON);
            m_ActiveSettings.StationID = RADIO_INVALID;
        } else {
            AudioEngine.StopRadio(nullptr, true);
        }

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        if (CTimer::GetTimeInMSPauseMode() > m_nRetuneStartedTime + 700u) {
            StartRadio((eRadioID)m_iRadioStationMenuRequest, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
            m_iRadioStationMenuRequest = RADIO_INVALID;
        }
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        StartRadio(RADIO_OFF, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
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
    auto volume = -4.0f;
    if (!CTimer::GetIsPaused() || !m_bEnabledInPauseMode) {
        if (CTimer::GetIsSlowMotionActive()) {
            volume = -100.0f;
        } else if (TheCamera.m_bWideScreenOn) {
            volume = -16.0f;
        } else if (AEAudioHardware.GetEffectsMasterScalingFactor() > 0.0f
            && AEAudioHardware.GetMusicMasterScalingFactor() > 0.0f
        ) {
            // The radio is boosted if mission audio is playing nearby (or is forced to be audible)
            const auto isMissionAudioNearby = [] {
                if (CAEPedSpeechAudioEntity::s_bForceAudible) {
                    return true;
                }
                for (auto i = 0; i < 2; i++) {
                    if (AudioEngine.IsMissionAudioSampleFinished(i) || AudioEngine.GetMissionAudioEvent(i) == 0xFFFF) {
                        continue;
                    }
                    const auto pos = AudioEngine.GetMissionAudioPosition(i);
                    if (!pos) {
                        return true;
                    }
                    if (CAEAudioEnvironment::GetPositionRelativeToCamera(*pos).Magnitude() <= 15.0f) {
                        return true;
                    }
                }
                return false;
            }();

            if (isMissionAudioNearby) {
                const auto ratio = AEAudioHardware.GetEffectsMasterScalingFactor() / AEAudioHardware.GetMusicMasterScalingFactor();
                m_f80 = std::max(20.0f * std::log10(ratio) - 9.0f, 0.0f);
                m_f84 = -0.02f * m_f80;
                volume = m_f80 - 4.0f;
            } else if (m_f80 < 0.0f) { // Decay the boost back to 0
                m_f80 = std::min(m_f80 + m_f84, 0.0f);
                volume = m_f80 - 4.0f;
            }
        }

        if (AudioEngine.IsAmbienceRadioActive()) {
            volume -= 20.0f;
        }
    }

    if (m_bBassEnhance && m_ActiveSettings.BassSetting != eBassSetting::NORMAL) {
        switch (m_ActiveSettings.BassSetting) {
        case eBassSetting::BOOST:
            volume -= 2.0f;
            break;
        case eBassSetting::CUT:
            volume += 1.5f;
            break;
        }
    }

    AEAudioHardware.SetChannelVolume(m_HwClientHandle, 0, volume, 0);
}


// 0x4E8400
void CAERadioTrackManager::PlayRadioAnnouncement(uint32) {
    // NOP
}

// 0x4EB550
void CAERadioTrackManager::StartRadio(const tVehicleAudioSettings& settings) {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (settings.RadioType == AE_RT_EMERGENCY) {
        StartRadio(RADIO_EMERGENCY_AA, settings.BassSetting, settings.BassFactor, 0);
        return;
    }

    if (settings.RadioType != AE_RT_CIVILIAN)
        return;

    const bool needsRetune = [&] {
       if (!m_bRadioAutoSelect)
           return false;

       const auto savedId = m_nSavedRadioStationId;
       if (savedId < 0 || savedId == settings.RadioStation || savedId == RADIO_OFF || savedId == RADIO_EMERGENCY_AA)
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
        StartRadio((eRadioID)m_nSavedRadioStationId, settings.BassSetting, settings.BassFactor, 0);
    } else {
        StartRadio(settings.RadioStation, settings.BassSetting, settings.BassFactor, 0);
    }
}

// 0x4EB3C0
void CAERadioTrackManager::StartRadio(eRadioID id, eBassSetting bassSetting, float bassGain, bool skipTrack) {
    id = std::min(id, RADIO_OFF);

    if (CTimer::GetIsPaused()) {
        m_bEnabledInPauseMode = true;

        if (IsRadioOn() && id == m_ActiveSettings.StationID) {
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

    auto* const rs = &m_RequestedSettings;
    rs->StationID = id;
    rs->BassSetting             = bassSetting;
    rs->BassGain            = bassGain;
    if (id == RADIO_OFF) {
        rs->Reset();
    } else if (m_aRadioState[id].m_iTimeInMs < 0 || !TrackRadioStation(id, skipTrack)) {
        ChooseTracksForStation(rs->StationID);
        rs->PlayTime = CAEAudioUtility::GetRandomNumberInRange(0, 300'000);
    }
    switch (m_nMode) {
    case eRadioTrackMode::RADIO_STARTING:
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
    case eRadioTrackMode::RADIO_PLAYING:
        m_nMode = eRadioTrackMode::RADIO_STOPPING;
    }
    m_aRadioState[rs->StationID].m_iTimeInPauseModeInMs = -1;

    m_bInitialised = true;
}

// 0x4EAC30
bool CAERadioTrackManager::TrackRadioStation(eRadioID id, bool skipTrack) {
    auto& state = m_aRadioState[(int32)id];

    // Tuning back to a station that was left more than 5 in-game hours ago isn't allowed
    if (state.m_nGameClockHours >= 0 && state.m_nGameClockDays >= 0) {
        auto days = (int32)CClock::ms_nGameClockDays - state.m_nGameClockDays;
        if (days < 0) {
            auto month = (int8)CClock::ms_nGameClockMonth - 1;
            if (month < 0) {
                month += 12;
            }
            days += CClock::daysInMonth[month];
        }
        if (days * 24 - state.m_nGameClockHours + CClock::ms_nGameClockHours > 5) {
            return false;
        }
    }

    // How long has the current track been playing for?
    auto elapsed = (int32)(CTimer::GetTimeInMS() - (uint32)state.m_iTimeInMs);
    if (elapsed <= 7000) {
        elapsed = 7000;
    }
    if (skipTrack && elapsed <= state.m_aElapsed[0] + 1) {
        elapsed = state.m_aElapsed[0] + 1;
    }

    // Reset the requested settings - They're filled back up below
    for (auto i = 0u; i < tRadioSettings::NUM_TRACKS; i++) {
        m_RequestedSettings.TrackQueue[i]   = -1;
        m_RequestedSettings.TrackTypes[i]   = TYPE_NONE;
        m_RequestedSettings.TrackIndices[i] = -1;
    }

    // Find the track that is currently playing
    int32 trackIndex      = 0;
    int32 accumulatedTime = 0;
    for (; trackIndex < 3; trackIndex++) {
        accumulatedTime += state.m_aElapsed[trackIndex];
        if (elapsed <= accumulatedTime) {
            if (trackIndex == 0) {
                m_RequestedSettings.PlayTime = state.m_iTrackPlayTime + elapsed;
            } else {
                m_RequestedSettings.PlayTime = state.m_aElapsed[trackIndex] - accumulatedTime + elapsed;
            }
            break;
        }
    }

    int8 trackCount = 1; // The next slot to fill in the queue
    if (trackIndex < 3) {
        const auto trackType = state.m_aTrackTypes[trackIndex];
        switch (trackType) {
        case TYPE_INDENT:
        case TYPE_ADVERT:
        case TYPE_DJ_BANTER:
            m_RequestedSettings.TrackQueue[0] = state.m_aTrackQueue[trackIndex];
            m_RequestedSettings.TrackTypes[0] = trackType;
            if (id == RADIO_USER_TRACKS) {
                QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
            } else {
                QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
            }
            return true;
        case TYPE_INTRO:
            for (auto i = 0; i < 3; i++) {
                m_RequestedSettings.TrackQueue[i] = state.m_aTrackQueue[trackIndex + i];
                m_RequestedSettings.TrackTypes[i] = state.m_aTrackTypes[trackIndex + i];
            }
            return true;
        case TYPE_TRACK:
            for (auto i = 0; i < 2; i++) {
                m_RequestedSettings.TrackQueue[i] = state.m_aTrackQueue[trackIndex + i];
                m_RequestedSettings.TrackTypes[i] = state.m_aTrackTypes[trackIndex + i];
            }
            return true;
        case TYPE_USER_TRACK:
            for (auto i = 0; i < 2; i++) {
                m_RequestedSettings.TrackQueue[i] = state.m_aTrackQueue[trackIndex + i];
                m_RequestedSettings.TrackTypes[i] = state.m_aTrackTypes[trackIndex + i];
            }
            if (m_RequestedSettings.TrackQueue[1] == -1) {
                m_RequestedSettings.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                m_RequestedSettings.TrackTypes[1]   = TYPE_USER_TRACK;
                m_RequestedSettings.TrackIndices[1] = (int8)m_RequestedSettings.TrackQueue[1];
            }
            return true;
        case TYPE_OUTRO:
            m_RequestedSettings.TrackQueue[0] = state.m_aTrackQueue[trackIndex];
            m_RequestedSettings.TrackTypes[0] = trackType;
            if (id == RADIO_EMERGENCY_AA) {
                QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
                return true;
            }
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
            }
            if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings)) {
                QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);
            }
            return true;
        default: // TYPE_NONE
            return false;
        }
    }

    // Nothing is playing anymore - Queue up new tracks
    if (elapsed <= accumulatedTime + 7000) {
        if (id == RADIO_USER_TRACKS) {
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        } else {
            QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
        }
        if (elapsed - accumulatedTime < 5000) {
            m_RequestedSettings.PlayTime = elapsed - accumulatedTime;
        } else {
            m_RequestedSettings.PlayTime = 5000;
        }
        return true;
    }

    if (elapsed <= accumulatedTime + 154'488) {
        QueueUpTracksForStation(id, &trackCount, TYPE_TRACK, m_RequestedSettings);
        m_RequestedSettings.PlayTime = elapsed - accumulatedTime - 5000;
        return true;
    }

    if (elapsed > accumulatedTime + 160'000) {
        return false;
    }

    if (id == RADIO_USER_TRACKS) {
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_TRACK, m_RequestedSettings);
        if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, TYPE_ADVERT, m_RequestedSettings);
        }
    } else {
        QueueUpTracksForStation(id, &trackCount, TYPE_OUTRO, m_RequestedSettings);
        AddMusicTrackIndexToHistory(id, m_RequestedSettings.TrackIndices[trackCount - 1]);

        if (id == RADIO_EMERGENCY_AA) {
            QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings);
        } else {
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                if (CAEAudioUtility::ResolveProbability(0.5f)) {
                    QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
                }
                QueueUpTracksForStation(id, &trackCount, TYPE_INTRO, m_RequestedSettings);
            } else {
                if (CAEAudioUtility::ResolveProbability(0.5f)) {
                    QueueUpTracksForStation(id, &trackCount, TYPE_INDENT, m_RequestedSettings);
                }
                if (!QueueUpTracksForStation(id, &trackCount, TYPE_DJ_BANTER, m_RequestedSettings)) {
                    QueueUpTracksForStation(id, &trackCount, TYPE_ADVERT, m_RequestedSettings);
                }
            }
        }
    }

    m_RequestedSettings.PlayTime = elapsed - accumulatedTime - 154'488;
    return true;
}

// 0x4EA670 (Relocated body @ 0x4EA675)
bool CAERadioTrackManager::QueueUpTracksForStation(eRadioID id, int8* iTrackCount, int8 radioState, tRadioSettings& settings) {
    const auto station = (int32)id;

    switch (radioState) {
    case TYPE_INDENT:
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackQueue[*iTrackCount] = ChooseIdentIndex(id);
        if (settings.TrackQueue[*iTrackCount] == -1) {
            return false;
        }
        settings.TrackTypes[*iTrackCount] = TYPE_INDENT;
        (*iTrackCount)++;
        return true;
    case TYPE_ADVERT:
        settings.TrackQueue[*iTrackCount] = ChooseAdvertIndex(id);
        settings.TrackTypes[*iTrackCount] = TYPE_ADVERT;
        (*iTrackCount)++;
        return true;
    case TYPE_DJ_BANTER:
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackQueue[*iTrackCount] = ChooseDJBanterIndex(id);
        if (settings.TrackQueue[*iTrackCount] == -1) {
            return false;
        }
        settings.TrackTypes[*iTrackCount] = TYPE_DJ_BANTER;
        (*iTrackCount)++;
        return true;
    case TYPE_INTRO: {
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackIndices[*iTrackCount] = ChooseMusicTrackIndex(id);
        settings.TrackQueue[*iTrackCount]   = CAEAudioUtility::GetRandomNumberInRange(
            ms_aStationIntroTrackIds[station][settings.TrackIndices[*iTrackCount]].first,
            ms_aStationIntroTrackIds[station][settings.TrackIndices[*iTrackCount]].second
        );
        settings.TrackTypes[*iTrackCount] = TYPE_INTRO;
        (*iTrackCount)++;

        settings.TrackIndices[*iTrackCount] = settings.TrackIndices[*iTrackCount - 1];
        settings.TrackQueue[*iTrackCount]   = ms_aStationTrackIds[station][settings.TrackIndices[*iTrackCount]];
        settings.TrackTypes[*iTrackCount]   = TYPE_TRACK;
        (*iTrackCount)++;

        settings.TrackIndices[*iTrackCount] = settings.TrackIndices[*iTrackCount - 1];
        settings.TrackQueue[*iTrackCount]   = CAEAudioUtility::GetRandomNumberInRange(
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].first,
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].second
        );
        settings.TrackTypes[*iTrackCount] = TYPE_OUTRO;
        (*iTrackCount)++;
        return true;
    }
    case TYPE_TRACK: {
        if (id == RADIO_USER_TRACKS) {
            settings.TrackQueue[*iTrackCount]   = AEUserRadioTrackManager.SelectUserTrackIndex();
            settings.TrackTypes[*iTrackCount]   = TYPE_USER_TRACK;
            settings.TrackIndices[*iTrackCount] = (int8)settings.TrackQueue[*iTrackCount];
            (*iTrackCount)++;
            return true;
        }
        settings.TrackIndices[*iTrackCount] = ChooseMusicTrackIndex(id);
        settings.TrackQueue[*iTrackCount]   = ms_aStationTrackIds[station][settings.TrackIndices[*iTrackCount]];
        settings.TrackTypes[*iTrackCount]   = TYPE_TRACK;
        (*iTrackCount)++;

        settings.TrackIndices[*iTrackCount] = settings.TrackIndices[*iTrackCount - 1];
        settings.TrackQueue[*iTrackCount]   = CAEAudioUtility::GetRandomNumberInRange(
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].first,
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].second
        );
        settings.TrackTypes[*iTrackCount] = TYPE_OUTRO;
        (*iTrackCount)++;
        return true;
    }
    case TYPE_OUTRO:
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackIndices[*iTrackCount] = ChooseMusicTrackIndex(id);
        settings.TrackQueue[*iTrackCount]   = CAEAudioUtility::GetRandomNumberInRange(
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].first,
            ms_aStationOutroTrackIds[station][settings.TrackIndices[*iTrackCount]].second
        );
        settings.TrackTypes[*iTrackCount] = TYPE_OUTRO;
        (*iTrackCount)++;
        return true;
    }
    return false;
}

// 0x4E9820
void CAERadioTrackManager::StopRadio(tVehicleAudioSettings* settings, bool duringPause) {
    if (m_nMode == eRadioTrackMode::RADIO_STARTING
        || m_nMode == eRadioTrackMode::RADIO_WAITING_TO_PLAY
        || m_nMode == eRadioTrackMode::RADIO_PLAYING
    ) {
        if (!CTimer::GetIsPaused() || duringPause) {
            m_nMode = eRadioTrackMode::RADIO_STOPPING;
        }

        const auto station = m_ActiveSettings.StationID;
        auto&      state   = m_aRadioState[station];

        state.m_aElapsed[0]     = 0;
        state.m_iTrackPlayTime  = -1;
        state.m_aTrackQueue[0]  = -1;
        state.m_aTrackTypes[0]  = TYPE_NONE;
        state.m_aElapsed[1]     = 0;
        state.m_iTrackPlayTime  = -1;
        state.m_aTrackQueue[1]  = -1;
        state.m_aTrackTypes[1]  = TYPE_NONE;
        state.m_aElapsed[2]     = 0;
        state.m_iTrackPlayTime  = -1;
        state.m_aTrackQueue[2]  = -1;
        state.m_aTrackTypes[2]  = TYPE_NONE;
        state.m_iTimeInMs       = CTimer::GetTimeInMS();
        state.m_nGameClockDays  = CClock::ms_nGameClockDays;
        state.m_nGameClockHours = CClock::ms_nGameClockHours;

        // Count how long this station was listened to
        if (station != RADIO_EMERGENCY_AA && station != RADIO_OFF && state.m_iTimeInPauseModeInMs >= 0) {
            m_aListenTimes[station] += CTimer::GetTimeInMSPauseMode() - state.m_iTimeInPauseModeInMs;
        }

        if (station == RADIO_OFF) {
            m_aRadioState[RADIO_OFF].m_iTimeInMs = 0;
        } else {
            state.m_aElapsed[0] = m_ActiveSettings.TrackLengthMs - m_ActiveSettings.PlayTime - 100;

            switch (m_ActiveSettings.CurrTrackType) {
            case TYPE_INDENT:
            case TYPE_ADVERT:
            case TYPE_DJ_BANTER:
            case TYPE_OUTRO:
                state.m_iTrackPlayTime = m_ActiveSettings.PlayTime;
                state.m_aTrackQueue[0] = m_ActiveSettings.CurrTrackID;
                state.m_aTrackTypes[0] = m_ActiveSettings.CurrTrackType;
                break;
            case TYPE_INTRO:
                if (station == RADIO_TALK) {
                    state.m_aElapsed[1] = ms_anTalkRadioShowLengths[m_ActiveSettings.TrackIndices[0]];
                } else {
                    state.m_aElapsed[1] = 150'000;
                }
                state.m_aElapsed[2]    = 5'000;
                state.m_iTrackPlayTime = m_ActiveSettings.PlayTime;
                if (m_ActiveSettings.TrackQueue[0] == m_ActiveSettings.CurrTrackID) {
                    state.m_aTrackQueue[0] = m_ActiveSettings.TrackQueue[0];
                    state.m_aTrackTypes[0] = m_ActiveSettings.TrackTypes[0];
                    state.m_aTrackQueue[1] = m_ActiveSettings.TrackQueue[1];
                    state.m_aTrackTypes[1] = m_ActiveSettings.TrackTypes[1];
                    state.m_aTrackQueue[2] = m_ActiveSettings.TrackQueue[2];
                    state.m_aTrackTypes[2] = m_ActiveSettings.TrackTypes[2];
                } else {
                    state.m_aTrackQueue[0] = m_ActiveSettings.CurrTrackID;
                    state.m_aTrackTypes[0] = m_ActiveSettings.CurrTrackType;
                    state.m_aTrackQueue[1] = m_ActiveSettings.TrackQueue[0];
                    state.m_aTrackTypes[1] = m_ActiveSettings.TrackTypes[0];
                    state.m_aTrackQueue[2] = m_ActiveSettings.TrackQueue[1];
                    state.m_aTrackTypes[2] = m_ActiveSettings.TrackTypes[1];
                }
                break;
            case TYPE_TRACK:
            case TYPE_USER_TRACK:
                state.m_aElapsed[1]    = 5'000;
                state.m_iTrackPlayTime = m_ActiveSettings.PlayTime;
                if (m_ActiveSettings.TrackQueue[0] == m_ActiveSettings.CurrTrackID) {
                    state.m_aTrackQueue[0] = m_ActiveSettings.TrackQueue[0];
                    state.m_aTrackTypes[0] = m_ActiveSettings.TrackTypes[0];
                    state.m_aTrackQueue[1] = m_ActiveSettings.TrackQueue[1];
                    state.m_aTrackTypes[1] = m_ActiveSettings.TrackTypes[1];
                } else {
                    state.m_aTrackQueue[0] = m_ActiveSettings.CurrTrackID;
                    state.m_aTrackTypes[0] = m_ActiveSettings.CurrTrackType;
                    state.m_aTrackQueue[1] = m_ActiveSettings.TrackQueue[0];
                    state.m_aTrackTypes[1] = m_ActiveSettings.TrackTypes[0];
                }
                break;
            }
        }
    }

    m_bInitialised        = false;
    m_bEnabledInPauseMode = false;

    if (CTimer::GetIsPaused() && !duringPause) {
        m_iRadioStationMenuRequest = -1;
        m_nRetuneStartedTime       = 0;
    }

    if (settings) {
        m_nStationsListed            = 0;
        m_nStationsListDown          = 0;
        m_iRadioStationScriptRequest = -1;
        m_bDisplayStationName        = false;
        m_bRetuneJustStarted         = false;

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.0f, 1.0f);

        if (m_ActiveSettings.StationID == RADIO_INVALID) {
            m_ActiveSettings.StationID = RADIO_OFF;
        }
        settings->RadioStation = m_ActiveSettings.StationID;
        settings->BassSetting  = m_ActiveSettings.BassSetting;

        if (m_nMode != eRadioTrackMode::RADIO_STOPPED || m_bInitialised || m_nStationsListed || m_nStationsListDown) {
            m_nSavedTimeMs         = CTimer::GetTimeInMS();
            m_nSavedGameClockDays  = CClock::ms_nGameClockDays;
            m_nSavedGameClockHours = CClock::ms_nGameClockHours;
            m_nSavedRadioStationId = m_ActiveSettings.StationID;
        }

        if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA) {
            m_ActiveSettings.StationID = (eRadioID)CAEAudioUtility::GetRandomNumberInRange<int32>(1, 13);
        }
    } else if (duringPause) {
        m_nStationsListed    = 0;
        m_nStationsListDown  = 0;
        m_bRetuneJustStarted = false;
    }
}

// 0x4E94C0
int32 CAERadioTrackManager::ChooseIdentIndex(eRadioID id) {
    const auto range = ms_aIdentIndexRanges[(int32)id];
    if (range.first == NO_SOUND_INDEX) {
        return -1;
    }

    const auto indexCount = range.second - range.first; // -1
    while (true) {
        const auto index = CAEAudioUtility::GetRandomNumberInRange(range.first, range.second);

        // 0x44C is the first ident of this station - it's only played if that mission was passed
        if (id == RADIO_MODERN_HIP_HOP
            && index == 0x44C
            && CStats::GetStatValue(STAT_ARE_YOU_GOING_TO_SAN_FIERRO_MISSION_ACCOMPLISHED) == 0.0f
        ) {
            continue;
        }

        // Re-roll if this ident was played recently
        const auto historyCount = std::min(indexCount - 1, (int32)IDENT_INDEX_HISTORY_COUNT);
        bool       wasPlayed    = false;
        for (auto i = 0; i < historyCount && !wasPlayed; i++) {
            wasPlayed = m_nIdentIndexHistory[id].indices[i] == index;
        }
        if (!wasPlayed) {
            return index;
        }
    }
}

// 0x4E9570
int32 CAERadioTrackManager::ChooseAdvertIndex(eRadioID id) {
    while (true) {
        const auto index = CAEAudioUtility::GetRandomNumberInRange(ms_anAdvertIndexRange.first, ms_anAdvertIndexRange.second);

        // Adverts already in the station's queue may not be picked again
        bool isInQueue = false;
        for (const auto queued : ms_aAdvertIndicesInQueue[id]) {
            if (queued == index) {
                isInQueue = true;
                break;
            }
        }
        if (isInQueue) {
            continue;
        }

        // ... nor may ones that were played recently
        if (rng::find(m_nAdvertIndexHistory[id].indices, index) == m_nAdvertIndexHistory[id].indices.end()) {
            return index;
        }
    }
}

// 0x4EA270
int8 CAERadioTrackManager::ChooseMusicTrackIndex(eRadioID id) {
    if (id == RADIO_TALK) {
        return ChooseTalkRadioShow();
    }

    const auto trackCount = ms_anMusicTrackCounts[(int32)id];
    while (true) {
        const auto index = (int8)CAEAudioUtility::GetRandomNumberInRange(0, trackCount - 1);

        // Re-roll if this track was played recently
        const auto historyCount = std::min(trackCount - 2, (int32)MUSIC_TRACK_HISTORY_COUNT);
        bool       wasPlayed    = false;
        for (auto i = 0; i < historyCount && !wasPlayed; i++) {
            wasPlayed = m_nMusicTrackIndexHistory[id].indices[i] == index;
        }
        if (!wasPlayed) {
            return index;
        }
    }
}

// 0x4EA2D0
int32 CAERadioTrackManager::ChooseDJBanterIndex(eRadioID id) {
    const auto station = (int32)id;

    // The original passes the raw address of the table as a `int32**`
    const auto AsList = [](auto& ranges) { return reinterpret_cast<int32**>(&ranges); };

    // A "special" DJ banter (played only once) might be pending
    int32 index = -1;
    switch (m_nSpecialDJBanterPending) { // todo: enum
    case 0:
        index = ms_aForcedDJBanterIndexRanges[station].first;
        break;
    case 1:
        if (m_nSpecialDJBanterIndex == 0
            || (m_nSpecialDJBanterIndex == 1
                && ms_aSpecialDJBanterIndexRanges[station].first != ms_aSpecialDJBanterIndexRanges[station].second
            )
        ) {
            const auto range = ms_aSpecialDJBanterIndexRanges[station];
            index = m_nSpecialDJBanterIndex == 0 ? range.first : range.second;
        }
        break;
    case 2:
        index = ms_anSpecialDJBanterIndices[station][(int8)m_nSpecialDJBanterIndex];
        break;
    }

    // The pending special banter may not be one that was played recently
    if (index == NO_SOUND_INDEX) {
        index = -1;
    } else if (index >= 0 && rng::find(m_nDJBanterIndexHistory[station].indices, index) != m_nDJBanterIndexHistory[station].indices.end()) {
        index = -1;
    }
    if (index != -1) {
        return index;
    }

    // No special banter - pick a regular one
    if (id == RADIO_EMERGENCY_AA) {
        return ChooseDJBanterIndexFromList(RADIO_EMERGENCY_AA, CGameLogic::LaRiotsActiveHere()
            ? AsList(ms_aDJBanterIndexRangesRiots)
            : AsList(ms_aDJBanterIndexRanges));
    }

    if (!CAEAudioUtility::ResolveProbability(0.6f)) {
        return index;
    }

    if (CGame::currArea != 0) {
        return index;
    }

    // Pick a list of banters based on the weather forecast...
    const auto hour = CClock::ms_nGameClockHours;
    if (!CWeather::ForecastWeather(WEATHER_RAINY_COUNTRYSIDE, 3) && !CWeather::ForecastWeather(WEATHER_RAINY_SF, 3)) {
        if ((!CWeather::ForecastWeather(WEATHER_EXTRASUNNY_LA, 3)
             && !CWeather::ForecastWeather(WEATHER_EXTRASUNNY_SMOG_LA, 3)
             && !CWeather::ForecastWeather(WEATHER_EXTRASUNNY_COUNTRYSIDE, 3)
             && !CWeather::ForecastWeather(WEATHER_EXTRASUNNY_SF, 3)
             && !CWeather::ForecastWeather(WEATHER_EXTRASUNNY_VEGAS, 3)
             && !CWeather::ForecastWeather(WEATHER_EXTRASUNNY_DESERT, 3))
            || hour < 8 || hour > 5
        ) {
            if (CWeather::ForecastWeather(WEATHER_FOGGY_SF, 3) && CAEAudioUtility::ResolveProbability(0.5f)) {
                if (const auto i = ChooseDJBanterIndexFromList(id, AsList(ms_aDJBanterIndexRangesFoggy)); i != -1) {
                    return i;
                }
            }
        } else {
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                if (const auto i = ChooseDJBanterIndexFromList(id, AsList(ms_aDJBanterIndexRangesClear)); i != -1) {
                    return i;
                }
            }
        }
    } else {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            if (const auto i = ChooseDJBanterIndexFromList(id, AsList(ms_aDJBanterIndexRangesRainy)); i != -1) {
                return i;
            }
        }
    }

    // ... or based on the time of day
    auto list = AsList(ms_aDJBanterIndexRanges);
    if (hour < 6 || hour > 8) {
        if (hour < 18 || hour > 20) {
            if ((hour < 22 && hour > 2) || !CAEAudioUtility::ResolveProbability(0.3f)) {
                return ChooseDJBanterIndexFromList(id, list);
            }
            list = AsList(ms_aDJBanterIndexRangesRiots);
        } else {
            if (!CAEAudioUtility::ResolveProbability(0.3f)) {
                return ChooseDJBanterIndexFromList(id, list);
            }
            list = AsList(ms_aDJBanterIndexRangesTimeA);
        }
    } else {
        if (!CAEAudioUtility::ResolveProbability(0.3f)) {
            return ChooseDJBanterIndexFromList(id, list);
        }
        list = AsList(ms_aDJBanterIndexRangesTimeB);
    }

    if (const auto i = ChooseDJBanterIndexFromList(id, list); i != -1) {
        return i;
    }
    return ChooseDJBanterIndexFromList(id, AsList(ms_aDJBanterIndexRanges));
}

// 0x4E95E0
int32 CAERadioTrackManager::ChooseDJBanterIndexFromList(eRadioID id, int32** list) {
    const auto* const ranges  = reinterpret_cast<const std::pair<int32, int32>*>(list);
    const auto        station = (int32)id;
    const auto        range   = ranges[station];
    if (range.first == NO_SOUND_INDEX) {
        return -1;
    }

    const auto count = range.second - range.first; // -1
    const auto index = CAEAudioUtility::GetRandomNumberInRange(0, count);
    if (count + 1 < 1) {
        return -1;
    }

    // Re-roll (by rotating the picked index) if it was played recently
    const auto historyCount = std::min(
        ms_aDJBanterIndexRanges[station].second - ms_aDJBanterIndexRanges[station].first - 1,
        (int32)DJBANTER_INDEX_HISTORY_COUNT
    );
    for (int32 offset = 0;; offset++) {
        const auto banterIndex = (offset + index) % (count + 1) + range.first;

        bool wasPlayed = false;
        for (auto i = 0; i < historyCount && !wasPlayed; i++) {
            wasPlayed = m_nDJBanterIndexHistory[station].indices[i] == banterIndex;
        }
        if (!wasPlayed) {
            return banterIndex;
        }
        if (count + 1 <= offset + 1) { // All of the list's entries were played recently
            return -1;
        }
    }
}

// 0x4EB180
void CAERadioTrackManager::ChooseTracksForStation(eRadioID id) {
    int8 trackCount = 0;

    for (auto i = 0u; i < tRadioSettings::NUM_TRACKS; i++) {
        m_RequestedSettings.TrackTypes[i] = TYPE_NONE;
        m_RequestedSettings.TrackQueue[i] = -1;
        m_RequestedSettings.TrackIndices[i] = -1;
    }

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
        if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
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
    AddMusicTrackIndexToHistory(id, m_RequestedSettings.TrackIndices[trackCount - 1]);

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
    int8 indices[0x23]; // 35
    rng::fill(indices, -1);

    int32 count = 0;

    // Mission specific shows
    if (CStats::GetStatValue(STAT_RYDERS_MISSION_ROBBING_UNCLE_SAM_ACCOMPLISHED) != 0.0f
        && CStats::GetStatValue(STAT_MIKE_TORENO_MISSION_ACCOMPLISHED) == 0.0f
    ) {
        indices[count++] = 0xE;
    }
    if (CStats::GetStatValue(STAT_ARCHITECTURAL_ESPIONAGE_MISSION_ACCOMPLISHED) != 0.0f) {
        indices[count++] = 0xF;
    }
    if (CStats::GetStatValue(STAT_JIZZY_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0xC;
    } else if (CStats::GetStatValue(STAT_ARCHITECTURAL_ESPIONAGE_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0xD;
    }
    if (CStats::GetStatValue(STAT_SMALL_TOWN_BANK_MISSION_ACCOMPLISHED) != 0.0f
        && CStats::GetStatValue(STAT_PHOTO_OPPORTUNITY_MISSION_ACCOMPLISHED) == 0.0f
    ) {
        indices[count++] = 6;
    }
    if (CStats::GetStatValue(STAT_DRIVE_THRU_MISSION_ACCOMPLISHED) == 0.0f
        || CStats::GetStatValue(STAT_REUNITING_THE_FAMILIES_MISSION_ACCOMPLISHED) != 0.0f
    ) {
        if (CStats::GetStatValue(STAT_PHOTO_OPPORTUNITY_MISSION_ACCOMPLISHED) != 0.0f
            && CStats::GetStatValue(STAT_DON_PEYOTE_MISSION_ACCOMPLISHED) == 0.0f
        ) {
            indices[count++] = 4;
        } else if (CStats::GetStatValue(STAT_DON_PEYOTE_MISSION_ACCOMPLISHED) != 0.0f) {
            indices[count++] = 5;
        }
    } else {
        indices[count++] = 3;
    }
    if (CStats::GetStatValue(STAT_LOCAL_LIQUOR_STORE_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 7;
    } else {
        indices[count++] = 8;
    }
    if (CStats::GetStatValue(STAT_BADLANDS_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 9;
    } else if (CStats::GetStatValue(STAT_555_WE_TIP_MISSION_ACCOMPLISHED) == 0.0f
        && CStats::GetStatValue(STAT_PLAYING_TIME) == 0.0f
    ) {
        indices[count++] = 10;
    } else if (CStats::GetStatValue(STAT_PLAYING_TIME) != 0.0f) {
        indices[count++] = 0xB;
    }
    if (CStats::GetStatValue(STAT_HIDDEN_PACKAGES_FOUND) == 0.0f) {
        indices[count++] = 0x1B;
    } else {
        indices[count++] = 0x1C;
    }
    if (CStats::GetStatValue(STAT_TAGS_SPRAYED) == 0.0f) {
        indices[count++] = 0x1D;
    } else {
        indices[count++] = 0x1E;
    }
    if (CStats::GetStatValue(STAT_LEAST_FAVORITE_GANG) == 0.0f) {
        indices[count++] = 0;
    } else if (CStats::GetStatValue(STAT_GANG_MEMBERS_WASTED) != 0.0f
        && CStats::GetStatValue(STAT_CRIMINALS_WASTED) == 0.0f
    ) {
        indices[count++] = 1;
    } else if (CStats::GetStatValue(STAT_MOST_FAVORITE_RADIO_STATION) != 0.0f) {
        indices[count++] = 2;
    }

    // Show of the first mission that wasn't passed yet
    if (CStats::GetStatValue(STAT_DRIVE_THRU_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x10;
    } else if (CStats::GetStatValue(STAT_MANAGEMENT_ISSUES_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x11;
    } else if (CStats::GetStatValue(STAT_LEAST_FAVORITE_GANG) == 0.0f) {
        indices[count++] = 0x12;
    } else if (CStats::GetStatValue(STAT_555_WE_TIP_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x13;
    } else if (CStats::GetStatValue(STAT_YAY_KA_BOOM_BOOM_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x14;
    } else if (CStats::GetStatValue(STAT_FISH_IN_A_BARREL_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x15;
    } else if (CStats::GetStatValue(STAT_BREAKING_THE_BANK_AT_CALIGULAS_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x16;
    } else if (CStats::GetStatValue(STAT_A_HOME_IN_THE_HILLS_MISSION_ACCOMPLISHED) == 0.0f) {
        indices[count++] = 0x17;
    } else if (CStats::GetStatValue(STAT_MAYBE_SET_RIOT_MODE) == 0.0f) {
        indices[count++] = 0x18;
    } else if (CStats::GetStatValue(STAT_CITY_UNLOCKED) != 4.0f) {
        indices[count++] = 0x19;
    } else {
        indices[count++] = 0x1A;
    }

    // Re-roll the picked show if it was played recently
    auto index = CAEAudioUtility::GetRandomNumberInRange(0, count - 1);
    if (count - 1 > 0) {
        while (true) {
            bool wasPlayed = false;
            for (auto i = 0; i < count - 1 && !wasPlayed; i++) {
                wasPlayed = m_nMusicTrackIndexHistory[RADIO_TALK].indices[i] == (int8)index;
            }
            if (!wasPlayed) {
                break;
            }
            index = CAEAudioUtility::GetRandomNumberInRange(0, count - 1);
        }
    }
    return indices[index];
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
    } else {
        const auto* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        
        if (settings && notsa::contains({
                AE_RT_CIVILIAN,
                AE_RT_EMERGENCY,
                AE_RT_UNKNOWN
            }, settings->RadioType)
            || CAudioEngine::IsAmbienceRadioActive()
        ) {
            m_bPauseMode = false;
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, 1.0f);
        } else {
            StopRadio(nullptr, false);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            m_bPauseMode = false;
        }
    }
}

// 0x4EB9A0
void CAERadioTrackManager::Service(int32 playTime) {
    ms_nPreviousTrackPlayTime = m_ActiveSettings.PlayTime;

    m_ActiveSettings.PlayTime      = playTime;
    m_ActiveSettings.TrackLengthMs = AEAudioHardware.GetTrackLengthMs();
    m_ActiveSettings.CurrTrackID   = AEAudioHardware.GetPlayingTrackID();

    if (!CTimer::GetIsPaused()) {
        CheckForMissionStatsChanges();
        CheckForStationRetune();
    } else {
        CheckForStationRetuneDuringPause();
    }

    // The requested settings are only applied once the radio has stopped
    if (m_bInitialised && m_nMode == eRadioTrackMode::RADIO_STOPPED) {
        if (m_RequestedSettings.StationID == RADIO_OFF) {
            m_ActiveSettings = m_RequestedSettings;
            if (const auto settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
                settings && notsa::contains({AE_RT_CIVILIAN, AE_RT_EMERGENCY, AE_RT_UNKNOWN}, settings->RadioType)
            ) {
                m_bDisplayStationName = true;
            } else {
                m_bInitialised = false;
            }
        } else if (!CAudioEngine::IsAmbienceTrackActive()) {
            m_ActiveSettings = m_RequestedSettings;
            m_nMode          = eRadioTrackMode::RADIO_STARTING;
            if (IsVehicleRadioActive()) {
                m_bDisplayStationName = true;
            } else {
                m_bInitialised = false;
            }
        }
    }

    switch (m_nMode) {
    case eRadioTrackMode::RADIO_STARTING:
        if (m_ActiveSettings.PlayTime < 0) {
            m_ActiveSettings.PlayTime = 0;
        }
        AEAudioHardware.SetBassSetting(
            m_bBassEnhance ? m_ActiveSettings.BassSetting : eBassSetting::NORMAL,
            m_ActiveSettings.BassGain
        );
        AEAudioHardware.PlayTrack(
            m_ActiveSettings.TrackQueue[0],
            m_ActiveSettings.TrackQueue[1],
            m_ActiveSettings.PlayTime,
            m_ActiveSettings.TrackFlags,
            m_ActiveSettings.TrackTypes[0] == TYPE_USER_TRACK,
            m_ActiveSettings.TrackTypes[1] == TYPE_USER_TRACK
        );
        m_nMode = eRadioTrackMode::RADIO_WAITING_TO_PLAY;
        return;
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
        if (m_ActiveSettings.PlayTime == -2) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.0f, 1.0f);
            StartTrackPlayback();
            m_nRetuneStartedTime = CTimer::GetTimeInMS();
            m_aRadioState[m_ActiveSettings.StationID].m_iTimeInPauseModeInMs = CTimer::GetTimeInMSPauseMode();
            m_nMode = eRadioTrackMode::RADIO_PLAYING;
            return;
        }
        if (m_ActiveSettings.PlayTime == -8
            && (m_ActiveSettings.CurrTrackID == m_ActiveSettings.TrackQueue[1]
                || (m_ActiveSettings.CurrTrackID == m_ActiveSettings.TrackQueue[0] && m_ActiveSettings.TrackQueue[1] == -1)
            )
        ) {
            // Fade in an advert followed by two user tracks
            m_ActiveSettings.TrackQueue[0]   = ChooseAdvertIndex(RADIO_USER_TRACKS);
            m_ActiveSettings.TrackTypes[0]   = TYPE_ADVERT;
            m_ActiveSettings.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
            m_ActiveSettings.TrackTypes[1]   = TYPE_USER_TRACK;
            m_ActiveSettings.TrackIndices[1] = (int8)m_ActiveSettings.TrackQueue[1];

            m_nMode = eRadioTrackMode::RADIO_STARTING;
            return;
        }
        break;
    case eRadioTrackMode::RADIO_PLAYING:
        if (m_ActiveSettings.StationID == RADIO_USER_TRACKS && m_ActiveSettings.PlayTime == -6) {
            // Switch to the next user track
            if (AEAudioHardware.GetActiveTrackID() == m_ActiveSettings.TrackQueue[0]
                && m_ActiveSettings.TrackQueue[1] != -1
            ) {
                m_ActiveSettings.TrackQueue[0]   = m_ActiveSettings.TrackQueue[1];
                m_ActiveSettings.TrackIndices[0] = m_ActiveSettings.TrackIndices[1];
                m_ActiveSettings.TrackTypes[0]   = m_ActiveSettings.TrackTypes[1];

                m_ActiveSettings.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                m_ActiveSettings.TrackTypes[1]   = TYPE_USER_TRACK;
                m_ActiveSettings.TrackIndices[1] = (int8)m_ActiveSettings.TrackQueue[1];
            } else {
                m_ActiveSettings.TrackQueue[0]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                m_ActiveSettings.TrackTypes[0]   = TYPE_USER_TRACK;
                m_ActiveSettings.TrackIndices[0] = (int8)m_ActiveSettings.TrackQueue[0];

                m_ActiveSettings.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                m_ActiveSettings.TrackTypes[1]   = TYPE_USER_TRACK;
                m_ActiveSettings.TrackIndices[1] = (int8)m_ActiveSettings.TrackQueue[1];
            }
            m_nMode = eRadioTrackMode::RADIO_STARTING;
        }

        // Remember the track that just started playing
        if (m_ActiveSettings.TrackQueue[0] == m_ActiveSettings.CurrTrackID) {
            switch (m_ActiveSettings.TrackTypes[0]) {
            case TYPE_INDENT:
                AddIdentIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.TrackQueue[0]);
                break;
            case TYPE_ADVERT:
                AddAdvertIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.TrackQueue[0]);
                break;
            case TYPE_DJ_BANTER:
                AddDJBanterIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.TrackQueue[0]);
                break;
            case TYPE_INTRO:
            case TYPE_TRACK:
            case TYPE_OUTRO:
            case TYPE_USER_TRACK:
                AddMusicTrackIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.TrackIndices[0]);
                break;
            }
            m_ActiveSettings.CurrTrackType = m_ActiveSettings.TrackTypes[0];
            m_ActiveSettings.CurrTrackIdx  = m_ActiveSettings.TrackIndices[0];
        } else if (m_ActiveSettings.PrevTrackID == m_ActiveSettings.CurrTrackID) {
            switch (m_ActiveSettings.PrevTrackType) {
            case TYPE_INDENT:
                AddIdentIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.PrevTrackID);
                break;
            case TYPE_ADVERT:
                AddAdvertIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.PrevTrackID);
                break;
            case TYPE_DJ_BANTER:
                AddDJBanterIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.PrevTrackID);
                break;
            case TYPE_INTRO:
            case TYPE_TRACK:
            case TYPE_OUTRO:
            case TYPE_USER_TRACK:
                AddMusicTrackIndexToHistory(m_ActiveSettings.StationID, m_ActiveSettings.PrevTrackIdx);
                break;
            }
            m_ActiveSettings.CurrTrackType = m_ActiveSettings.PrevTrackType;
            m_ActiveSettings.CurrTrackIdx  = m_ActiveSettings.PrevTrackIdx;
        }

        // Skip to the next user track
        if (m_ActiveSettings.StationID == RADIO_USER_TRACKS
            && (m_ActiveSettings.TrackTypes[0] == TYPE_USER_TRACK
                || AEUserRadioTrackManager.GetUserTrackPlayMode() == 0
            )
            && CPad::GetPad(0)->IsRadioTrackSkipPressed()
        ) {
            StopRadio(nullptr, true);
            while (m_nMode != eRadioTrackMode::RADIO_STOPPED
                || m_bInitialised
                || m_nStationsListed
                || m_nStationsListDown
            ) {
                Service(AEAudioHardware.GetTrackPlayTime());
                AEAudioHardware.Service();
            }
            StartRadio(m_ActiveSettings.StationID, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, true);
        }

        CheckForPause();
        UpdateRadioVolumes();
        CheckForTrackConcatenation();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP, 0.0f, 1.0f);
        return;
    case eRadioTrackMode::RADIO_STOPPING:
    case eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED:
        AEAudioHardware.StopTrack();
        m_nMode = eRadioTrackMode::RADIO_WAITING_TO_STOP;
        return;
    case eRadioTrackMode::RADIO_STOPPING_SILENCED:
        m_nMode = eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED;
        return;
    case eRadioTrackMode::RADIO_WAITING_TO_STOP:
        if (m_ActiveSettings.PlayTime == -6 || m_ActiveSettings.PlayTime == -8) {
            m_nMode = eRadioTrackMode::RADIO_STOPPED;
        } else if (m_ActiveSettings.PlayTime == -7 || m_ActiveSettings.PlayTime == -2) {
            AEAudioHardware.StopTrack();
        }
        return;
    }
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
