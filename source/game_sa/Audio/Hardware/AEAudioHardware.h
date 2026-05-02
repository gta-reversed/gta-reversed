#pragma once

#include "dsound.h"

#include "Vector.h"

#include "AETrackLoader.h"
#include "AESoundManager.h"
#include "AEStreamThread.h"
#include "AudioEngine.h"
#include "AEMP3BankLoader.h"

union CAEAudioHardwarePlayFlags {
    uint16 m_nFlags{};
    struct {
        // byte 1
        bool m_bIsFrontend : 1;        // 0x1
        bool m_bIsUncompressable : 1;  // 0x2
        bool m_bIsUnduckable : 1;      // 0x4
        bool m_bIsStartPercentage : 1; // 0x8
        bool m_bIsMusicMastered : 1;   // 0x10
        bool : 1;                      // 0x20
        bool m_bIsRolledOff : 1;       // 0x40
        bool m_bIsSmoothDucking : 1;   // 0x80
        // byte 2
        bool m_bIsForcedFront : 1;     // 0x100
        bool m_IsPausable : 1;         // 0x200
    };

    void CopyFromAESound(const CAESound& sound) {
        m_bIsFrontend        = sound.IsFrontEnd();
        m_bIsUncompressable  = sound.IsIncompressible();
        m_bIsUnduckable      = sound.IsUnduckable();
        m_bIsStartPercentage = sound.GetPlayTimeIsPercentage();
        m_bIsMusicMastered   = sound.IsMusicMastered();
        m_bIsRolledOff       = sound.GetRolledOff();
        m_bIsSmoothDucking   = sound.GetSmoothDucking();
        m_bIsForcedFront     = sound.IsForcedFront();
        m_IsPausable         = m_bIsFrontend && sound.IsUnpausable();
    }
};
VALIDATE_SIZE(CAEAudioHardwarePlayFlags, 0x2);

class CAEStreamingChannel;
class CAEMP3TrackLoader;
class CAEMP3BankLoader;
class CAEAudioChannel;
class tBeatInfo;

// NOTSA name
union tAudioChannelFlags {
    uint16 value{};
    struct {
        // byte 1
        bool bIsSecondary : 1;   //< 0x01 -- Channel's volume is rescaled via a seperate group. For ambient, radio and cutscene tracks
        bool bUnduckable : 1;    //< 0x02 -- Sounds can not be "ducked", e.g. ambient sounds get silent while a dialogue plays
        bool bClampVolToNeg : 1; //< 0x04 -- Clamps the volume (-inf, 0]
        bool unk0x8 : 1;         //< 0x08
        bool bIsMusic : 1;       //< 0x10 -- not sure
        bool bIsNotStream : 1;   //< 0x20 -- not sure
        bool bFadeNearEnd : 1;   //< 0x40 -- Fade audio based on how close to finishing, used for one-shot audio e.g. explosions etc?
        bool bSlowFadeout : 1;   //< 0x80
        // byte 2
        /* ... unk ... */
    };
};
VALIDATE_SIZE(tAudioChannelFlags, 0x2);

class CAEAudioHardware {
public:
    bool                    m_bInitialised{};
    bool                    m_bDisableEffectsLoading{};
    bool                    m_IsMaxSecondarySlowFadeout{};
    bool                    m_IsMaxGlobalSlowFadeout{};
    bool                    m_IsHardwareMixAvailable{};
    uint8                   m_nReverbEnvironment{ (uint8)-1};
    tAudioChannelFlags      m_awChannelFlags[MAX_NUM_AUDIO_CHANNELS]{};
    uint16                  field_86{};
    int32                   m_nReverbDepth{ -10000 };
    uint16                  m_nNumAvailableChannels{};
    uint16                  m_nNumChannels{};
    uint16                  m_anNumChannelsInSlot[MAX_NUM_AUDIO_CHANNELS]{};
    float                   m_afChannelVolumes[MAX_NUM_AUDIO_CHANNELS]{}; // -1000.f
    float                   m_afChannelVolumeLinear[MAX_NUM_AUDIO_CHANNELS]{};
    float                   m_afChannelsFrqScalingFactor[MAX_NUM_AUDIO_CHANNELS]{};

    float                   m_fMusicMasterScalingFactor{ 1.f };
    float                   m_fEffectMasterScalingFactor{ 1.f };

    float                   m_fMusicFaderScalingFactor{ 1.f };
    float                   m_fEffectsFaderScalingFactor{ 1.f };

    float                   m_fNonStreamFaderScalingFactor{ 1.f };
    float                   m_fStreamFaderScalingFactor{ 1.f };

    float                   m_PrevMaxVolumeSecondary{};
    float                   m_PrevMaxVolumeGlobal{};
    tVirtualChannelSettings m_VirtualChannelSettings{};
    //union { // TODO: Get rid of the union, and use `m_VirtualChannelSettings` directly
    //    struct {
    //        int16 m_aBankSlotIds[MAX_NUM_SOUNDS];
    //        int16 m_aSoundIdsInSlots[MAX_NUM_SOUNDS];
    //    };
    //};
    int16                   m_VirtualChannelLoopTimes[MAX_NUM_SOUNDS]{};
    int16                   m_VirtualChannelSoundLengths[MAX_NUM_SOUNDS]{};

    eBassSetting            m_BassSetting{};
    float                   m_BassGain{};
    CAEMP3BankLoader*       m_pMP3BankLoader{};
    CAEMP3TrackLoader*      m_pMP3TrackLoader{};
    IDirectSound8*          m_pDSDevice{};
    uint32                  m_nSpeakerConfig{};
    int32                   m_n3dEffectsQueryResult{}; // TODO: 1 - EAX available
    DSCAPS                  m_dsCaps{};
    IDirectSound3DListener* m_pDirectSound3dListener{};
    CAEStreamingChannel*    m_pStreamingChannel{};
    CAEStreamThread         m_pStreamThread{};
    CAEAudioChannel*        m_aChannels[MAX_NUM_AUDIO_CHANNELS]{};
    tBeatInfo               gBeatInfo{};
    uint8                   m_PlayingTrackFlags{};

public:
    static void InjectHooks();

    // Return types aren't real, I've just copied the signatures for now

    CAEAudioHardware();
    ~CAEAudioHardware() = default;

    bool Initialise();
    bool InitDirectSoundListener(uint32 numChannels, uint32 samplesPerSec, uint32 bitsPerSample);
    void Terminate();

    int16 AllocateChannels(uint16 numChannels);

    void PlaySound(int16 channel, uint16 channelSlot, eSoundID soundIdInSlot, eSoundBankSlot bankSlot, int16 playPosition, CAEAudioHardwarePlayFlags flags, float speed);
    uint16 GetNumAvailableChannels() const;
    void GetChannelPlayTimes(int16 channel, int16* playTimes);
    void SetChannelVolume(int16 channel, uint16 channelId, float volume, uint8 unused);

    void LoadSoundBank(eSoundBank bank, eSoundBankSlot slot);
    bool IsSoundBankLoaded(eSoundBank bank, eSoundBankSlot slot);
    int8 GetSoundBankLoadingStatus(eSoundBank bank, eSoundBankSlot slot);
    bool EnsureSoundBankIsLoaded(eSoundBank bank, eSoundBankSlot slot, bool checkLoadingTune = false, bool cancelSoundsInSlot = false);

    void LoadSound(eSoundBank bank, eSoundID sfx, eSoundBankSlot slot);
    bool IsSoundLoaded(eSoundBank bank, eSoundID sfx, eSoundBankSlot slot);
    bool GetSoundLoadingStatus(eSoundBank bank, eSoundID sfx, eSoundBankSlot slot);

    void StopSound(int16 channel, uint16 channelSlot) const;
    void SetChannelPosition(int16 slotId, uint16 channelSlot, const CVector& vecPos, uint8 unused) const;
    void SetChannelFrequencyScalingFactor(int16 channel, uint16 channelSlot, float freqFactor);
    void RescaleChannelVolumes();
    void UpdateReverbEnvironment();
    float GetSoundHeadroom(eSoundID sfx, eSoundBankSlot slot);

    void EnableEffectsLoading();
    void DisableEffectsLoading();

    void RequestVirtualChannelSoundInfo(uint16 vch, eSoundID sfx, eSoundBankSlot slot);
    void GetVirtualChannelSoundLengths(int16* outArr) const;
    void GetVirtualChannelSoundLoopStartTimes(int16* outArr) const;

    void PlayTrack(uint32 trackID, int nextTrackID, uint32 startOffsetMs, uint8 trackFlags, bool bUserTrack, bool bUserNextTrack);
    void StartTrackPlayback() const;
    void StopTrack();

    int32 GetTrackPlayTime() const;
    int32 GetTrackLengthMs() const;
    int32 GetActiveTrackID() const;
    int32 GetPlayingTrackID() const;
    void GetBeatInfo(tBeatInfo* beatInfo);
    void GetActualNumberOfHardwareChannels();

    void SetBassSetting(eBassSetting bassSetting, float bassGain);
    void EnableBassEq();
    void DisableBassEq();

    void SetChannelFlags(int16 channel, uint16 channelId, tAudioChannelFlags flags);

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

    // notsa
    const CAEBankSlot& GetBankSlot(eSoundBankSlot slot) const;
    CAEStaticChannel* GetStaticChannel(int16 channel, uint16 channelId) {
        assert(channel >= 0 && channelId < m_anNumChannelsInSlot[channel]);
        assert(channel + channelId != 0); // channel #0 is a streaming channel!
        return reinterpret_cast<CAEStaticChannel*>(m_aChannels[channel + channelId]);
    }

private:
    auto GetChannels() const { return std::span{m_aChannels, m_nNumChannels}; }

private:
    // 0x4D83E0
    CAEAudioHardware* Constructor() {
        this->CAEAudioHardware::CAEAudioHardware();
        return this;
    }

    // 0x4D83A0
    CAEAudioHardware* Destructor() {
        this->CAEAudioHardware::~CAEAudioHardware();
        return this;
    }

};
VALIDATE_SIZE(CAEAudioHardware, 0x1014); // Size might be bigger, but nothing is accessed beyond `0x1014`

extern CAEAudioHardware& AEAudioHardware;
