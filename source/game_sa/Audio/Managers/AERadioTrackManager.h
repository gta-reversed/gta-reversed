/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

enum class eTrackType : int8 {
    INDENT      = 0,
    ADVERT      = 1,
    DJ_BANTER   = 2,
    INTRO       = 3,
    TRACK       = 4,
    OUTRO       = 5,
    NONE        = 6,
    USER_TRACK  = 7,
};

struct tRadioSettings {
    static constexpr size_t NUM_TRACKS = 5u;

    tRadioSettings(eRadioID currentStation = RADIO_OFF) :
        StationID(currentStation)
    {
        rng::fill(TrackQueue, -1);
        rng::fill(TrackTypes, eTrackType::NONE);
        rng::fill(TrackIndices, -1);
    }

    //! NOTSA: Empties the queue, the other fields are kept
    void ClearTracks() {
        rng::fill(TrackQueue, -1);
        rng::fill(TrackTypes, eTrackType::NONE);
        rng::fill(TrackIndices, -1);
    }

    void SwitchToNextTrack() {
        PrevTrackID   = TrackQueue.front();
        PrevTrackType = TrackTypes.front();
        PrevTrackIdx  = TrackIndices.front();

        const auto Rotate = [](auto& arr, auto invalidValue) {
            std::copy(arr.begin() + 1, arr.end(), arr.begin());
            arr.back() = invalidValue;
        };
        Rotate(TrackQueue,   -1);
        Rotate(TrackTypes,   eTrackType::NONE);
        Rotate(TrackIndices, -1);
    }

    std::array<int32, NUM_TRACKS>      TrackQueue{ -1 };
    int32                              CurrTrackID{ -1 };
    int32                              PrevTrackID{ -1 };
    int32                              PlayTime{ 0 };
    int32                              TrackLengthMs{ 0 };
    int8                               TrackFlags{ 2 };        // TODO: enum
    eRadioID                           StationID{ RADIO_OFF }; // NOTSA init value.
    eBassSetting                       BassSetting{ eBassSetting::NORMAL };
    float                              BassGain{}; // unk. init
    std::array<eTrackType, NUM_TRACKS> TrackTypes{ eTrackType::NONE };
    eTrackType                         CurrTrackType{ eTrackType::NONE };
    eTrackType                         PrevTrackType{ eTrackType::NONE };
    std::array<int8, NUM_TRACKS>       TrackIndices{ -1 };
    int8                               CurrTrackIdx{ -1 }; //!< Index into `TrackIndices`
    int8                               PrevTrackIdx{ -1 }; //!< Index into `TrackIndices`
};
VALIDATE_SIZE(tRadioSettings, 0x3C);

struct tRadioState {
    std::array<int32, 3>      Elapsed{ 0 };
    int32                     TimeInPauseModeInMs{ -1 };
    int32                     TimeInMs{ -1 };
    int32                     TrackPlayTime{ -1 };
    std::array<int32, 3>      TrackQueue{ -1 };
    std::array<eTrackType, 3> TrackTypes{ eTrackType::NONE };
    int8                      GameClockDays{ -1 };
    int8                      GameClockHours{ -1 };

    void Reset(bool paused = false) {
        rng::fill(Elapsed, 0);
        rng::fill(TrackQueue, -1);
        rng::fill(TrackTypes, eTrackType::NONE);

        TimeInMs = -1;
        if (!paused)
            TimeInPauseModeInMs = -1;
        TrackPlayTime  = -1;
        GameClockDays  = -1;
        GameClockHours = -1;
    }
};
VALIDATE_SIZE(tRadioState, 0x2C);

//typedef int8 RadioStationId; => eRadioID

// NOTSA
template<typename T, size_t Count>
struct tRadioIndexHistory {
    std::array<T, Count> indices{-1};

    void Reset() {
        rng::fill(indices, -1);
    }

    void PutAtFirst(int32 index) {
        if constexpr (Count > 1) {
            // rotate all elements to right.
            std::rotate(indices.rbegin(), indices.rbegin() + 1, indices.rend());
        }
        indices[0] = index;
    }
};
static_assert(sizeof(tRadioIndexHistory<int32, 1>) == sizeof(int32)); // No VALIDATE_SIZE because the preprocessor is dumb

enum class eRadioTrackMode {
    RADIO_STARTING,
    RADIO_WAITING_TO_PLAY,
    RADIO_PLAYING,
    RADIO_STOPPING, // ?
    RADIO_STOPPING_SILENCED,
    RADIO_STOPPING_CHANNELS_STOPPED,
    RADIO_WAITING_TO_STOP,
    RADIO_STOPPED
};

class CAERadioTrackManager {
public:
    using TrackRange                                   = std::array<int32, 2>; //!< NOTSA: { first, last } track id of a list

    static constexpr auto DJBANTER_INDEX_HISTORY_COUNT = 15;
    static constexpr auto ADVERT_INDEX_HISTORY_COUNT   = 40;
    static constexpr auto IDENT_INDEX_HISTORY_COUNT    = 8;
    static constexpr auto MUSIC_TRACK_HISTORY_COUNT    = 20;
    using DJBanterIndexHistory = tRadioIndexHistory<int32, DJBANTER_INDEX_HISTORY_COUNT>;
    using AdvertIndexHistory   = tRadioIndexHistory<int32, ADVERT_INDEX_HISTORY_COUNT>;
    using IdentIndexHistory    = tRadioIndexHistory<int32, IDENT_INDEX_HISTORY_COUNT>;
    using MusicTrackHistory    = tRadioIndexHistory<int8, MUSIC_TRACK_HISTORY_COUNT>;

    static inline auto& ms_DJBanterIndexHistory        = StaticRef<DJBanterIndexHistory[RADIO_COUNT]>(0xB61D78); // 210
    static inline auto& ms_AdvertIndexHistory          = StaticRef<AdvertIndexHistory[RADIO_COUNT]>(0xB620C0);   // 560
    static inline auto& ms_IdentIndexHistory           = StaticRef<IdentIndexHistory[RADIO_COUNT]>(0xB62980);    // 112
    static inline auto& ms_MusicTrackIndexHistory      = StaticRef<MusicTrackHistory[RADIO_COUNT]>(0xB62B40);    // 280

    static inline auto& ms_StatsLastHitTimeOutHours    = StaticRef<uint8>(0xB62C58); // = -1;
    static inline auto& ms_StatsLastHitGameClockHours  = StaticRef<uint8>(0xB62C59); // = -1;
    static inline auto& ms_StatsLastHitGameClockDays   = StaticRef<uint8>(0xB62C5A); // = -1;
    static inline auto& ms_StatsStartedCrash1          = StaticRef<uint8>(0xB62C5B); // = 0;
    static inline auto& ms_StatsStartedCat2            = StaticRef<uint8>(0xB62C5C); // = 0;
    static inline auto& ms_StatsStartedBadlands        = StaticRef<uint8>(0xB62C5D); // = 0;
    static inline auto& ms_StatsPassedVCrash2          = StaticRef<uint8>(0xB62C5E); // = 0;
    static inline auto& ms_StatsPassedTruth2           = StaticRef<uint8>(0xB62C5F); // = 0;
    static inline auto& ms_StatsPassedSweet2           = StaticRef<uint8>(0xB62C60); // = 0;
    static inline auto& ms_StatsPassedStrap4           = StaticRef<uint8>(0xB62C61); // = 0;
    static inline auto& ms_StatsPassedSCrash1          = StaticRef<uint8>(0xB62C62); // = 0;
    static inline auto& ms_StatsPassedRiot1            = StaticRef<uint8>(0xB62C63); // = 0;
    static inline auto& ms_StatsPassedRyder2           = StaticRef<uint8>(0xB62C64); // = 0;
    static inline auto& ms_StatsPassedMansion2         = StaticRef<uint8>(0xB62C65); // = 0;
    static inline auto& ms_StatsPassedLAFin2           = StaticRef<uint8>(0xB62C66); // = 0;
    static inline auto& ms_StatsPassedFarlie3          = StaticRef<uint8>(0xB62C67); // = 0;
    static inline auto& ms_StatsPassedDesert10         = StaticRef<uint8>(0xB62C68); // = 0;
    static inline auto& ms_StatsPassedDesert8          = StaticRef<uint8>(0xB62C69); // = 0;
    static inline auto& ms_StatsPassedDesert5          = StaticRef<uint8>(0xB62C6A); // = 0;
    static inline auto& ms_StatsPassedDesert3          = StaticRef<uint8>(0xB62C6B); // = 0;
    static inline auto& ms_StatsPassedDesert1          = StaticRef<uint8>(0xB62C6C); // = 0;
    static inline auto& ms_StatsPassedCat1             = StaticRef<uint8>(0xB62C6D); // = 0;
    static inline auto& ms_StatsPassedCasino10         = StaticRef<uint8>(0xB62C6E); // = 0;
    static inline auto& ms_StatsPassedCasino6          = StaticRef<uint8>(0xB62C6F); // = 0;
    static inline auto& ms_StatsPassedCasino3          = StaticRef<uint8>(0xB62C70); // = 0;
    static inline auto& ms_StatsCitiesPassed           = StaticRef<uint8>(0xB62C71); // = 0;
    static inline auto& ms_SpecialDJBanterIndex        = StaticRef<uint8>(0xB62C72); // = -1;
    static inline auto& ms_SpecialDJBanterPending      = StaticRef<uint8>(0xB62C73); // = 3; // ?

public:
    static void InjectHooks();

    CAERadioTrackManager(int32 hwClientHandle);

    CAERadioTrackManager() = default; // NOTSA
    ~CAERadioTrackManager() = default;

    bool Initialise(int32 channelId);
    void InitialiseRadioStationID(eRadioID id);

    void Reset();
    static void ResetStatistics();

    bool   IsRadioOn() const;
    bool   HasRadioRetuneJustStarted() const;
    eRadioID GetCurrentRadioStationID() const;
    int32* GetRadioStationListenTimes();
    void   SetRadioAutoRetuneOnOff(bool enable);
    void   SetBassEnhanceOnOff(bool enable);
    void   SetBassSetting(eBassSetting bassSetting, float bassGrain);
    void   RetuneRadio(eRadioID radioId);

    void  DisplayRadioStationName();
    const GxtChar* GetRadioStationName(eRadioID id);
    void GetRadioStationNameKey(eRadioID id, char(&outStr)[8]);
    static bool IsVehicleRadioActive();

    void StartTrackPlayback();
    void UpdateRadioVolumes();
    void PlayRadioAnnouncement(uint32);
    void StartRadio(eRadioID id, eBassSetting bassSetting, float bassGain, bool skipTrack);
    void StartRadio(const tVehicleAudioSettings& settings);
    void StopRadio(tVehicleAudioSettings* settings, bool duringPause);

    void Service(int32 playTime);

    static void Load();
    static void Save();

protected:
    void AddMusicTrackIndexToHistory(eRadioID id, int8 trackIndex);
    void AddIdentIndexToHistory(eRadioID id, int32 trackIndex);
    void AddAdvertIndexToHistory(eRadioID id, int32 trackIndex);
    void AddDJBanterIndexToHistory(eRadioID id, int32 trackIndex);

    void  ChooseTracksForStation(eRadioID id);
    int32 ChooseIdentIndex(eRadioID id);
    int32 ChooseAdvertIndex(eRadioID id);
    int32 ChooseDJBanterIndex(eRadioID id);
    int32        ChooseDJBanterIndexFromList(eRadioID id, const TrackRange* list);
    int8  ChooseMusicTrackIndex(eRadioID id);
    static int8  ChooseTalkRadioShow();

    void CheckForTrackConcatenation();
    static void CheckForMissionStatsChanges();
    void CheckForStationRetune();
    void CheckForStationRetuneDuringPause();
    void CheckForPause();

    bool QueueUpTracksForStation(eRadioID id, int8* trackCount, eTrackType trackType, tRadioSettings& settings);
    bool TrackRadioStation(eRadioID id, bool skipTrack);

private:
    bool                                 m_IsInitialised{ false };
    bool                                 m_DisplayStationName{ false };
    char                                 m_Prev{ 0 }; // TODO: make sense of this.
    bool                                 m_EnabledInPauseMode{ false };
    bool                                 m_BassEnhance{ true };
    bool                                 m_PauseMode{ false };
    bool                                 m_RetuneJustStarted{ false };
    bool                                 m_RadioAutoSelect{ true };
    std::array<uint8, RADIO_COUNT>       m_TracksInARow{};
    uint8                                m_SavedGameClockDays{ 0xff };
    uint8                                m_SavedGameClockHours{ 0xff };
    std::array<int32, RADIO_COUNT>       m_ListenTimes{}; // Filled from `CStats::FavoriteRadioStationList`
    uint32                               m_TimeRadioStationRetuned{ 0 };
    uint32                               m_TimeToDisplayRadioName{ 0 };
    uint32                               m_SavedTimeMs{ 0 };
    uint32                               m_RetuneStartedTime{};
    uint32                               m_PlayBeginTime{ 0 };
    int32                                m_HwClientHandle{};
    eRadioTrackMode                      m_Mode{ eRadioTrackMode::RADIO_STOPPED };
    int32                                m_StationsListed{ 0 };
    int32                                m_StationsListDown{ 0 };
    int32                                m_SavedRadioStationId{ -1 };       // TODO: convert to eRadioID after finished reversing
    int32                                m_RadioStationMenuRequest{ -1 };   // <-
    int32                                m_RadioStationScriptRequest{ -1 }; // <-
    float                                m_RadioDuckedVolume{};             // Reduces radio volume if mission audio is nearby
    float                                m_RadioDuckFadeRate{};             // Rate of recovery of ducked radio volume
    tRadioSettings                       m_RequestedSettings{};             // settings1
    tRadioSettings                       m_ActiveSettings{};                // settings2
    std::array<tRadioState, RADIO_COUNT> m_RadioStates{};
    uint32                               m_Field368{ 0 };
    eRadioMode                           m_UserTrackPlayMode{};
};
VALIDATE_SIZE(CAERadioTrackManager, 0x370);

extern CAERadioTrackManager& AERadioTrackManager;
