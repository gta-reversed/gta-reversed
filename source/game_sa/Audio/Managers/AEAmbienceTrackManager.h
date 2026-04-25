#pragma once

class CAEAmbienceTrackManager {
    struct tAmbienceSettings {
        int32 TrackID;
        int32 PlayingTrackID;
        uint8 TrackFlags;
    };
public:
    enum AmbienceStatus : int32 {
        STARTING,
        WAITING_TO_PLAY,
        READY_TO_PLAY,
        PLAYING,
        STOPPING,
        STOPPING_SILENCED,
        STOPPING_CHANNELS_STOPPED,
        WAITING_TO_STOP,
        STOPPED,
    };

    bool              m_OverrideRadio;
    bool              m_LastAmbienceOverrodeRadio;
    bool              m_StartAmbienceAtBeginning;
    eRadioID          m_AmbienceRadioStation;
    int32             m_HwClientHandle; // AKA channel
    AmbienceStatus    m_AmbienceStatus;
    int32             m_SpecialMissionAmbienceTrackID;
    int32             m_PrevAmbiencePlayTimeMs;
    int32             m_PrevAmbienceStopTimeMs;
    float             m_Volume;
    float             m_FreqFactor;
    tAmbienceSettings m_RequestedSettings;

    /*tAmbienceSettings m_ActiveSettings;*/

public:
    static void InjectHooks();

    bool Initialise(int32 channelId);
    void Reset();

    void CheckForPause() const;

    void StopAmbienceTrack();
    [[nodiscard]] bool IsAmbienceRadioActive() const;
    void UpdateAmbienceTrackAndVolume();

    [[nodiscard]] bool IsAmbienceTrackActive() const;

    void PlaySpecialMissionAmbienceTrack(eAudioEvents event);
    void StopSpecialMissionAmbienceTrack();

    void Service(int32 trackPlayTime);
    void StartTrackPlayback() const;
private:
    void ChangeStatusToStopped();

};

VALIDATE_SIZE(CAEAmbienceTrackManager, 0x2C);

extern CAEAmbienceTrackManager& AEAmbienceTrackManager;
