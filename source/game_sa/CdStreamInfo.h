#pragma once

#include <Windows.h>
#include "Queue.h"

enum class eCdStreamStatus : int32 {
    READING_SUCCESS = 0,
    WAITING_TO_READ = 250,
    READING_FAILURE = 254,
    READING = 255,
};

union SyncObj {
    HANDLE             hSemaphore;
    CONDITION_VARIABLE cv;
};

struct CdStream {
    uint32          nSectorOffset;
    uint32          nSectorsToRead;
    LPVOID          lpBuffer;
    BYTE            field_C;
    bool            bLocked;
    bool            bInUse;
    BYTE            field_F;
    eCdStreamStatus status;
    SyncObj         sync;
    HANDLE          hFile;
    OVERLAPPED      overlapped;
};

struct CdStreamInfoSA {
    DWORD     streamCreateFlags;
    BOOL      streamingInitialized;
    BOOL      overlappedIO;
    Queue     m_queue;
    CdStream* pStreams;
    DWORD     thread_id;
    HANDLE    semaphore;
    HANDLE    thread;
    HANDLE    streamHandles[32];
    DWORD     streamCount;
    DWORD     openStreamCount;
    CHAR      streamNames[2048];
    DWORD     field_8A8;
    DWORD     lastPosn;
    DWORD     field_8B0;
    DWORD     field_8B4;
    DWORD     gtaint_id;
    DWORD     gta3_id;
};

VALIDATE_SIZE(SyncObj, sizeof(HANDLE));
VALIDATE_SIZE(CdStreamInfoSA, 0x8CC);
VALIDATE_SIZE(CdStream, 0x30);

const int32 MAX_CD_STREAM_HANDLES = 32;
const int32 MAX_CD_STREAM_IMAGE_NAME_SIZE = 64;

extern HANDLE(&gStreamFileHandles)[MAX_CD_STREAM_HANDLES];
extern char(&gCdImageNames)[MAX_CD_STREAM_HANDLES][MAX_CD_STREAM_IMAGE_NAME_SIZE];
extern uint32& gStreamFileCreateFlags;
extern CdStream*& gCdStreams;
extern int32& gStreamCount;
extern int32& gOpenStreamCount;
extern int32& gStreamingInitialized;
extern int32& gOverlappedIO;
extern Queue& gStreamQueue;
extern HANDLE& gStreamSemaphore;
extern HANDLE& gStreamingThread;
extern DWORD& gStreamingThreadId;
extern uint32& gLastCdStreamPosn;

void InjectCdStreamHooks();
int32 CdStreamOpen(const char* lpFileName);
eCdStreamStatus CdStreamSync(int32 streamId);
eCdStreamStatus CdStreamGetStatus(int32 streamId);
bool CdStreamRead(int32 streamId, void* lpBuffer, uint32 offsetAndHandle, int32 sectorCount);
static uint32 CdStreamGetLastPosn() { return gLastCdStreamPosn; }
[[noreturn]] void WINAPI CdStreamThread(LPVOID lpParam);
void CdStreamInitThread();
void CdStreamInit(int32 streamCount);
void CdStreamRemoveImages();
void CdStreamShutdown();
