#pragma once

class CAECutsceneTrackManager {
public:
    enum Status : int32 {
        S0 = 0,
        S1 = 1,
        S2 = 2,
        STATE_PLAYING = 3,
        STATE_STOP_REQUESTED = 4,
        S5 = 5,
        S6 = 6,
        S7 = 7,
        S8 = 8,
        S9 = 9,
        S10 = 10,
    };

    bool   m_bPlayRequest;
    bool   m_bPaused;
    int32  m_nAudioChannel;
    int32  m_nStatus;
    uint32 m_nTrackId;
    int32  _f10;
    uint32 _f14;
    int32  _f18;
    int8   m_bIsUserTrack;
    char   _pad[4];
    uint32 m_nPlayingTrackId;
    int32  m_nTrackPlayTime;
    int32  m_nTrackLengthMs;

public:
    static void InjectHooks();

    ~CAECutsceneTrackManager() = default;

    bool Initialise(int32 nAudioChannel);

    void PlayPreloadedCutsceneTrack();
    void StopCutsceneTrack();

    bool IsCutsceneTrackActive() const;
    uint8 GetCutsceneTrackStatus() const;

    void PauseTrack(bool pause);
    void Service(int32 trackPlayTime);
    void CheckForPause() const;
    void PreloadBeatTrack(int16 trackId, bool wait);
    void PreloadCutsceneTrack(int16 trackId, bool wait);
    void StartTrackPlayback() const;
};

VALIDATE_SIZE(CAECutsceneTrackManager, 0x30);

extern CAECutsceneTrackManager& AECutsceneTrackManager;
