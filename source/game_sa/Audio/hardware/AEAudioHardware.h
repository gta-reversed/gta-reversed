#pragma once

#include "dsound.h"

#include "Vector.h"

#include "AESoundManager.h"
#include "AEStreamThread.h"

#ifdef PlaySound
#undef PlaySound
#endif

union CAEAudioHardwarePlayFlags {
    uint16 m_nFlags;
    struct {
        uint16 m_bIsFrontend : 1;
        uint16 m_bIsUncompressable : 1;
        uint16 m_bIsUnduckable : 1;
        uint16 m_bIsStartPercentage : 1;
        uint16 m_bIsMusicMastered : 1;
        uint16 : 1;
        uint16 m_bIsRolledOff : 1;
        uint16 m_bIsSmoothDucking : 1;

        uint16 m_bIsForcedFront : 1;
        uint16 m_bUnpausable : 1;
    };
};

class CAEStreamingChannel;
class CAEMP3TrackLoader;
class CAEMP3BankLoader;
class CAEAudioChannel;
class tBeatInfo;

#define USE_DSOUND

class CAEAudioHardware {
public:
    bool                    m_bInitialised; // maybe
    bool                    m_bDisableEffectsLoading;
    uint8                   m_prev;
    uint8                   field_3;
    uint8                   field_4;
    uint8                   m_nReverbEnvironment;
    int16                   m_awChannelFlags[MAX_NUM_AUDIO_CHANNELS];
    uint16                  field_86;
    int32                   m_nReverbDepth;
    uint16                  m_nNumAvailableChannels;
    int16                   m_nNumChannels;
    uint16                  m_anNumChannelsInSlot[MAX_NUM_AUDIO_CHANNELS];
    float                   m_afChannelVolumes[MAX_NUM_AUDIO_CHANNELS];
    float                   m_afUnkn[MAX_NUM_AUDIO_CHANNELS];
    float                   m_afChannelsFrqScalingFactor[MAX_NUM_AUDIO_CHANNELS];
    float                   m_fMusicMasterScalingFactor;
    float                   m_fEffectMasterScalingFactor;
    float                   m_fMusicFaderScalingFactor;
    float                   m_fEffectsFaderScalingFactor;
    float                   m_fNonStreamFaderScalingFactor;
    float                   m_fStreamFaderScalingFactor;
    float                   field_428;
    float                   field_42C;
    int16                   m_aBankSlotIds[MAX_NUM_SOUNDS];
    int16                   m_aSoundIdsInSlots[MAX_NUM_SOUNDS];
    int16                   m_anVirtualChannelSoundLoopStartTimes[MAX_NUM_SOUNDS];
    int16                   m_anVirtualChannelSoundLengths[MAX_NUM_SOUNDS];
    uint32                  m_nBassSet;
    float                   m_fBassEqGain;
    CAEMP3BankLoader*       m_pMP3BankLoader;
    CAEMP3TrackLoader*      m_pMP3TrackLoader;
    IDirectSound8*          m_pDevice;
    void*                   m_dwSpeakerConfig;
    void*                   m_n3dEffectsQueryResult;
    DSCAPS                  m_dsCaps;
    IDirectSound3DListener* m_pDirectSound3dListener;
    CAEStreamingChannel*    m_pStreamingChannel;
    CAEStreamThread         m_pStreamThread;
    CAEAudioChannel*        m_aChannels[MAX_NUM_AUDIO_CHANNELS];
    uint32                  m_aBeatInfo[40];
    int32                   field_1004;
    int32                   field_1008;
    int32                   field_100C;
    uint8                   field_1010;
    int32                   field_1014;

public:
    static void InjectHooks();

    // Return types aren't real, I've just copied the signatures for now

    CAEAudioHardware();
    ~CAEAudioHardware();

    bool Initialise();
    void InitOpenALListener();
    bool InitDirectSoundListener(uint32 numChannels, uint32 samplesPerSec, uint32 bitsPerSample);
    void Terminate();

    int16 AllocateChannels(uint16 numChannels);

    void PlaySound(int16 channel, uint16 channelSlot, uint16 soundIdInSlot, uint16 bankSlot, int16 playPosition, int16 flags, float speed);
    uint16 GetNumAvailableChannels() const;
    void GetChannelPlayTimes(int16 channel, int16* playTimes);
    void SetChannelVolume(int16 channel, uint16 channelId, float volume, uint8 unused);

    bool LoadSoundBank(uint16 bankId, int16 bankSlotId);
    bool IsSoundBankLoaded(uint16 bankId, int16 bankSlotId);
    int8 GetSoundBankLoadingStatus(uint16 bankId, int16 bankSlotId);

    bool LoadSound(uint16 bank, uint16 sound, int16 slot);
    bool IsSoundLoaded(uint16, uint16, int16);
    bool GetSoundLoadingStatus(uint16 bankId, uint16 sfxId, int16 bankSlot);

    void StopSound(int16 channel, uint16 channelSlot);
    void SetChannelPosition(int16 channel, uint16 channelSlot, CVector* vecPos, uint8 unused);
    void SetChannelFrequencyScalingFactor(int16 channel, uint16 channelSlot, float freqFactor);
    void RescaleChannelVolumes();
    void UpdateReverbEnvironment();
    float GetSoundHeadroom(uint16 soundId, int16 bankSlotId);

    void EnableEffectsLoading();
    void DisableEffectsLoading();

    void RequestVirtualChannelSoundInfo(uint16 soundIndex, uint16 soundIdInSlot, uint16 bankSlotId);
    void GetVirtualChannelSoundLengths(int16* outArr);
    void GetVirtualChannelSoundLoopStartTimes(int16* outArr);

    void PlayTrack(uint32 trackId, int32 nextTrackId, uint32 a3, char a4, bool isUserTrack, bool nextIsUserTrack);
    void StartTrackPlayback() const;
    void StopTrack();

    int32 GetTrackPlayTime() const;
    int32 GetTrackLengthMs() const;
    int32 GetActiveTrackID() const;
    int32 GetPlayingTrackID() const;
    void GetBeatInfo(tBeatInfo* beatInfo);
    void GetActualNumberOfHardwareChannels();

    void SetBassSetting(int8 nBassSet, float fBassEqGain);
    void EnableBassEq();
    void DisableBassEq();

    void SetChannelFlags(int16 channel, uint16 channelId, int16 flags);

    void SetMusicMasterScalingFactor(float factor);
    float GetMusicMasterScalingFactor() const;

    void SetEffectsMasterScalingFactor(float factor);
    float GetEffectsMasterScalingFactor() const;

    void SetMusicFaderScalingFactor(float factor);

    void SetEffectsFaderScalingFactor(float factor);
    float GetEffectsFaderScalingFactor() const;

    void SetStreamFaderScalingFactor(float factor);
    void SetNonStreamFaderScalingFactor(float factor);

    bool IsStreamingFromDVD();
    char GetDVDDriveLetter();
    bool CheckDVD();

    void PauseAllSounds();
    void ResumeAllSounds();

    void Query3DSoundEffects();

    void Service();
};

VALIDATE_SIZE(CAEAudioHardware, 0x1018);

extern CAEAudioHardware& AEAudioHardware;
