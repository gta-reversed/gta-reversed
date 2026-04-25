#pragma once
#include "AEAudioChannel.h"

class NOTSA_EXPORT_VTABLE CAEStaticChannel : public CAEAudioChannel {
public:
    bool   m_OverwriteIntroWhenWrapped; //< originally m_bNeedData
    bool   m_bUnkn2;
    bool   m_bNeedsSynch;
    bool   m_bUnkn4;
    int32  m_LoopStartOffset; //< offset to the part that looped section starts
    uint32 m_LoopEndOffset;
    int32  m_TotalLoops; //< number of loops to fill the buffer
    uint32 m_nSyncTime;
    int32  m_CurrentBufferOffsetMs;
    int32  m_IntroOverwriteOffset;
    int32  m_LoopedBytes;
    uint16 m_NumLoopsCoverIntro; //< number of loops that fill one intro section. refer `CAEStaticChannel::SetAudioBuffer`
    uint16 field_82;
    void*  m_pBuffer;
    uint16 field_88;
    bool   m_IsHardwareMixAvailable;
    char   field_8B;
    int16  field_8C;

public:
    CAEStaticChannel(IDirectSound* pDirectSound, uint16 channelId, bool arg3, uint32 samplesPerSec, uint16 bitsPerSample);

    void   Service() override;
    bool   IsSoundPlaying() override;
    int16  GetPlayTime() override;
    uint16 GetLength() override;
    void   Play(int16 timeInMs, int8 unused, float scalingFactor) override;
    void   SynchPlayback() override;
    void   Stop() override;

    bool SetAudioBuffer(void* buffer, uint16 size, int16 f88, int16 f8c, int16 loopOffset, uint16 frequency);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CAEStaticChannel, 0x90);
