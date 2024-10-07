#pragma once

#include "dsound.h"

#include "Vector.h"

#define USE_DSOUND

#pragma pack(push, 1)
class NOTSA_EXPORT_VTABLE CAEAudioChannel {
public:
#ifdef USE_DSOUND
    IDirectSound*         m_pDirectSound;
    union {
        IDirectSoundBuffer*  m_pDirectSoundBuffer;
        IDirectSoundBuffer8* m_pDirectSoundBuffer8;
    };
    IDirectSound3DBuffer* m_pDirectSound3DBuffer;
#endif

    char                  _pad10[24];
    uint32                m_nFlags;
    uint32                m_nLengthInBytes;
    uint32                field_30; // unused
    float                 m_fVolume;
    bool                  m_bNoScalingFactor;
    uint8                 field_39; // unused
    uint16                m_nChannelId;
    uint32                m_nFrequency;
    uint32                m_nOriginalFrequency;
    bool                  m_bLooped;
    uint8                 field_45;
    uint8                 field_46; // unused
    WAVEFORMATEX          m_WaveFormat;
    uint16                field_59;
#ifdef USE_DSOUND
    char                  _pad;
    union {
        struct {
            bool Bit0x1 : 1;
        } bufferStatus;
        uint32 m_nBufferStatus;
    };
#endif

public:
    CAEAudioChannel(IDirectSound* directSound, uint16 channelId, uint32 samplesPerSec, uint16 bitsPerSample);
    virtual ~CAEAudioChannel();

    // VTABLE
    virtual void   Service() = 0;
    virtual bool   IsSoundPlaying() = 0;
    virtual int16  GetPlayTime() = 0;
    virtual uint16 GetLength() = 0;
    virtual void   Play(int16, int8, float) = 0;
    virtual void   SynchPlayback() = 0;
    virtual void   Stop() = 0;
    virtual void   SetFrequencyScalingFactor(float factor);

    void   SetPosition(const CVector& vecPos) const;
    float  GetVolume() const { return m_fVolume; };
    void   SetVolume(float volume);
    bool   IsBufferPlaying() const { return m_nBufferStatus & DSBSTATUS_PLAYING; };
    uint32 GetCurrentPlaybackPosition() const;
    uint32 ConvertFromBytesToMS(uint32 bytes) const;
    uint32 ConvertFromMsToBytes(uint32 ms) const;
    void   SetFrequency(uint32 freq);
    void   SetOriginalFrequency(uint32 freq);
    void   UpdateStatus();

    // Methods not in android IDB
    bool Lost() const;

    // Those 2 require DirectSound EAX 4.0 extensions or some alternative to be available in project
    bool SetReverbAndDepth(uint32 reverb, uint32 depth);
    void SetNotInRoom(uint8 type); // 0 - frontend, 1 - world

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
#pragma pack(pop)
VALIDATE_SIZE(CAEAudioChannel, 0x60);
VALIDATE_OFFSET(CAEAudioChannel, m_pDirectSound, 0x4);
VALIDATE_OFFSET(CAEAudioChannel, m_nChannelId, 0x3A);
//VALIDATE_OFFSET(CAEAudioChannel, m_nBufferFrequency, 0x4B);
//VALIDATE_OFFSET(CAEAudioChannel, m_wFrequencyMult, 0x49);

extern uint32& g_numSoundChannelsUsed;
