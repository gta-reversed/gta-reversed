#include "StdInc.h"

#include "AERadioTrackManager.h"
#include "RadioStreamsPC.h"

#include "AEAudioHardware.h"
#include "AEUserRadioTrackManager.h"
#include "AEAudioUtility.h"
#include "AEAudioEnvironment.h"

auto& AERadioTrackManager = StaticRef<CAERadioTrackManager>(0x8CB6F8);

// The music track lists of the stations that have their own tracks (`RADIO_EMERGENCY_AA` .. `RADIO_TALK`)
using TrackRange               = CAERadioTrackManager::TrackRange;
static auto& s_MusicTracks     = StaticRef<notsa::mdarray<int32, RADIO_USER_TRACKS, 31>>(0x8C9040);
static auto& s_MusicIntros     = StaticRef<notsa::mdarray<TrackRange, RADIO_USER_TRACKS, 31>>(0x8C9610);
static auto& s_MusicOutros     = StaticRef<notsa::mdarray<TrackRange, RADIO_USER_TRACKS, 31>>(0x8CA1B0);
static auto& s_TalkShowLengths = StaticRef<std::array<int32, 32>>(0x8CAD50); //!< Length of each WCTR show [ms]

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
    RH_ScopedInstall(ChooseDJBanterIndex, 0x4EA2D0, { .Reversed = false });
    RH_ScopedInstall(ChooseDJBanterIndexFromList, 0x4E95E0, { .Reversed = false });
    RH_ScopedInstall(ChooseAdvertIndex, 0x4E9570, { .Reversed = false });
    RH_ScopedInstall(ChooseIdentIndex, 0x4E94C0, { .Reversed = false });
    RH_ScopedInstall(ChooseMusicTrackIndex, 0x4EA270, { .Reversed = false });
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
    RH_ScopedInstall(IsRadioOn, 0x4E8350);
    RH_ScopedInstall(InitialiseRadioStationID, 0x4E8330);
    RH_ScopedInstall(SetBassEnhanceOnOff, 0x4E9DB0);
    RH_ScopedInstall(SetBassSetting, 0x4E82F0);
    RH_ScopedInstall(SetRadioAutoRetuneOnOff, 0x4E82E0);
    RH_ScopedInstall(RetuneRadio, 0x4E8290);
    RH_ScopedInstall(ResetStatistics, 0x4E8200);
    RH_ScopedInstall(Reset, 0x4E7F80);
}

// Code from 0x5B9390
CAERadioTrackManager::CAERadioTrackManager(int32 hwClientHandle) :
    m_HwClientHandle{ hwClientHandle },
    m_UserTrackPlayMode{ AEUserRadioTrackManager.GetUserTrackPlayMode() } {
    // All constant value inits are done using member init lists

    rng::copy(CStats::GetFullFavoriteRadioStationList(), m_ListenTimes.begin());

    for (auto i = 0u; i < RADIO_COUNT; i++) {
        ms_MusicTrackIndexHistory[i].Reset();
        ms_DJBanterIndexHistory[i].Reset();
        ms_AdvertIndexHistory[i].Reset();
        ms_IdentIndexHistory[i].Reset();
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
    m_IsInitialised      = false;
    m_DisplayStationName = false;
    rng::copy(CStats::GetFullFavoriteRadioStationList(), m_ListenTimes.begin());

    rng::for_each(ms_DJBanterIndexHistory, &DJBanterIndexHistory::Reset);
    rng::for_each(ms_AdvertIndexHistory, &AdvertIndexHistory::Reset);
    rng::for_each(ms_IdentIndexHistory, &IdentIndexHistory::Reset);
    rng::for_each(ms_MusicTrackIndexHistory, &MusicTrackHistory::Reset);
    rng::for_each(m_RadioStates, [](auto& s) {
        s.Reset();
    });

    m_RequestedSettings = m_ActiveSettings = tRadioSettings{CAEAudioUtility::GetRandomRadioStation()};
    m_StationsListed = m_StationsListDown = 0;
    m_TimeRadioStationRetuned = m_TimeToDisplayRadioName = 0;
    m_Prev = m_PlayBeginTime = 0;
    m_RetuneStartedTime = 0;
    m_EnabledInPauseMode = false;
    m_SavedGameClockDays = m_SavedGameClockHours = -1;
    m_RadioAutoSelect = m_BassEnhance = true;
    m_SavedRadioStationId = m_RadioStationMenuRequest = m_RadioStationScriptRequest = RADIO_INVALID;
    ms_SpecialDJBanterPending = 3; // todo: enum
    ms_SpecialDJBanterIndex = -1;
    m_PauseMode = m_RetuneJustStarted = false;
    m_RadioDuckedVolume = m_RadioDuckFadeRate = 0.0f;
    ResetStatistics();
}

// 0x4E8200
void CAERadioTrackManager::ResetStatistics() {
    ms_StatsCitiesPassed          = 0;
    ms_StatsLastHitGameClockDays  = -1;
    ms_StatsLastHitGameClockHours = -1;
    ms_StatsLastHitTimeOutHours   = -1;
    ms_StatsPassedCasino3         = false;
    ms_StatsPassedCasino6         = false;
    ms_StatsPassedCasino10        = false;
    ms_StatsPassedCat1            = false;
    ms_StatsPassedDesert1         = false;
    ms_StatsPassedDesert3         = false;
    ms_StatsPassedDesert5         = false;
    ms_StatsPassedDesert8         = false;
    ms_StatsPassedDesert10        = false;
    ms_StatsPassedFarlie3         = false;
    ms_StatsPassedLAFin2          = false;
    ms_StatsPassedMansion2        = false;
    ms_StatsPassedRyder2          = false;
    ms_StatsPassedRiot1           = false;
    ms_StatsPassedSCrash1         = false;
    ms_StatsPassedStrap4          = false;
    ms_StatsPassedSweet2          = false;
    ms_StatsPassedTruth2          = false;
    ms_StatsPassedVCrash2         = false;
    ms_StatsStartedBadlands       = false;
    ms_StatsStartedCat2           = false;
    ms_StatsStartedCrash1         = false;
}

// 0x4E8350
bool CAERadioTrackManager::IsRadioOn() const {
    return m_Mode != eRadioTrackMode::RADIO_STOPPED || m_IsInitialised || m_StationsListed || m_StationsListDown;
}

// 0x4E8370
bool CAERadioTrackManager::HasRadioRetuneJustStarted() const {
    return m_RetuneJustStarted;
}

// 0x4E83E0
int32* CAERadioTrackManager::GetRadioStationListenTimes() {
    return m_ListenTimes.data();
}

// 0x4E83F0
eRadioID CAERadioTrackManager::GetCurrentRadioStationID() const {
    return m_ActiveSettings.StationID == RADIO_INVALID ? RADIO_OFF : m_ActiveSettings.StationID;
}

// 0x4E82E0
void CAERadioTrackManager::SetRadioAutoRetuneOnOff(bool enable) {
    m_RadioAutoSelect = enable;
}

// 0x4E82F0
void CAERadioTrackManager::SetBassSetting(eBassSetting bassSetting, float bassGrain) {
    m_RequestedSettings.BassGain = m_ActiveSettings.BassGain = bassGrain;
    m_RequestedSettings.BassSetting = m_ActiveSettings.BassSetting = bassSetting;
    AEAudioHardware.SetBassSetting(m_BassEnhance ? bassSetting : eBassSetting::NORMAL, bassGrain);
}

// 0x4E9DB0
void CAERadioTrackManager::SetBassEnhanceOnOff(bool enable) {
    m_BassEnhance = enable;
    if (m_Mode == eRadioTrackMode::RADIO_PLAYING) {
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
        m_RadioStationMenuRequest = retunedStation;
        m_RetuneStartedTime       = CTimer::GetTimeInMSPauseMode();
    } else {
        m_RadioStationScriptRequest = retunedStation;
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

    if (m_DisplayStationName && IsVehicleRadioActive()) {
        m_TimeToDisplayRadioName = CTimer::GetTimeInMS() + 2'500;
        m_DisplayStationName     = false;
    }

    if (CTimer::GetTimeInMS() < m_TimeToDisplayRadioName) {
        int station = m_StationsListed + m_RequestedSettings.StationID;
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
            CFont::SetColor(HudColour.GetRGB(m_StationsListed || m_StationsListDown ? HUD_COLOUR_DARK_GRAY : HUD_COLOUR_GOLD));
            CFont::PrintString(SCREEN_WIDTH / 2, SCREEN_SCALE_Y(22.0f), GetRadioStationName((eRadioID)station));
            CFont::DrawFonts();
        }
    }
}

// 0x4E9E10
const GxtChar* CAERadioTrackManager::GetRadioStationName(eRadioID id) {
    if (id <= RADIO_EMERGENCY_AA) {
        assert(id == RADIO_EMERGENCY_AA); // Only emergency radio is expected
        return nullptr;
    }

    char key[8];
    GetRadioStationNameKey(id, key);
    return TheText.Get(key);
}

// 0x4E8380
void CAERadioTrackManager::GetRadioStationNameKey(eRadioID id, char(&outStr)[8]) {
    switch (id) {
    case RADIO_OFF:
        notsa::string_copy(outStr, "FEA_NON");
        break;
    case RADIO_USER_TRACKS:
        notsa::string_copy(outStr, "FEA_MP3");
        break;
    default:
        assert(0 <= id && id < RADIO_USER_TRACKS);
        notsa::format_to_sz(outStr, "FEA_R{:d}", (int32)id - 1);
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
    if (ms_SpecialDJBanterPending != 3) {
        if (CClock::GetGameClockHours() + 24 * CClock::GetNumberOfDaysPassed(ms_StatsLastHitGameClockDays) - ms_StatsLastHitGameClockHours >= ms_StatsLastHitTimeOutHours) {
            ms_SpecialDJBanterPending = 3;
        }
    }

    const auto statsCitiesPassed = CStats::GetStatValue<uint8>(STAT_CITY_UNLOCKED);
    if (ms_StatsCitiesPassed < statsCitiesPassed) {
        ms_StatsCitiesPassed = statsCitiesPassed;
        if (statsCitiesPassed == 1 || statsCitiesPassed == 2) {
            ms_StatsLastHitGameClockDays  = CClock::GetGameClockDays();
            ms_StatsLastHitGameClockHours = CClock::GetGameClockHours();
            ms_StatsLastHitTimeOutHours   = 24;
            ms_SpecialDJBanterPending     = 1;
            ms_SpecialDJBanterIndex       = ms_StatsCitiesPassed - 1;
        }
    }

    const auto Update = [](uint8& inputStat, const eStats stat, const auto specialDJBanterIndex) {
        const auto statValue = CStats::GetStatValue<uint8>(stat);
        if (inputStat < statValue) {
            inputStat = statValue;
            if (inputStat == 1) {
                ms_StatsLastHitGameClockDays  = CClock::GetGameClockDays();
                ms_StatsLastHitGameClockHours = CClock::GetGameClockHours();
                ms_StatsLastHitTimeOutHours   = 24 * 7;
                ms_SpecialDJBanterPending     = 2;
                ms_SpecialDJBanterIndex       = specialDJBanterIndex;
            }
        }
    };

    Update(ms_StatsPassedCasino3, STAT_LEAST_FAVORITE_RADIO_STATION, 0);
    Update(ms_StatsPassedCasino6, STAT_CURRENT_WEAPON_SKILL, 1);
    Update(ms_StatsPassedCasino10, STAT_WEAPON_SKILL_LEVELS, 2);
    Update(ms_StatsPassedCat1, STAT_LOCAL_LIQUOR_STORE_MISSION_ACCOMPLISHED, 3);
    Update(ms_StatsPassedDesert1, STAT_PLAYING_TIME, 4);
    Update(ms_StatsPassedDesert3, STAT_PILOT_RANKING, 5);
    Update(ms_StatsPassedDesert5, STAT_STRONGEST_GANG, 6);
    Update(ms_StatsPassedDesert8, STAT_2ND_STRONGEST_GANG, 7);
    Update(ms_StatsPassedDesert10, STAT_3RD_STRONGEST_GANG, 8);
    Update(ms_StatsPassedFarlie3, STAT_MIKE_TORENO_MISSION_ACCOMPLISHED, 9);
    Update(ms_StatsPassedLAFin2, STAT_LEAST_FAVORITE_GANG, 10);
    Update(ms_StatsPassedMansion2, STAT_A_HOME_IN_THE_HILLS_MISSION_ACCOMPLISHED, 11);
    Update(ms_StatsPassedRyder2, STAT_RYDERS_MISSION_ROBBING_UNCLE_SAM_ACCOMPLISHED, 12);
    Update(ms_StatsPassedRiot1, STAT_RIOT_MISSION_ACCOMPLISHED, 13);
    Update(ms_StatsPassedSCrash1, STAT_GANG_STRENGTH, 14);
    Update(ms_StatsPassedStrap4, STAT_TERRITORY_UNDER_CONTROL, 15);
    Update(ms_StatsPassedSweet2, STAT_DRIVE_THRU_MISSION_ACCOMPLISHED, 16);
    Update(ms_StatsPassedTruth2, STAT_ARE_YOU_GOING_TO_SAN_FIERRO_MISSION_ACCOMPLISHED, 17);
    Update(ms_StatsPassedVCrash2, STAT_HIGH_NOON_MISSION_ACCOMPLISHED, 18);
    Update(ms_StatsStartedBadlands, STAT_THE_GREEN_SABRE_MISSION_ACCOMPLISHED, 19);
    Update(ms_StatsStartedCat2, STAT_MAYBE_CATALINA_MEETING, 20);
    Update(ms_StatsStartedCrash1, STAT_MAYBE_WU_ZI_MEETING, 21);
}

// 0x4EA930
void CAERadioTrackManager::CheckForTrackConcatenation() {
    auto& as                    = m_ActiveSettings;
    int8  trackCount            = 1; // The one playing right now stays in front

    const auto PlayQueuedTracks = [&] {
        AEAudioHardware.PlayTrack(
            as.TrackQueue[0],
            as.TrackQueue[1],
            0,
            as.TrackFlags,
            as.TrackTypes[0] == eTrackType::USER_TRACK,
            as.TrackTypes[1] == eTrackType::USER_TRACK
        );
    };
    const auto QueueUserTrack = [&](int8 slot) {
        as.TrackQueue[slot]   = AEUserRadioTrackManager.SelectUserTrackIndex();
        as.TrackTypes[slot]   = eTrackType::USER_TRACK;
        as.TrackIndices[slot] = (int8)as.TrackQueue[slot];
    };

    // The user track play mode was changed in the menu, so the next track has to be picked again
    if (as.StationID == RADIO_USER_TRACKS) {
        const auto prevMode = m_UserTrackPlayMode;
        if (prevMode != AEUserRadioTrackManager.GetUserTrackPlayMode()) {
            if ((prevMode == RADIO_MODE_SEQUENTIAL || AEUserRadioTrackManager.GetUserTrackPlayMode() == RADIO_MODE_SEQUENTIAL) && as.PlayTime != -4) {
                AEUserRadioTrackManager.SetUserTrackIndex(as.TrackQueue[0]);
                QueueUserTrack(1);
                trackCount = 2;
                PlayQueuedTracks();
            }
            m_UserTrackPlayMode = AEUserRadioTrackManager.GetUserTrackPlayMode();
        }
    }

    // Wait until the hardware has gone on to the next track
    const auto nextTrack = as.TrackQueue[1];
    if (AEAudioHardware.GetActiveTrackID() != nextTrack || nextTrack < 0) {
        return;
    }
    as.SwitchToNextTrack();

    if (as.TrackQueue[1] == -1) { // Nothing left after this one, queue up more
        const auto id = as.StationID;
        if (id == RADIO_USER_TRACKS) { // 0x4EAAA5
            if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
                as.TrackQueue[trackCount]   = ChooseAdvertIndex(RADIO_USER_TRACKS);
                as.TrackTypes[trackCount++] = eTrackType::ADVERT;
            }
            QueueUserTrack(trackCount++);
            QueueUserTrack(trackCount);
        } else {
            const auto QueueUp = [&](eTrackType type) {
                return QueueUpTracksForStation(id, &trackCount, type, as);
            };
            switch (as.TrackTypes[0]) {
            case eTrackType::INTRO:
            case eTrackType::TRACK:
            case eTrackType::OUTRO: { // 0x4EAB37 - A song is on, something else may come after it
                if (id == RADIO_EMERGENCY_AA) {
                    QueueUp(eTrackType::DJ_BANTER);
                } else if (m_TracksInARow[id] < 2 && CAEAudioUtility::ResolveProbability(0.5f)) {
                    if (CAEAudioUtility::ResolveProbability(0.5f)) {
                        QueueUp(eTrackType::INDENT);
                    }
                    QueueUp(eTrackType::INTRO);
                } else {
                    if (CAEAudioUtility::ResolveProbability(0.5f)) {
                        QueueUp(eTrackType::INDENT);
                    }
                    if (!QueueUp(eTrackType::DJ_BANTER)) {
                        QueueUp(eTrackType::ADVERT);
                    }
                }
                break;
            }
            case eTrackType::INDENT:
            case eTrackType::ADVERT:
            case eTrackType::DJ_BANTER:
            case eTrackType::NONE:
            case eTrackType::USER_TRACK: // 0x4EAB29
                QueueUp(eTrackType::INTRO);
                break;
            default:
                NOTSA_UNREACHABLE("unexpected track type: {}", as.TrackTypes[0]);
            }
        }
    }
    PlayQueuedTracks();
}

// 0x4EB660
void CAERadioTrackManager::CheckForStationRetune() {
    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA) {
        return;
    }
    m_RetuneJustStarted = false;
    if ((notsa::contains({ eRadioTrackMode::RADIO_STARTING, eRadioTrackMode::RADIO_WAITING_TO_PLAY, eRadioTrackMode::RADIO_PLAYING }, m_Mode)
         || m_IsInitialised
         || m_StationsListed != 0
         || m_StationsListDown != 0
         || m_ActiveSettings.StationID == RADIO_OFF)
        && AudioEngine.GetCutsceneTrackStatus() == 0) {
        if (const auto vs = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio(); vs) {
            if (notsa::contains({ AE_RT_CIVILIAN, AE_RT_EMERGENCY,  AE_RT_UNKNOWN }, vs->RadioType)
                && CReplay::Mode != MODE_PLAYBACK)
            {
                if (vs->RadioType != AE_RT_CIVILIAN) {
                    return;
                }

                if (m_RadioStationScriptRequest >= 0) {
                    m_StationsListDown          = m_StationsListed;
                    m_StationsListed            = m_RadioStationScriptRequest - m_RequestedSettings.StationID;
                    m_RadioStationScriptRequest = -1;
                    m_TimeRadioStationRetuned   = CTimer::GetTimeInMS();
                    m_DisplayStationName        = true;
                    m_RetuneJustStarted         = true;
                } else if (CPad::GetPad()->NextStationJustUp()) {
                    m_StationsListDown        = m_StationsListed++;
                    m_TimeRadioStationRetuned = CTimer::GetTimeInMS();
                    m_DisplayStationName      = true;
                    m_RetuneJustStarted       = true;
                } else if (CPad::GetPad()->LastStationJustUp()) {
                    m_StationsListDown        = m_StationsListed--;
                    m_TimeRadioStationRetuned = CTimer::GetTimeInMS();
                    m_DisplayStationName      = true;
                    m_RetuneJustStarted       = true;
                }
            }
        }
    }

    if (!m_StationsListed && !m_StationsListDown) {
        return;
    }

    eRadioID newStation = static_cast<eRadioID>(m_StationsListed % 256 + m_RequestedSettings.StationID);
    // wrap the radios in range of Playback FM (id 1) to Radio off. (id 13)
    if (newStation > RADIO_EMERGENCY_AA) {
        if (newStation >= RADIO_COUNT) {
            newStation = static_cast<eRadioID>(newStation - RADIO_COUNT + RADIO_CLASSIC_HIP_HOP);
        }
    } else {
        newStation = static_cast<eRadioID>(newStation + RADIO_COUNT - RADIO_CLASSIC_HIP_HOP);
    }

    const auto Start = [this, newStation]() {
        StartRadio(
            newStation,
            m_ActiveSettings.BassSetting,
            m_ActiveSettings.BassGain,
            false
        );
        m_StationsListed = m_StationsListDown = 0;
    };

    if (newStation == RADIO_OFF || newStation == RADIO_USER_TRACKS && !AEUserRadioTrackManager.m_nUserTracksCount) {
        // turn off the radio while it's playing
        StopRadio(nullptr, false);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        Start();
        return;
    }

    if (m_ActiveSettings.StationID == RADIO_OFF) {
        // turn on the radio now
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON);
        m_ActiveSettings.StationID = RADIO_INVALID;
    } else {
        StopRadio(nullptr, false);
    }
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);

    if (CTimer::GetTimeInMS() > m_TimeRadioStationRetuned + 1'500
        && CTimer::GetTimeInMS() > m_PlayBeginTime + (TheCamera.m_fCameraAverageSpeed > 0.9f ? 4'000 : 2'000)) {
        Start();
    }
}

// 0x4EB890
void CAERadioTrackManager::CheckForStationRetuneDuringPause() {
    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA && IsRadioOn() || m_RadioStationMenuRequest <= RADIO_INVALID) {
        return;
    }

    if (m_RadioStationMenuRequest != RADIO_OFF) {
        if (m_ActiveSettings.StationID == RADIO_OFF) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_ON);
            m_ActiveSettings.StationID = RADIO_INVALID;
        } else {
            AudioEngine.StopRadio(nullptr, true);
        }

        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        if (CTimer::GetTimeInMSPauseMode() > m_RetuneStartedTime + 700u) {
            StartRadio((eRadioID)m_RadioStationMenuRequest, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
            m_RadioStationMenuRequest = RADIO_INVALID;
        }
    } else {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_CLICK_OFF);
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        StartRadio(RADIO_OFF, m_ActiveSettings.BassSetting, m_ActiveSettings.BassGain, 0);
        m_RadioStationMenuRequest = RADIO_INVALID;
    }
}

// 0x4EA640
void CAERadioTrackManager::StartTrackPlayback() {
    AEAudioHardware.SetChannelFlags(
        m_HwClientHandle,
        0,
        eAudioChannelFlags(FLAG_SECONDARY_GROUP | FLAG_UNDUCKABLE | FLAG_CLAMP_VOL_TO_NEG | FLAG_IS_MUSIC | FLAG_IS_NOT_STREAM)
    );
    AEAudioHardware.StartTrackPlayback();
    UpdateRadioVolumes();
}

// 0x4EA010
void CAERadioTrackManager::UpdateRadioVolumes() {
    auto volume = -4.0f;
    if (!CTimer::GetIsPaused() || !m_EnabledInPauseMode) {
        if (CTimer::GetIsSlowMotionActive()) {
            volume = -100.0f;
        }
        else if (TheCamera.m_bWideScreenOn) {
            volume = -16.0f;
        }
        else if (AEAudioHardware.GetEffectsMasterScalingFactor() > 0.0f && AEAudioHardware.GetMusicMasterScalingFactor() > 0.0f) {
            if (!CAEPedSpeechAudioEntity::s_bForceAudible) {
                uint8 audio = 0;
                for (; audio < 2; audio++) {
                    if (!AudioEngine.IsMissionAudioSampleFinished(audio) && AudioEngine.GetMissionAudioEvent(audio) != -1) {
                        const auto missionAudioPosition = AudioEngine.GetMissionAudioPosition(audio);

                        // If there's no position OR it's within 15.0f, recalculate and break
                        if (!missionAudioPosition || CAEAudioEnvironment::GetPositionRelativeToCamera(*missionAudioPosition).Magnitude() <= 15.0f) {
                            const auto musicScale  = AEAudioHardware.GetMusicMasterScalingFactor();
                            const auto effectScale = AEAudioHardware.GetEffectsMasterScalingFactor();
                            m_RadioDuckedVolume = std::min(CAEAudioUtility::AudioLog10(effectScale / musicScale) * 20.0f - 9.0f, 0.0f);
                            m_RadioDuckFadeRate = -0.02f * m_RadioDuckedVolume;
                            volume = m_RadioDuckedVolume - 4.0f;
                            break;
                        }
                    }
                }

                // no recalculation happened
                if (audio >= 2) {
                    if (m_RadioDuckedVolume < 0.0f) {
                        m_RadioDuckedVolume = std::min(m_RadioDuckedVolume + m_RadioDuckFadeRate, 0.0f);
                        volume = m_RadioDuckedVolume - 4.0f;
                    }
                }
            }
        }
        if (AudioEngine.IsAmbienceRadioActive()) {
            volume -= 20.0f;
        }
    }

    if (m_BassEnhance) {
        switch (m_ActiveSettings.BassSetting) {
        case eBassSetting::BOOST:
            volume -= 2.0f;
            break;
        case eBassSetting::CUT:
            volume += 1.5f;
            break;
        case eBassSetting::NORMAL:
            break;
        default:
            NOTSA_UNREACHABLE("unexpected bass setting: {}", m_ActiveSettings.BassSetting);
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
    // plugin::CallMethod<0x4EB550, CAERadioTrackManager*, tVehicleAudioSettings*>(this, settings);

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (settings.RadioType == AE_RT_EMERGENCY) {
        StartRadio(RADIO_EMERGENCY_AA, settings.BassSetting, settings.BassFactor, 0);
        return;
    }

    if (settings.RadioType != AE_RT_CIVILIAN)
        return;

    const bool needsRetune = [&] {
        if (!m_RadioAutoSelect) {
            return false;
        }

        const auto savedId = m_SavedRadioStationId;
        if (savedId < 0 || savedId == settings.RadioStation || savedId == RADIO_OFF || savedId == RADIO_EMERGENCY_AA) {
            return false;
        }

        if (CTimer::GetTimeInMS() > m_SavedTimeMs + 60'000) {
            return false;
        }

        if (m_SavedGameClockHours < 0 || m_SavedGameClockDays < 0) {
            return false;
        }

        if (CClock::GetGameClockHours() + 24 * CClock::GetNumberOfDaysPassed(m_SavedGameClockDays) - m_SavedGameClockHours > 5) {
            return false;
        }

        return true;
    }();

    if (needsRetune) {
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_START);
        StartRadio((eRadioID)m_SavedRadioStationId, settings.BassSetting, settings.BassFactor, 0);
    } else {
        StartRadio(settings.RadioStation, settings.BassSetting, settings.BassFactor, 0);
    }
}

// 0x4EB3C0
void CAERadioTrackManager::StartRadio(eRadioID id, eBassSetting bassSetting, float bassGain, bool skipTrack) {
    id = std::min(id, RADIO_OFF);

    if (CTimer::GetIsPaused()) {
        m_EnabledInPauseMode = true;

        if (IsRadioOn() && id == m_ActiveSettings.StationID) {
            m_RadioStates[id].TimeInPauseModeInMs = CTimer::GetTimeInMSPauseMode();
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
        rs->ClearTracks();
    } else if (m_RadioStates[id].TimeInMs < 0 || !TrackRadioStation(id, skipTrack)) {
        ChooseTracksForStation(rs->StationID);
        rs->PlayTime = CAEAudioUtility::GetRandomNumberInRange(0, 300'000);
    }
    switch (m_Mode) {
    case eRadioTrackMode::RADIO_STARTING:
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
    case eRadioTrackMode::RADIO_PLAYING:
        m_Mode = eRadioTrackMode::RADIO_STOPPING;
    }
    m_RadioStates[rs->StationID].TimeInPauseModeInMs = -1;

    m_IsInitialised                                  = true;
}

// 0x4EAC30
bool CAERadioTrackManager::TrackRadioStation(eRadioID id, bool skipTrack) {
    auto& st         = m_RadioStates[id];
    auto& rs         = m_RequestedSettings;
    int8  trackCount = 0;

    if (st.GameClockHours >= 0 && st.GameClockDays >= 0) {
        if (CClock::GetGameClockHours() + 24 * CClock::GetNumberOfDaysPassed(st.GameClockDays) - st.GameClockHours > 5) {
            return false;
        }
    }

    const auto timeAway = [&] {
        const auto t = std::max((int32)CTimer::GetTimeInMS() - st.TimeInMs, 7'000);
        return skipTrack
            ? std::max(t, st.Elapsed[0] + 1)
            : t;
    }();

    rs.ClearTracks();

    const auto QueueUp = [&](eRadioID station, eTrackType type) {
        return QueueUpTracksForStation(station, &trackCount, type, rs);
    };

    // Continue the tracks that were queued up when the station was left
    auto totalElapsed = 0;
    for (auto i = 0u; i < st.Elapsed.size(); i++) {
        totalElapsed += st.Elapsed[i];
        if (timeAway > totalElapsed) {
            continue;
        }
        rs.PlayTime = i == 0
            ? st.TrackPlayTime + timeAway
            : st.Elapsed[i] - totalElapsed + timeAway;

        const auto RestoreQueue = [&](size_t num) {
            // BUGFIX: The original always copies `num` tracks, reading past `TrackQueue` / `TrackTypes` when fewer were saved from `i` on
            const auto        numSaved = notsa::bugfixes::GenericOOB ? std::min(num, st.TrackQueue.size() - i) : num;
            const auto* const queue    = st.TrackQueue.data();
            const auto* const types    = st.TrackTypes.data();
            for (auto j = 0u; j < numSaved; j++) {
                rs.TrackQueue[j] = queue[i + j];
                rs.TrackTypes[j] = types[i + j];
            }
        };

        switch (st.TrackTypes[i]) {
        case eTrackType::INDENT:
        case eTrackType::ADVERT:
        case eTrackType::DJ_BANTER: { // 0x4EADEE
            RestoreQueue(1);
            trackCount = 1;
            if (id == RADIO_USER_TRACKS) {
                QueueUp(RADIO_USER_TRACKS, eTrackType::TRACK);
            } else {
                QueueUp(id, eTrackType::INTRO);
            }
            return true;
        }
        case eTrackType::OUTRO: { // 0x4EAE44
            RestoreQueue(1);
            trackCount = 1;
            if (id == RADIO_EMERGENCY_AA) {
                QueueUp(RADIO_EMERGENCY_AA, eTrackType::DJ_BANTER);
                return true;
            }
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                QueueUp(id, eTrackType::INDENT);
            }
            if (!QueueUp(id, eTrackType::DJ_BANTER)) {
                QueueUp(id, eTrackType::ADVERT);
            }
            return true;
        }
        case eTrackType::INTRO: // 0x4EAED5
            RestoreQueue(3);
            return true;
        case eTrackType::TRACK: // 0x4EAF80
            RestoreQueue(2);
            return true;
        case eTrackType::USER_TRACK: { // 0x4EAF17
            RestoreQueue(2);
            if (rs.TrackQueue[1] == -1) {
                rs.TrackQueue[1]   = AEUserRadioTrackManager.SelectUserTrackIndex();
                rs.TrackTypes[1]   = eTrackType::USER_TRACK;
                rs.TrackIndices[1] = (int8)rs.TrackQueue[1];
            }
            return true;
        }
        case eTrackType::NONE: // 0x4EB14B
            return false;
        default:
            NOTSA_UNREACHABLE("unexpected track type: {}", st.TrackTypes[i]);
        }
    }

    // All of them are over by now, pretend the station played on
    if (timeAway <= totalElapsed + 7'000) { // 0x4EAD6B
        if (id == RADIO_USER_TRACKS) {
            QueueUp(RADIO_USER_TRACKS, eTrackType::TRACK);
            QueueUp(RADIO_USER_TRACKS, eTrackType::TRACK);
        } else {
            QueueUp(id, eTrackType::INTRO);
        }
        rs.PlayTime = std::min(timeAway - totalElapsed, 5'000);
        return true;
    }

    if (timeAway <= totalElapsed + 155'000) { // 0x4EB004
        QueueUp(id, eTrackType::TRACK);
        rs.PlayTime = timeAway - totalElapsed - 5'000;
        return true;
    }

    if (timeAway > totalElapsed + 160'000) {
        return false;
    }

    // 0x4EB03D
    if (id == RADIO_USER_TRACKS) {
        QueueUp(RADIO_USER_TRACKS, eTrackType::TRACK);
        if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
            QueueUp(RADIO_USER_TRACKS, eTrackType::ADVERT);
        }
    } else {
        QueueUp(id, eTrackType::OUTRO);
        AddMusicTrackIndexToHistory(id, rs.TrackIndices[trackCount - 1]);
        if (id == RADIO_EMERGENCY_AA) {
            QueueUp(RADIO_EMERGENCY_AA, eTrackType::DJ_BANTER);
        } else if (CAEAudioUtility::ResolveProbability(0.5f)) {
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                QueueUp(id, eTrackType::INDENT);
            }
            QueueUp(id, eTrackType::INTRO);
        } else {
            if (CAEAudioUtility::ResolveProbability(0.5f)) {
                QueueUp(id, eTrackType::INDENT);
            }
            if (!QueueUp(id, eTrackType::DJ_BANTER)) {
                QueueUp(id, eTrackType::ADVERT);
            }
        }
    }
    rs.PlayTime = timeAway - totalElapsed - 155'000;
    return true;
}

// 0x4EA670
bool CAERadioTrackManager::QueueUpTracksForStation(eRadioID id, int8* trackCount, eTrackType trackType, tRadioSettings& settings) {
    auto& n         = *trackCount;

    const auto Push = [&](eTrackType type, int32 trackId, int8 musicIdx) {
        settings.TrackQueue[n]   = trackId;
        settings.TrackTypes[n]   = type;
        settings.TrackIndices[n] = musicIdx;
        n++;
    };
    const auto GetRandomTrackIn = [](const TrackRange& range) {
        return CAEAudioUtility::GetRandomNumberInRange(range[0], range[1]);
    };

    switch (trackType) {
    case eTrackType::INDENT: { // 0x4EA871
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackQueue[n] = ChooseIdentIndex(id);
        if (settings.TrackQueue[n] == -1) {
            return false;
        }
        settings.TrackTypes[n++] = eTrackType::INDENT;
        return true;
    }
    case eTrackType::ADVERT: { // 0x4EA8AD - NOTE: Doesn't set `TrackIndices`, neither does the original
        settings.TrackQueue[n]   = ChooseAdvertIndex(id);
        settings.TrackTypes[n++] = eTrackType::ADVERT;
        return true;
    }
    case eTrackType::DJ_BANTER: { // 0x4EA8DB
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        settings.TrackQueue[n] = ChooseDJBanterIndex(id);
        if (settings.TrackQueue[n] == -1) {
            return false;
        }
        settings.TrackTypes[n++] = eTrackType::DJ_BANTER;
        return true;
    }
    case eTrackType::INTRO: { // 0x4EA732
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        const auto musicIdx = ChooseMusicTrackIndex(id);
        Push(eTrackType::INTRO, GetRandomTrackIn(s_MusicIntros[id][musicIdx]), musicIdx);
        Push(eTrackType::TRACK, s_MusicTracks[id][musicIdx], musicIdx);
        Push(eTrackType::OUTRO, GetRandomTrackIn(s_MusicOutros[id][musicIdx]), musicIdx);
        return true;
    }
    case eTrackType::TRACK: { // 0x4EA688
        if (id == RADIO_USER_TRACKS) {
            const auto userTrack = AEUserRadioTrackManager.SelectUserTrackIndex();
            Push(eTrackType::USER_TRACK, userTrack, (int8)userTrack);
            return true;
        }
        const auto musicIdx = ChooseMusicTrackIndex(id);
        Push(eTrackType::TRACK, s_MusicTracks[id][musicIdx], musicIdx);
        Push(eTrackType::OUTRO, GetRandomTrackIn(s_MusicOutros[id][musicIdx]), musicIdx);
        return true;
    }
    case eTrackType::OUTRO: { // 0x4EA80A
        if (id == RADIO_USER_TRACKS) {
            return false;
        }
        const auto musicIdx = ChooseMusicTrackIndex(id);
        Push(eTrackType::OUTRO, GetRandomTrackIn(s_MusicOutros[id][musicIdx]), musicIdx);
        return true;
    }
    case eTrackType::NONE:
    case eTrackType::USER_TRACK: // 0x4EA90F
        return true;
    default:
        NOTSA_UNREACHABLE("unexpected track type: {}", trackType);
    }
}

// 0x4E9820
void CAERadioTrackManager::StopRadio(tVehicleAudioSettings* settings, bool duringPause) {
    if (notsa::contains({ eRadioTrackMode::RADIO_STARTING, eRadioTrackMode::RADIO_WAITING_TO_PLAY, eRadioTrackMode::RADIO_PLAYING }, m_Mode)) {
        if (!CTimer::GetIsPaused() || duringPause) {
            m_Mode = eRadioTrackMode::RADIO_STOPPING;
        }

        // Remember where the station was, `TrackRadioStation` continues from here
        const auto& as = m_ActiveSettings;
        const auto  id = as.StationID;
        auto&       st = m_RadioStates[id];
        st.Reset(true);
        st.TimeInMs       = CTimer::GetTimeInMS();
        st.GameClockDays  = (int8)CClock::GetGameClockDays();
        st.GameClockHours = (int8)CClock::GetGameClockHours();

        if (st.TimeInPauseModeInMs >= 0 && id != RADIO_EMERGENCY_AA && id != RADIO_OFF) {
            m_ListenTimes[id] += CTimer::GetTimeInMSPauseMode() - st.TimeInPauseModeInMs;
        }

        if (id != RADIO_OFF) {
            st.Elapsed[0] = as.TrackLengthMs - as.PlayTime - 100;

            // Saves `num` tracks, starting with the one playing right now
            const auto SaveQueue = [&](size_t num) {
                st.TrackPlayTime = as.PlayTime;
                if (as.TrackQueue[0] == as.CurrTrackID) {
                    for (auto i = 0u; i < num; i++) {
                        st.TrackQueue[i] = as.TrackQueue[i];
                        st.TrackTypes[i] = as.TrackTypes[i];
                    }
                } else {
                    st.TrackQueue[0] = as.CurrTrackID;
                    st.TrackTypes[0] = as.CurrTrackType;
                    for (auto i = 1u; i < num; i++) {
                        st.TrackQueue[i] = as.TrackQueue[i - 1];
                        st.TrackTypes[i] = as.TrackTypes[i - 1];
                    }
                }
            };

            switch (as.CurrTrackType) {
            case eTrackType::INDENT:
            case eTrackType::ADVERT:
            case eTrackType::DJ_BANTER:
            case eTrackType::OUTRO: // 0x4E99E9
                st.TrackPlayTime = as.PlayTime;
                st.TrackQueue[0] = as.CurrTrackID;
                st.TrackTypes[0] = as.CurrTrackType;
                break;
            case eTrackType::INTRO: // 0x4E9A33
                st.Elapsed[1] = id == RADIO_TALK
                    ? s_TalkShowLengths[as.TrackIndices[0]]
                    : 150'000;
                st.Elapsed[2] = 5'000;
                SaveQueue(3);
                break;
            case eTrackType::TRACK:
            case eTrackType::USER_TRACK: // 0x4E9BB5
                st.Elapsed[1] = 5'000;
                SaveQueue(2);
                break;
            case eTrackType::NONE:
                break;
            default:
                NOTSA_UNREACHABLE("unexpected track type: {}", as.CurrTrackType);
            }
        }
    }

    m_IsInitialised      = false;
    m_EnabledInPauseMode = false;
    if (CTimer::GetIsPaused() && !duringPause) {
        m_RadioStationMenuRequest = RADIO_INVALID;
        m_RetuneStartedTime       = 0;
    }

    if (!settings) {
        if (duringPause) {
            m_StationsListed    = 0;
            m_StationsListDown  = 0;
            m_RetuneJustStarted = false;
        }
        return;
    }

    m_StationsListed            = 0;
    m_StationsListDown          = 0;
    m_RadioStationScriptRequest = RADIO_INVALID;
    m_DisplayStationName        = false;
    m_RetuneJustStarted         = false;
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);

    // Give the station back to the vehicle, so it plays it the next time
    if (m_ActiveSettings.StationID == RADIO_INVALID) {
        m_ActiveSettings.StationID = RADIO_OFF;
    }
    settings->RadioStation = m_ActiveSettings.StationID;
    settings->BassSetting  = m_ActiveSettings.BassSetting;

    if (IsRadioOn()) { // 0x4E9D2B
        m_SavedTimeMs         = CTimer::GetTimeInMS();
        m_SavedGameClockDays  = CClock::GetGameClockDays();
        m_SavedGameClockHours = CClock::GetGameClockHours();
        m_SavedRadioStationId = m_ActiveSettings.StationID;
    }

    if (m_ActiveSettings.StationID == RADIO_EMERGENCY_AA) {
        m_ActiveSettings.StationID = CAEAudioUtility::GetRandomRadioStation();
    }
}

// 0x4E94C0
int32 CAERadioTrackManager::ChooseIdentIndex(eRadioID id) {
    return plugin::CallMethodAndReturn<int32, 0x4E94C0, CAERadioTrackManager*, eRadioID>(this, id);
}

// 0x4E9570
int32 CAERadioTrackManager::ChooseAdvertIndex(eRadioID id) {
    return plugin::CallMethodAndReturn<int32, 0x4E9570, CAERadioTrackManager*, eRadioID>(this, id);
}

// 0x4EA270
int8 CAERadioTrackManager::ChooseMusicTrackIndex(eRadioID id) {
    return plugin::CallMethodAndReturn<int8, 0x4EA270, CAERadioTrackManager*, eRadioID>(this, id);
}

// 0x4EA2D0
int32 CAERadioTrackManager::ChooseDJBanterIndex(eRadioID id) {
    return plugin::CallMethodAndReturn<int32, 0x4EA2D0, CAERadioTrackManager*, eRadioID>(this, id);
}

// 0x4E95E0
int32 CAERadioTrackManager::ChooseDJBanterIndexFromList(eRadioID id, const TrackRange* list) {
    return plugin::CallMethodAndReturn<int32, 0x4E95E0, CAERadioTrackManager*, eRadioID, const TrackRange*>(this, id, list);
}

// 0x4EB180
void CAERadioTrackManager::ChooseTracksForStation(eRadioID id) {
    int8 trackCount = 0;

    for (auto i = 0u; i < tRadioSettings::NUM_TRACKS; i++) {
        m_RequestedSettings.TrackTypes[i] = eTrackType::NONE;
        m_RequestedSettings.TrackQueue[i] = -1;
        m_RequestedSettings.TrackIndices[i] = -1;
    }

    if (!CAEAudioUtility::ResolveProbability(0.95f)) {
        if (id) {
            if (CAEAudioUtility::ResolveProbability(0.5f))
                QueueUpTracksForStation(id, &trackCount, eTrackType::INDENT, m_RequestedSettings);

            if (!QueueUpTracksForStation(id, &trackCount, eTrackType::DJ_BANTER, m_RequestedSettings))
                QueueUpTracksForStation(id, &trackCount, eTrackType::ADVERT, m_RequestedSettings);

            if (id == RADIO_USER_TRACKS) {
                QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, eTrackType::TRACK, m_RequestedSettings);
                return;
            }
        } else {
            QueueUpTracksForStation(RADIO_EMERGENCY_AA, &trackCount, eTrackType::DJ_BANTER, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, eTrackType::INTRO, m_RequestedSettings);
        return;
    }

    if (id == RADIO_USER_TRACKS) {
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, eTrackType::TRACK, m_RequestedSettings);
        QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, eTrackType::TRACK, m_RequestedSettings);
        if (!FrontEndMenuManager.m_RadioMode && CAEAudioUtility::ResolveProbability(0.17f)) {
            QueueUpTracksForStation(RADIO_USER_TRACKS, &trackCount, eTrackType::ADVERT, m_RequestedSettings);
        }
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.9f)) {
        QueueUpTracksForStation(id, &trackCount, eTrackType::TRACK, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, eTrackType::INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, eTrackType::INTRO, m_RequestedSettings);
        return;
    }

    QueueUpTracksForStation(id, &trackCount, eTrackType::OUTRO, m_RequestedSettings);
    AddMusicTrackIndexToHistory(id, m_RequestedSettings.TrackIndices[trackCount - 1]);

    if (id == RADIO_EMERGENCY_AA) {
        QueueUpTracksForStation(id, &trackCount, eTrackType::DJ_BANTER, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f)) {
        if (CAEAudioUtility::ResolveProbability(0.5f)) {
            QueueUpTracksForStation(id, &trackCount, eTrackType::INDENT, m_RequestedSettings);
        }
        QueueUpTracksForStation(id, &trackCount, eTrackType::INTRO, m_RequestedSettings);
        return;
    }

    if (CAEAudioUtility::ResolveProbability(0.5f))
        QueueUpTracksForStation(id, &trackCount, eTrackType::INDENT, m_RequestedSettings);

    if (!QueueUpTracksForStation(id, &trackCount, eTrackType::DJ_BANTER, m_RequestedSettings))
        QueueUpTracksForStation(id, &trackCount, eTrackType::ADVERT, m_RequestedSettings);
}

// 0x4E8E40
int8 CAERadioTrackManager::ChooseTalkRadioShow() {
    static auto& s_Blacklist = StaticRef<std::array<int8, 256>>(0xB62C1C);

    std::array<int8, 32> availableShows{};
    size_t numOptions{};
    rng::fill(availableShows, -1);

    if (CStats::GetStatValue(STAT_RYDERS_MISSION_ROBBING_UNCLE_SAM_ACCOMPLISHED) != 0.0f && CStats::GetStatValue(STAT_MIKE_TORENO_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 14;
    } else if (CStats::GetStatValue(STAT_ARCHITECTURAL_ESPIONAGE_MISSION_ACCOMPLISHED) != 0.0f) {
        availableShows[numOptions++] = 15;
    }

    if (CStats::GetStatValue(STAT_JIZZY_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 12;
    } else if (CStats::GetStatValue(STAT_ARCHITECTURAL_ESPIONAGE_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 13;
    }

    if (CStats::GetStatValue(STAT_SMALL_TOWN_BANK_MISSION_ACCOMPLISHED) != 0.0f && CStats::GetStatValue(STAT_PHOTO_OPPORTUNITY_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 6;
    }

    if (CStats::GetStatValue(STAT_DRIVE_THRU_MISSION_ACCOMPLISHED) != 0.0f && CStats::GetStatValue(STAT_REUNITING_THE_FAMILIES_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 3;
    } else if (CStats::GetStatValue(STAT_PHOTO_OPPORTUNITY_MISSION_ACCOMPLISHED) != 0.0f && CStats::GetStatValue(STAT_DON_PEYOTE_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 4;
    } else if (CStats::GetStatValue(STAT_DON_PEYOTE_MISSION_ACCOMPLISHED) != 0.0f) {
        availableShows[numOptions++] = 5;
    }

    if (CStats::GetStatValue(STAT_LOCAL_LIQUOR_STORE_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 7;
    } else {
        availableShows[numOptions++] = 8;
    }

    if (CStats::GetStatValue(STAT_BADLANDS_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 9;
    } else if (CStats::GetStatValue(STAT_555_WE_TIP_MISSION_ACCOMPLISHED) == 0.0f && CStats::GetStatValue(STAT_PLAYING_TIME) == 0.0f) {
        availableShows[numOptions++] = 10;
    } else if (CStats::GetStatValue(STAT_PLAYING_TIME) != 0.0f) {
        availableShows[numOptions++] = 11;
    }

    availableShows[numOptions++] = CStats::GetStatValue(STAT_HIDDEN_PACKAGES_FOUND) == 0.0f ? 27 : 28;
    availableShows[numOptions++] = CStats::GetStatValue(STAT_TAGS_SPRAYED) == 0.0f ? 29 : 30;

    if (CStats::GetStatValue(STAT_LEAST_FAVORITE_GANG) == 0.0f) {
        availableShows[numOptions++] = 0;
    } else if (CStats::GetStatValue(STAT_GANG_MEMBERS_WASTED) != 0.0f && CStats::GetStatValue(STAT_CRIMINALS_WASTED) == 0.0f) {
        availableShows[numOptions++] = 1;
    } else if (CStats::GetStatValue(STAT_MOST_FAVORITE_RADIO_STATION) != 0.0f) {
        availableShows[numOptions++] = 2;
    }

    if (CStats::GetStatValue(STAT_DRIVE_THRU_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 16;
    } else if (CStats::GetStatValue(STAT_MANAGEMENT_ISSUES_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 17;
    } else if (CStats::GetStatValue(STAT_LEAST_FAVORITE_GANG) == 0.0f) {
        availableShows[numOptions++] = 18;
    } else if (CStats::GetStatValue(STAT_555_WE_TIP_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 19;
    } else if (CStats::GetStatValue(STAT_YAY_KA_BOOM_BOOM_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 20;
    } else if (CStats::GetStatValue(STAT_FISH_IN_A_BARREL_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 21;
    } else if (CStats::GetStatValue(STAT_BREAKING_THE_BANK_AT_CALIGULAS_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 22;
    } else if (CStats::GetStatValue(STAT_A_HOME_IN_THE_HILLS_MISSION_ACCOMPLISHED) == 0.0f) {
        availableShows[numOptions++] = 23;
    } else if (CStats::GetStatValue(STAT_MAYBE_SET_RIOT_MODE) == 0.0f) {
        availableShows[numOptions++] = 24;
    } else if (CStats::GetStatValue(STAT_CITY_UNLOCKED) != 4.0f) {
        availableShows[numOptions++] = 25;
    } else {
        availableShows[numOptions++] = 26;
    }

    // NOTSA: Original logic was, find random entry -> check if it's blacklisted -> return if not, otherwise repeat.
    std::span shows{availableShows.data(), numOptions};
    rng::shuffle(shows, std::mt19937{ std::random_device{}() });

    for (int8 show : shows) {
        if (!rng::contains(s_Blacklist | rngv::take(numOptions - 1), show)) {
            // it's not already played? so we return
            return show;
        }
    }
    NOTSA_UNREACHABLE("no show left!");
}

// 0x4E96C0
void CAERadioTrackManager::AddMusicTrackIndexToHistory(eRadioID id, int8 trackIndex) {
    if (trackIndex >= 0 && ms_MusicTrackIndexHistory[id].indices[0] != trackIndex) {
        ms_MusicTrackIndexHistory[id].PutAtFirst(trackIndex);
        m_TracksInARow[id]++;
    }
}

// 0x4E9720
void CAERadioTrackManager::AddIdentIndexToHistory(eRadioID id, int32 trackIndex) {
    if (ms_IdentIndexHistory[id].indices[0] != trackIndex) {
        ms_IdentIndexHistory[id].PutAtFirst(trackIndex);
    }
}

// 0x4E9760
void CAERadioTrackManager::AddAdvertIndexToHistory(eRadioID id, int32 trackIndex) {
    if (ms_AdvertIndexHistory[id].indices[0] != trackIndex) {
        ms_AdvertIndexHistory[id].PutAtFirst(trackIndex);
        m_TracksInARow[id] = 0;
    }
}

// 0x4E97B0
void CAERadioTrackManager::AddDJBanterIndexToHistory(eRadioID id, int32 trackIndex) {
    if (ms_DJBanterIndexHistory[id].indices[0] != trackIndex) {
        ms_DJBanterIndexHistory[id].PutAtFirst(trackIndex);
        m_TracksInARow[id] = 0;
    }
}

// 0x4EA590
void CAERadioTrackManager::CheckForPause() {
    if (CTimer::GetIsPaused()) {
        m_PauseMode = true;
        AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, m_EnabledInPauseMode ? 1.0f : 0.0f);
    } else {
        const auto* settings = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();
        
        if (settings && notsa::contains({
                AE_RT_CIVILIAN,
                AE_RT_EMERGENCY,
                AE_RT_UNKNOWN
            }, settings->RadioType)
            || CAudioEngine::IsAmbienceRadioActive()
        ) {
            m_PauseMode = false;
            AEAudioHardware.SetChannelFrequencyScalingFactor(m_HwClientHandle, 0, 1.0f);
        } else {
            StopRadio(nullptr, false);
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            m_PauseMode = false;
        }
    }
}

// 0x4EB9A0
void CAERadioTrackManager::Service(int32 playTime) {
    m_ActiveSettings.PlayTime = playTime;
    m_ActiveSettings.TrackLengthMs = AEAudioHardware.GetTrackLengthMs();
    m_ActiveSettings.CurrTrackID = AEAudioHardware.GetPlayingTrackID();

    if (CTimer::GetIsPaused()) {
        CheckForStationRetuneDuringPause();
    } else {
        CheckForMissionStatsChanges();
        CheckForStationRetune();
    }

    if (m_IsInitialised && m_Mode == eRadioTrackMode::RADIO_STOPPED) {
        if (m_RequestedSettings.StationID == eRadioID::RADIO_OFF) {
            m_ActiveSettings = m_RequestedSettings;
            m_IsInitialised  = false;

            const auto vs = CAEVehicleAudioEntity::StaticGetPlayerVehicleAudioSettingsForRadio();

            if (vs && (vs->RadioType != eAERadioType::AE_RT_CIVILIAN || notsa::contains({eAERadioType::AE_RT_EMERGENCY, eAERadioType::AE_RT_UNKNOWN}, vs->RadioType))) {
                m_DisplayStationName = true;
            }
        } else if (!CAudioEngine::IsAmbienceTrackActive()) {
            m_ActiveSettings = m_RequestedSettings;
            m_Mode           = eRadioTrackMode::RADIO_STARTING;
            if (IsVehicleRadioActive()) {
                m_DisplayStationName = true;
            }
            m_IsInitialised = false;
        }
    }

    enum
    {
        PLAYTIME_FOR_USERTRAK_ADVERT = -8,
        PLAYTIME_UNK_MIN7 = -7,
        PLAYTIME_UNK_MIN6 = -6,
        PLAYTIME_FOR_RETUNE = -2,
    };

    auto& as = m_ActiveSettings;

    const auto AddToHistory = [this, &as](eTrackType type, int8 index, int32 queue) {
        switch (type) {
            case eTrackType::INTRO:
            case eTrackType::TRACK:
            case eTrackType::OUTRO:
            case eTrackType::USER_TRACK:
                AddMusicTrackIndexToHistory(as.StationID, index);
                break;
            case eTrackType::ADVERT:
                AddAdvertIndexToHistory(as.StationID, queue);
                break;
            case eTrackType::DJ_BANTER:
                AddDJBanterIndexToHistory(as.StationID, queue);
                break;
            case eTrackType::INDENT:
                AddIdentIndexToHistory(as.StationID, queue);
                break;
            case eTrackType::NONE:
                break;
            default:
                NOTSA_UNREACHABLE("unexpected track type: {}", type);
        }
        as.CurrTrackType = type;
        as.CurrTrackIdx = index;
    };

    const auto QueueUserTrackForNext = [&] {
        as.TrackQueue[1] = AEUserRadioTrackManager.SelectUserTrackIndex();
        as.TrackTypes[1] = eTrackType::USER_TRACK;
        as.TrackIndices[1] =  as.TrackQueue[1];
    };

    switch (m_Mode) {
    case eRadioTrackMode::RADIO_STARTING:
        as.PlayTime = std::max(as.PlayTime, 0);
        AEAudioHardware.SetBassSetting(m_BassEnhance ? as.BassSetting : eBassSetting::NORMAL, as.BassGain);
        AEAudioHardware.PlayTrack(
            as.TrackQueue[0],
            as.TrackQueue[1],
            as.PlayTime,
            as.TrackFlags,
            as.TrackTypes[0] == eTrackType::USER_TRACK,
            as.TrackTypes[1] == eTrackType::USER_TRACK
        );
        m_Mode = eRadioTrackMode::RADIO_WAITING_TO_PLAY;
        break;
    case eRadioTrackMode::RADIO_WAITING_TO_PLAY:
        if (as.PlayTime == PLAYTIME_FOR_RETUNE) {
            AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
            StartTrackPlayback();
            m_PlayBeginTime = CTimer::GetTimeInMS();
            m_RadioStates[as.StationID].TimeInPauseModeInMs = CTimer::GetTimeInMSPauseMode();
            m_Mode                                          = eRadioTrackMode::RADIO_PLAYING;
        } else if (as.PlayTime == PLAYTIME_FOR_USERTRAK_ADVERT) {
            const auto curTrack = as.CurrTrackID;
            const auto nextTrack = as.TrackQueue[1];
            if (curTrack == nextTrack || curTrack == as.TrackQueue[0] && nextTrack == -1) {
                as.TrackQueue[0] = ChooseAdvertIndex(RADIO_USER_TRACKS);
                as.TrackTypes[0] = eTrackType::ADVERT;
                QueueUserTrackForNext();
                m_Mode = eRadioTrackMode::RADIO_STARTING;
            }
        }
        break;
    case eRadioTrackMode::RADIO_PLAYING:
        if (as.StationID == RADIO_USER_TRACKS && as.PlayTime == PLAYTIME_UNK_MIN6) {
            const auto nextTrack = as.TrackQueue[1];
            if (AEAudioHardware.GetActiveTrackID() != as.TrackQueue[0] || nextTrack == -1) {
                as.TrackQueue[0] = AEUserRadioTrackManager.SelectUserTrackIndex();
                as.TrackTypes[0] = eTrackType::USER_TRACK;
                as.TrackIndices[0] = as.TrackQueue[0];
                QueueUserTrackForNext();
            } else {
                as.TrackQueue[0] = nextTrack;
                as.TrackTypes[0] = as.TrackTypes[1];
                as.TrackIndices[0] = as.TrackIndices[1];
                QueueUserTrackForNext();
            }
            m_Mode = eRadioTrackMode::RADIO_STARTING;
        }

        if (as.TrackQueue[0] == as.CurrTrackID) {
            AddToHistory(as.TrackTypes[0], as.TrackIndices[0], as.TrackQueue[0]);
        } else if (as.PrevTrackID == as.CurrTrackID) {
            AddToHistory(as.PrevTrackType, as.PrevTrackIdx, as.PrevTrackID);
        }

        if (as.StationID == RADIO_USER_TRACKS
            && (as.TrackTypes[0] == eTrackType::USER_TRACK || AEUserRadioTrackManager.GetUserTrackPlayMode() == eRadioMode::RADIO_MODE_RADIO))
        {
            if (CPad::GetPad()->IsRadioTrackSkipPressed()) {
                StopRadio(nullptr, true);
                while (m_Mode != eRadioTrackMode::RADIO_STOPPED || m_IsInitialised || m_StationsListed || m_StationsListDown) {
                    Service(AEAudioHardware.GetTrackPlayTime()); // wtf
                    AEAudioHardware.Service();
                }
                StartRadio(as.StationID, as.BassSetting, as.BassGain, true);
            }
        }
        CheckForPause();
        UpdateRadioVolumes();
        CheckForTrackConcatenation();
        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_RADIO_RETUNE_STOP);
        break;
    case eRadioTrackMode::RADIO_STOPPING:
    case eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED:
        AEAudioHardware.StopTrack();
        m_Mode = eRadioTrackMode::RADIO_WAITING_TO_STOP;
        break;
    case eRadioTrackMode::RADIO_STOPPING_SILENCED:
        m_Mode = eRadioTrackMode::RADIO_STOPPING_CHANNELS_STOPPED;
        break;
    case eRadioTrackMode::RADIO_WAITING_TO_STOP:
        if (as.PlayTime == PLAYTIME_UNK_MIN6 || as.PlayTime == PLAYTIME_FOR_USERTRAK_ADVERT) {
            m_Mode = eRadioTrackMode::RADIO_STOPPED;
        } else if (as.PlayTime == PLAYTIME_UNK_MIN7 || as.PlayTime == PLAYTIME_FOR_RETUNE) {
            AEAudioHardware.StopTrack();
        }
        break;
    case eRadioTrackMode::RADIO_STOPPED:
        return;
    default:
        NOTSA_UNREACHABLE("unexpected radio track mgr state: {}", m_Mode);
    }
}

// 0x5D40E0
void CAERadioTrackManager::Load() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : ms_MusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(historyIndex);
        }

        for (auto& identIndex : ms_IdentIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(identIndex);
        }

        for (auto& advertIndex : ms_AdvertIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : ms_DJBanterIndexHistory[r].indices) {
            CGenericGameStorage::LoadDataFromWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsCitiesPassed);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedCasino3);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedCasino6);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedCasino10);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedCat1);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedDesert1);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedDesert3);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedDesert5);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedDesert8);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedDesert10);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedFarlie3);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedLAFin2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedMansion2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedRyder2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedRiot1);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedSCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedStrap4);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedSweet2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedTruth2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsPassedVCrash2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsStartedBadlands);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsStartedCat2);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsStartedCrash1);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsLastHitGameClockDays);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsLastHitGameClockHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_StatsLastHitTimeOutHours);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_SpecialDJBanterPending);
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_SpecialDJBanterIndex);
}

// 0x5D3EE0
void CAERadioTrackManager::Save() {
    for (auto r = 0; r < RADIO_COUNT; r++) {
        for (auto& historyIndex : ms_MusicTrackIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(historyIndex);
        }

        for (auto& identIndex : ms_IdentIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(identIndex);
        }

        for (auto& advertIndex : ms_AdvertIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(advertIndex);
        }

        for (auto& banterIndex : ms_DJBanterIndexHistory[r].indices) {
            CGenericGameStorage::SaveDataToWorkBuffer(banterIndex);
        }
    }

    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsCitiesPassed);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedCasino3);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedCasino6);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedCasino10);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedCat1);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedDesert1);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedDesert3);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedDesert5);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedDesert8);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedDesert10);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedFarlie3);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedLAFin2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedMansion2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedRyder2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedRiot1);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedSCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedStrap4);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedSweet2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedTruth2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsPassedVCrash2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsStartedBadlands);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsStartedCat2);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsStartedCrash1);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsLastHitGameClockDays);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsLastHitGameClockHours);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_StatsLastHitTimeOutHours);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_SpecialDJBanterPending);
    CGenericGameStorage::SaveDataToWorkBuffer(ms_SpecialDJBanterIndex);
}
