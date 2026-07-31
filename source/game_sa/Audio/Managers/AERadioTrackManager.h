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

    void Reset() {
        *this = {};
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
    std::array<int32, 3> m_aElapsed{0};
    int32 m_iTimeInPauseModeInMs{-1};
    int32 m_iTimeInMs{-1};
    int32 m_iTrackPlayTime{-1};
    std::array<int32, 3> m_aTrackQueue{-1};
    std::array<eTrackType, 3>  m_aTrackTypes{eTrackType::NONE};
    int8 m_nGameClockDays{-1};
    int8 m_nGameClockHours{-1};

    void Reset(bool paused = false) {
        rng::fill(m_aElapsed, 0);
        rng::fill(m_aTrackQueue, -1);
        rng::fill(m_aTrackTypes, eTrackType::NONE);

        m_iTimeInMs = -1;
        if (!paused)
            m_iTimeInPauseModeInMs = -1;
        m_iTrackPlayTime = -1;
        m_nGameClockDays = -1;
        m_nGameClockHours = -1;
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
    bool            m_bInitialised{false};
    bool            m_bDisplayStationName{false};
    char            m_prev{0}; // TODO: make sense of this.
    bool            m_bEnabledInPauseMode{false};
    bool            m_bBassEnhance{true};
    bool            m_bPauseMode{false};
    bool            m_bRetuneJustStarted{false};
    bool            m_bRadioAutoSelect{true};
    std::array<uint8, RADIO_COUNT>       m_nTracksInARow{};
    uint8           m_nSavedGameClockDays{0xff};
    uint8           m_nSavedGameClockHours{0xff};
    std::array<int32, RADIO_COUNT>       m_aListenTimes{}; // Filled from `CStats::FavoriteRadioStationList`
    uint32          m_nTimeRadioStationRetuned{0};
    uint32          m_nTimeToDisplayRadioName{0};
    uint32          m_nSavedTimeMs{0};
    uint32          m_nRetuneStartedTime;
    uint32          m_PlayBeginTime{0};
    int32           m_HwClientHandle;
    eRadioTrackMode m_nMode{eRadioTrackMode::RADIO_STOPPED};
    int32           m_nStationsListed{0};
    int32           m_nStationsListDown{0};
    int32           m_nSavedRadioStationId{-1};         // TODO: convert to eRadioID after finished reversing
    int32           m_iRadioStationMenuRequest{ -1 };   // <-
    int32           m_iRadioStationScriptRequest{ -1 }; // <-
    float           m_RadioDuckedVolume{}; // Reduces radio volume if mission audio is nearby
    float           m_RadioDuckFadeRate{}; // Rate of recovery of ducked radio volume
    tRadioSettings  m_RequestedSettings{}; // settings1
    tRadioSettings  m_ActiveSettings{}; // settings2
    std::array<tRadioState, RADIO_COUNT> m_aRadioState{};
    uint32          field_368{0};
    eRadioMode      m_nUserTrackPlayMode{};

public:
    static constexpr auto DJBANTER_INDEX_HISTORY_COUNT = 15;
    static constexpr auto ADVERT_INDEX_HISTORY_COUNT   = 40;
    static constexpr auto IDENT_INDEX_HISTORY_COUNT    = 8;
    static constexpr auto MUSIC_TRACK_HISTORY_COUNT    = 20;
    using DJBanterIndexHistory = tRadioIndexHistory<int32, DJBANTER_INDEX_HISTORY_COUNT>;
    using AdvertIndexHistory   = tRadioIndexHistory<int32, ADVERT_INDEX_HISTORY_COUNT>;
    using IdentIndexHistory    = tRadioIndexHistory<int32, IDENT_INDEX_HISTORY_COUNT>;
    using MusicTrackHistory    = tRadioIndexHistory<int8, MUSIC_TRACK_HISTORY_COUNT>;

    static inline auto& m_nDJBanterIndexHistory = StaticRef<DJBanterIndexHistory[RADIO_COUNT]>(0xB61D78); // 210
    static inline auto& m_nAdvertIndexHistory = StaticRef<AdvertIndexHistory[RADIO_COUNT]>(0xB620C0);       // 560
    static inline auto& m_nIdentIndexHistory = StaticRef<IdentIndexHistory[RADIO_COUNT]>(0xB62980);          // 112
    static inline auto& m_nMusicTrackIndexHistory = StaticRef<MusicTrackHistory[RADIO_COUNT]>(0xB62B40);   // 280

    static inline auto& m_nStatsLastHitTimeOutHours = StaticRef<uint8>(0xB62C58); // = -1;
    static inline auto& m_nStatsLastHitGameClockHours = StaticRef<uint8>(0xB62C59); // = -1;
    static inline auto& m_nStatsLastHitGameClockDays = StaticRef<uint8>(0xB62C5A); // = -1;
    static inline auto& m_nStatsStartedCrash1 = StaticRef<uint8>(0xB62C5B); // = 0;
    static inline auto& m_nStatsStartedCat2 = StaticRef<uint8>(0xB62C5C); // = 0;
    static inline auto& m_nStatsStartedBadlands = StaticRef<uint8>(0xB62C5D); // = 0;
    static inline auto& m_nStatsPassedVCrash2 = StaticRef<uint8>(0xB62C5E); // = 0;
    static inline auto& m_nStatsPassedTruth2 = StaticRef<uint8>(0xB62C5F); // = 0;
    static inline auto& m_nStatsPassedSweet2 = StaticRef<uint8>(0xB62C60); // = 0;
    static inline auto& m_nStatsPassedStrap4 = StaticRef<uint8>(0xB62C61); // = 0;
    static inline auto& m_nStatsPassedSCrash1 = StaticRef<uint8>(0xB62C62); // = 0;
    static inline auto& m_nStatsPassedRiot1 = StaticRef<uint8>(0xB62C63); // = 0;
    static inline auto& m_nStatsPassedRyder2 = StaticRef<uint8>(0xB62C64); // = 0;
    static inline auto& m_nStatsPassedMansion2 = StaticRef<uint8>(0xB62C65); // = 0;
    static inline auto& m_nStatsPassedLAFin2 = StaticRef<uint8>(0xB62C66); // = 0;
    static inline auto& m_nStatsPassedFarlie3 = StaticRef<uint8>(0xB62C67); // = 0;
    static inline auto& m_nStatsPassedDesert10 = StaticRef<uint8>(0xB62C68); // = 0;
    static inline auto& m_nStatsPassedDesert8 = StaticRef<uint8>(0xB62C69); // = 0;
    static inline auto& m_nStatsPassedDesert5 = StaticRef<uint8>(0xB62C6A); // = 0;
    static inline auto& m_nStatsPassedDesert3 = StaticRef<uint8>(0xB62C6B); // = 0;
    static inline auto& m_nStatsPassedDesert1 = StaticRef<uint8>(0xB62C6C); // = 0;
    static inline auto& m_nStatsPassedCat1 = StaticRef<uint8>(0xB62C6D); // = 0;
    static inline auto& m_nStatsPassedCasino10 = StaticRef<uint8>(0xB62C6E); // = 0;
    static inline auto& m_nStatsPassedCasino6 = StaticRef<uint8>(0xB62C6F); // = 0;
    static inline auto& m_nStatsPassedCasino3 = StaticRef<uint8>(0xB62C70); // = 0;
    static inline auto& m_nStatsCitiesPassed = StaticRef<uint8>(0xB62C71); // = 0;
    static inline auto& m_nSpecialDJBanterIndex = StaticRef<uint8>(0xB62C72); // = -1;
    static inline auto& m_nSpecialDJBanterPending = StaticRef<uint8>(0xB62C73); // = 3; // ?

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
    void StopRadio(tVehicleAudioSettings* settings, bool bDuringPause);

    void Service(int32 playTime);

    static void Load();
    static void Save();

protected:
    void AddMusicTrackIndexToHistory(eRadioID id, int8 trackIndex);
    void AddIdentIndexToHistory(eRadioID id, int8 trackIndex);
    void AddAdvertIndexToHistory(eRadioID id, int8 trackIndex);
    void AddDJBanterIndexToHistory(eRadioID id, int8 trackIndex);

    void  ChooseTracksForStation(eRadioID id);
    int32 ChooseIdentIndex(eRadioID id);
    int32 ChooseAdvertIndex(eRadioID id);
    int32 ChooseDJBanterIndex(eRadioID id);
    int32 ChooseDJBanterIndexFromList(eRadioID id, int32** list);
    int8  ChooseMusicTrackIndex(eRadioID id);
    static int8  ChooseTalkRadioShow();

    void CheckForTrackConcatenation();
    static void CheckForMissionStatsChanges();
    void CheckForStationRetune();
    void CheckForStationRetuneDuringPause();
    void CheckForPause();

    bool QueueUpTracksForStation(eRadioID id, int8* iTrackCount, eTrackType trackType, tRadioSettings& settings);
    bool TrackRadioStation(eRadioID id, bool skipTrack);
};
VALIDATE_SIZE(CAERadioTrackManager, 0x370);

extern CAERadioTrackManager& AERadioTrackManager;
