#pragma once

#include "dsound.h"

#include "CVector.h"

#pragma pack(push, 1)
class CAEAudioChannel {
public:
    IDirectSound*         m_pDirectSound;
    IDirectSoundBuffer*   m_pDirectSoundBuffer;
    IDirectSound3DBuffer* m_pDirectSound3DBuffer;
    char                  _pad10[24];
    uint32                m_nFlags;
    uint32                m_nLengthInBytes;
    uint32                field_30;
    float                 m_fVolume;
    bool                  m_bNoScalingFactor;
    uint8                 field_39;
    uint16                m_nChannelId;
    uint32                m_nFrequency;
    uint32                m_nOriginalFrequency;
    bool                  m_bLooped;
    uint8                 field_45;
    uint8                 field_46[1];
    uint16                field_47;
    uint16                m_wFrequencyMult;
    uint32                m_nBufferFrequency;
    uint32                m_nBytesPerSec;
    uint16                field_53;
    uint16                m_wBitsPerSample;
    uint16                field_57;
    uint16                field_59;
    char                  _pad;
    uint32                m_nBufferStatus;

public:
    CAEAudioChannel(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample);
    virtual ~CAEAudioChannel();

    // VTABLE
    virtual void   Service() = 0;
    virtual bool   IsSoundPlaying() = 0;
    virtual uint16 GetPlayTime() = 0;
    virtual uint16 GetLength() = 0;
    virtual void   Play(int16, int8, float) = 0;
    virtual void   SynchPlayback() = 0;
    virtual void   Stop() = 0;
    virtual void   SetFrequencyScalingFactor(float factor);

    void   SetPosition(CVector* vecPos);
    float  GetVolume() const { return m_fVolume; };
    void   SetVolume(float volume);
    bool   IsBufferPlaying() const { return m_nBufferStatus & DSBSTATUS_PLAYING; };
    int32  GetCurrentPlaybackPosition();
    uint32 ConvertFromBytesToMS(uint32 bytes);
    uint32 ConvertFromMsToBytes(uint32 ms);
    void   SetFrequency(uint32 freq);
    void   SetOriginalFrequency(uint32 freq);
    void   UpdateStatus();

    // Methods not in android IDB
    bool Lost();

    // Those 2 require DirectSound EAX 4.0 extensions or some alternative to be available in project
    bool SetReverbAndDepth(uint32 reverb, uint32 depth);
    void SetNotInRoom(uint8 type); // 0 - frontend, 1 - world

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    void SetFrequencyScalingFactor_Reversed(float factor);
};
#pragma pack(pop)
VALIDATE_SIZE(CAEAudioChannel, 0x60);

extern uint32& g_numSoundChannelsUsed;
