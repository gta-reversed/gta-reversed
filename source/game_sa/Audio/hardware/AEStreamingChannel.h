#pragma once

#include "AEAudioChannel.h"
#include "AEStreamingDecoder.h"
#include "AESmoothFadeThread.h"

enum class StreamingChannelState : int32 {
    UNK_MINUS_7 = -7,
    UNK_MINUS_6 = -6,
    UNK_MINUS_5 = -5,
    UNK_MINUS_4 = -4,
    UNK_MINUS_3 = -3,
    UNK_MINUS_2 = -2,
    UNK_MINUS_1 = -1,
};

class NOTSA_EXPORT_VTABLE CAEStreamingChannel : public CAEAudioChannel {
public:
    bool                  m_bInitialized{false};
    uint8                 field_61{0u};
    bool                  m_bPrepareNewStream{false};
    uint8                 m_bWrongSampleRate{false};
    uint8                 field_64{0u};
    uint8                 m_bFxEnabled{false};
    uint8                 m_nCurrentlyLoadedChunk{0u};
    uint8                 field_67{0u};
    void*                 m_pBuffer{nullptr};
    uint8                 m_aBuffer[0x60000];
    CAEStreamingDecoder*  m_pStreamingDecoder{nullptr};
    CAEStreamingDecoder*  m_pNextStreamingDecoder{nullptr};
    StreamingChannelState m_nState{StreamingChannelState::UNK_MINUS_6};
    uint32                m_nStreamPlayTimeMs{0u};
    uint32                m_nPlayTime;
    uint32                field_60084;
    int32                 field_60088{0u};
    uint64                m_nLastUpdateTime;
    IDirectSoundBuffer*   m_pSilenceBuffer;
    float                 m_fSomething{1.0f};

public:
    CAEStreamingChannel(IDirectSound* directSound, uint16 channelId)
        : CAEAudioChannel(directSound, channelId, 48000, 16)
    {} // 0x4F1800
    ~CAEStreamingChannel() override;

    void   Service() override;
    bool   IsSoundPlaying() override;
    int16  GetPlayTime() override;
    uint16 GetLength() override;
    void   Play(int16, int8, float) override;
    void   SynchPlayback() override;
    void   Stop() override;
    void   SetFrequencyScalingFactor(float factor) override;

    void  Initialise();
    void  InitialiseSilence();

    bool AddFX();
    void RemoveFX();

    int32 UpdatePlayTime();

    int32  GetActiveTrackID();
    int32  GetPlayingTrackID();
    void   SetNextStream(CAEStreamingDecoder* decoder);
    uint32 FillBuffer(void* buffer, uint32 size);
    void   SetBassEQ(uint8 mode, float gain);
    void   SetReady();
    void   PrepareStream(CAEStreamingDecoder* stream, int8 arg2, uint32 audioBytes);
    void   Pause();

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CAEStreamingChannel* Constructor(IDirectSound* directSound, uint16 channelId) {
        this->CAEStreamingChannel::CAEStreamingChannel(directSound, channelId);
        return this;
    }

    void Destructor() {
        CAEStreamingChannel::~CAEStreamingChannel();
    }

                                // NOTSA
    void DirectSoundBufferFadeToSilence() {
        if (!AESmoothFadeThread.RequestFade(m_pDirectSoundBuffer, -100.0, 35, true))
            m_pDirectSoundBuffer->Stop();
    }
};

VALIDATE_SIZE(CAEStreamingChannel, 0x60098);
