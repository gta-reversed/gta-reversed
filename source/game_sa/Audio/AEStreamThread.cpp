#include "StdInc.h"

#include "AEStreamThread.h"

#include "AEAudioUtility.h"

bool IsForeground() {
    return plugin::CallAndReturn<bool, 0x746060>();
}

void CAEStreamThread::InjectHooks() {
    using namespace ReversibleHooks;
    // Install("CAEStreamThread", "Initialise", 0x4F1680, &CAEStreamThread::Initialise);
    Install("CAEStreamThread", "Start", 0x4F11F0, &CAEStreamThread::Start);
    Install("CAEStreamThread", "Pause", 0x4F1200, &CAEStreamThread::Pause);
    Install("CAEStreamThread", "Resume", 0x4F1210, &CAEStreamThread::Resume);
    Install("CAEStreamThread", "WaitForExit", 0x4F1220, &CAEStreamThread::WaitForExit);
    Install("CAEStreamThread", "PlayTrack", 0x4F1230, &CAEStreamThread::PlayTrack);
    Install("CAEStreamThread", "GetTrackPlayTime", 0x4F1530, &CAEStreamThread::GetTrackPlayTime);
    Install("CAEStreamThread", "GetTrackLengthMs", 0x4F1550, &CAEStreamThread::GetTrackLengthMs);
    Install("CAEStreamThread", "GetActiveTrackID", 0x4F1560, &CAEStreamThread::GetActiveTrackID);
    Install("CAEStreamThread", "GetPlayingTrackID", 0x4F1570, &CAEStreamThread::GetPlayingTrackID);
    Install("CAEStreamThread", "StopTrack", 0x4F1580, &CAEStreamThread::StopTrack);
    Install("CAEStreamThread", "MainLoop", 0x4F15C0, &CAEStreamThread::MainLoop);
}

// 0x4F11E0
CAEStreamThread::~CAEStreamThread() {
    DeleteCriticalSection(&m_criticalSection);
}

// 0x4F1680
void CAEStreamThread::Initialise(CAEStreamingChannel* streamingChannel) {
    plugin::Call<0x4F1680, CAEStreamingChannel*>(streamingChannel);

    /*
    m_bActive          = true;
    m_bThreadActive    = false;
    field_12           = 0;
    m_bPreparingStream = 0;
    m_bStopRequest     = false;
    m_nPlayingTrackId  = streamingChannel->GetPlayingTrackID();
    m_nPlayTime        = streamingChannel->GetPlayTime();
    m_nActiveTrackId   = streamingChannel->GetActiveTrackID();
    m_nTrackLengthMs   = streamingChannel->GetLength();

    m_nHandle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)(&CAEStreamThread::MainLoop), this, CREATE_SUSPENDED, (LPDWORD)(&m_lpThreadId));
    SetThreadPriority(m_nHandle, 0);

    InitializeCriticalSection(&m_criticalSection);
    m_pStreamingChannel = streamingChannel;

    m_pMp3TrackLoader = new CAEMP3TrackLoader();
    m_pMp3TrackLoader->Initialise();
    */
}

// 0x4F11F0
void CAEStreamThread::Start() {
    m_bThreadActive = true;
    ResumeThread(m_nHandle);
}

// 0x4F1200
void CAEStreamThread::Pause() const {
    SuspendThread(m_nHandle);
}

// 0x4F1210
void CAEStreamThread::Resume() const {
    ResumeThread(m_nHandle);
}

// 0x4F1220
void CAEStreamThread::WaitForExit() const {
    WaitForSingleObject(m_nHandle, INFINITE);
}

// 0x4F1530
int32 CAEStreamThread::GetTrackPlayTime() const {
    if (m_bPreparingStream)
        return -8;
    else
        return m_pStreamingChannel->GetPlayTime();
}

// 0x4F1550
int32 CAEStreamThread::GetTrackLengthMs() const {
    return m_nTrackLengthMs;
}

// 0x4F1560
int32 CAEStreamThread::GetActiveTrackID() const {
    return m_nActiveTrackId;
}

// 0x4F1570
int32 CAEStreamThread::GetPlayingTrackID() const {
    return m_nPlayingTrackId;
}

// 0x4F1230
void CAEStreamThread::PlayTrack(uint32 iTrackId, int32 iNextTrackId, uint32 a3, int32 a4, bool bIsUserTrack, bool bNextIsUserTrack) {
    EnterCriticalSection(&m_criticalSection);

    if (m_pStreamingChannel->GetPlayTime() == -2)
        m_pStreamingChannel->Stop();

    m_iTrackId         = iTrackId;
    m_iNextTrackId     = iNextTrackId;
    field_1C           = a3;
    m_bIsUserTrackD    = a4;
    m_bIsUserTrack     = bIsUserTrack;
    m_bNextIsUserTrack = bNextIsUserTrack;
    field_12           = 1;

    LeaveCriticalSection(&m_criticalSection);
}

// 0x4F1580
void CAEStreamThread::StopTrack() {
    m_bStopRequest = true;
}

// 0x4F15C0
uint32 CAEStreamThread::MainLoop(void* param) {
    auto* stream = reinterpret_cast<CAEStreamThread*>(param);

    bool wasForeground = true;
    bool play = false;

    while (stream->m_bThreadActive) {
        bool isForeground = IsForeground();
        if (isForeground) {
            if (!wasForeground && play){
                stream->m_pStreamingChannel->Play(0, 0, 1.0f);
            }

            auto start = CAEAudioUtility::GetCurrentTimeInMilliseconds();

            stream->Service();
            stream->m_pStreamingChannel->Service();

            auto end = CAEAudioUtility::GetCurrentTimeInMilliseconds();
            if (end - start < 5)
                ::Sleep(start - end + 5);

        } else if (wasForeground) {
            if (stream->m_pStreamingChannel->IsBufferPlaying()) {
                stream->m_pStreamingChannel->Pause();
                play = true;
            } else {
                play = false;
            }
        }
        wasForeground = isForeground;
        ::Sleep(100);
    }

    return 0;
}

// 0x4F1290
void CAEStreamThread::Service() {
    plugin::CallMethod<0x4F1290, CAEStreamThread*>(this);
}
