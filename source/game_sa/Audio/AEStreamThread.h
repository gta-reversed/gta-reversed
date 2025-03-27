#pragma once

#include "AEStreamingChannel.h"

class CAEMP3TrackLoader;

class CAEStreamThread {
public:
    HANDLE m_nHandle;
    LPDWORD               m_lpThreadId;
    CAEStreamingChannel*  m_pStreamingChannel;
    CAEMP3TrackLoader*    m_pMp3TrackLoader;
    bool                  m_bActive;
    bool                  m_bThreadActive;
    char                  m_bNeedsService;
    bool                  m_bIsUserTrack;
    bool                  m_bNextIsUserTrack;
    char                  m_bPreparingStream;
    bool                  m_bStopRequest;
    char                  field_17;
    uint32                m_iTrackId;
    uint32                field_1C;
    int32                 m_iNextTrackId;
    int8                  m_TrackFlags;
    char                  field_25;
    char                  field_26;
    char                  field_27;
    uint32                m_nPlayTime;
    int32                 m_nTrackLengthMs;
    int32                 m_nActiveTrackId;
    int32                 m_nPlayingTrackId;
    _RTL_CRITICAL_SECTION m_criticalSection;

public:
    static void InjectHooks();

    CAEStreamThread() = default; // 0x4F11D0
    ~CAEStreamThread();

    bool Initialise(CAEStreamingChannel* streamingChannel);

    void Start();
    void Stop();
    void Pause() const;
    void Resume() const;
    void WaitForExit() const;

    void PlayTrack(uint32 iTrackId, int32 iNextTrackId, uint32 a3, int32 a4, bool bIsUserTrack, bool bNextIsUserTrack);
    void StopTrack();

    int16 GetTrackPlayTime() const;
    int32 GetTrackLengthMs() const;
    int32 GetActiveTrackID() const;
    int32 GetPlayingTrackID() const;

    static DWORD WINAPI MainLoop(LPVOID);
    void Service();
};

VALIDATE_SIZE(CAEStreamThread, 0x50);
