#include "StdInc.h"

HANDLE(&gStreamFileHandles)[MAX_CD_STREAM_HANDLES] = *(HANDLE(*)[MAX_CD_STREAM_HANDLES])0x8E4010;
char(&gCdImageNames)[MAX_CD_STREAM_HANDLES][MAX_CD_STREAM_IMAGE_NAME_SIZE] = *(char(*)[MAX_CD_STREAM_HANDLES][MAX_CD_STREAM_IMAGE_NAME_SIZE])0x8E4098;
uint32& gStreamFileCreateFlags = *(uint32*)0x8E3FE0;
CdStream*& gCdStreams = *(CdStream**)0x8E3FFC;
int32& gStreamCount = *(int32*)0x8E4090;
int32& gOpenStreamCount = *(int32*)0x8E4094;
int32& gStreamingInitialized = *(int32*)0x8E3FE4;
int32& gOverlappedIO = *(int32*)0x8E3FE8;
Queue& gStreamQueue = *(Queue*)0x8E3FEC;
HANDLE& gStreamSemaphore = *(HANDLE*)0x8E4004;
HANDLE& gStreamingThread = *(HANDLE*)0x8E4008;
DWORD& gStreamingThreadId = *(DWORD*)0x8E4000;
uint32& gLastCdStreamPosn = *(uint32*)0x8E4898;

#define APPLY_CD_STREAM_DEADLOCK_FIX 1

#ifdef APPLY_CD_STREAM_DEADLOCK_FIX
// thanks to http://forums.codeguru.com/showthread.php?175474-a-CCriticalSection-question
class CSync {
public:
    CSync() { InitializeCriticalSection(&m_CriticalSection); }
    ~CSync() { DeleteCriticalSection(&m_CriticalSection); }
    void Acquire() { EnterCriticalSection(&m_CriticalSection); }
    void Release() { LeaveCriticalSection(&m_CriticalSection); }

private:
    CRITICAL_SECTION m_CriticalSection;
};

class CLockGuard {
public:
    CLockGuard(CSync& refSync) : m_refSync(refSync) { Lock(); }
    ~CLockGuard() { Unlock(); }

private:
    CSync& m_refSync;
    CLockGuard(const CLockGuard& refcSource);
    CLockGuard& operator=(const CLockGuard& refcSource);

public:
    void Lock() { m_refSync.Acquire(); }
    void Unlock() { m_refSync.Release(); }
};

static CSync cdStreamThreadSync;
#endif
#include "AEBankLoader.h"
void InjectCdStreamHooks() {
    RH_ScopedNamespaceName("CdStream");
    RH_ScopedCategoryGlobal();

    RH_ScopedGlobalInstall(CdStreamOpen, 0x4067B0);
    RH_ScopedGlobalInstall(CdStreamSync, 0x406460);
    RH_ScopedGlobalInstall(CdStreamGetStatus, 0x4063E0);
    RH_ScopedGlobalInstall(CdStreamRead, 0x406A20);
    RH_ScopedGlobalInstall(CdStreamThread, 0x406560);
    RH_ScopedGlobalInstall(CdStreamInitThread, 0x4068F0);
    RH_ScopedGlobalInstall(CdStreamInit, 0x406B70);
    RH_ScopedGlobalInstall(CdStreamRemoveImages, 0x406690);
    RH_ScopedGlobalInstall(CdStreamShutdown, 0x406370);
}

// 0x4067B0
int32 CdStreamOpen(const char* lpFileName) {
    NOTSA_LOG_DEBUG("CdStreamOpen: {}", lpFileName);
    int32 freeHandleIndex = 0;
    for (; freeHandleIndex < MAX_CD_STREAM_HANDLES; freeHandleIndex++) {
        if (!gStreamFileHandles[freeHandleIndex])
            break;
    }
    SetLastError(NO_ERROR);
    const DWORD dwFlagsAndAttributes = gStreamFileCreateFlags | FILE_ATTRIBUTE_READONLY | FILE_FLAG_RANDOM_ACCESS;
    HANDLE file = CreateFileA(lpFileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, dwFlagsAndAttributes, nullptr);
    gStreamFileHandles[freeHandleIndex] = file;
    if (file == INVALID_HANDLE_VALUE)
        return 0;
    strncpy_s(gCdImageNames[freeHandleIndex], lpFileName, MAX_CD_STREAM_IMAGE_NAME_SIZE);
    return freeHandleIndex << 24;
}

// This function halts the caller thread if CdStreamThread is still reading the file to "sync" it.
// CdStreamSync is always called from the main thread. It's called when entering or exiting
// interiors. It's used when the game wants a model to be loaded immediately,
// like when spawning after exiting interiors.
// This function was responsible for causing the deadlock since it was not thread-safe.
// The deadlock problem only exists on PC version of SA. R* fixed it on android.
// So it's fixed here because we used a critical section for it.
// Workflow:
// 1. CdStreamRead is called on the main thread and signals gStreamSemaphore to
//    read game asset in a secondary thread a.k.a. CdStreamThread.
// 2. CdStreamThread starts reading the file on the secondary thread, and the main thread
//    immediately calls CdStreamSync to stop the main thread from execution until the model data has been read from the file.
// 3. When CdStreamThread is done reading the file, it signals `stream.sync.hSemaphore`, so the main thread can
//    continue executing code and continue the gameplay.
// 0x406460
eCdStreamStatus CdStreamSync(int32 streamId) {
    CdStream& stream = gCdStreams[streamId];
#ifdef APPLY_CD_STREAM_DEADLOCK_FIX
    CLockGuard lockGuard(cdStreamThreadSync);
#endif
    if (gStreamingInitialized) {
        if (stream.nSectorsToRead) {
            stream.bLocked = true;
#ifdef APPLY_CD_STREAM_DEADLOCK_FIX
            lockGuard.Unlock();
#endif
            WaitForSingleObject(stream.sync.hSemaphore, INFINITE);
#ifdef APPLY_CD_STREAM_DEADLOCK_FIX
            lockGuard.Lock();
#endif
        }
        stream.bInUse = false;
        return stream.status;
    } else if (gOverlappedIO && stream.hFile) {
        DWORD numberOfBytesTransferred = 0;
        if (!GetOverlappedResult(stream.hFile, &stream.overlapped, &numberOfBytesTransferred, true))
            return eCdStreamStatus::READING_FAILURE;
    }
    return eCdStreamStatus::READING_SUCCESS;
}

// 0x4063E0
eCdStreamStatus CdStreamGetStatus(int32 streamId) {
    CdStream& stream = gCdStreams[streamId];
    if (gStreamingInitialized) {
        if (stream.bInUse)
            return eCdStreamStatus::READING;

        if (stream.nSectorsToRead)
            return eCdStreamStatus::WAITING_TO_READ;

        if (stream.status != eCdStreamStatus::READING_SUCCESS) {
            const eCdStreamStatus status = stream.status;
            stream.status = eCdStreamStatus::READING_SUCCESS;
            return status;
        }
    } else if (gOverlappedIO) {
        if (WaitForSingleObjectEx(stream.hFile, 0, 1) != WAIT_OBJECT_0)
            return eCdStreamStatus::READING;
    }
    return eCdStreamStatus::READING_SUCCESS;
}

// When CdStreamRead is called, it will update CdStream information for the channel and
// signal gStreamSemaphore, so the secondary thread `CdStreamThread` can start reading the models.
// If this function is called with the same channelId/streamId whilst CdStreamThread is still reading the previous model
// for the channel, then it will return false.
// When CdStreamThread is done reading the model, then CdStreamThread will set `stream.nSectorsToRead` and `stream.bInUse` to 0,
// so the main thread can call CdStreamRead again to read more models.
// 0x406A20
bool CdStreamRead(int32 streamId, void* lpBuffer, uint32 offsetAndHandle, int32 sectorCount) {
    CdStream& stream = gCdStreams[streamId];
    gLastCdStreamPosn = sectorCount + offsetAndHandle;
    const uint32 sectorOffset = offsetAndHandle & 0xFFFFFF;
    stream.hFile = gStreamFileHandles[offsetAndHandle >> 24];
    SetLastError(NO_ERROR);
    if (gStreamingInitialized) {
        if (stream.nSectorsToRead || stream.bInUse)
            return false;
        stream.status = eCdStreamStatus::READING_SUCCESS;
        stream.nSectorOffset = sectorOffset;
        stream.nSectorsToRead = sectorCount;
        stream.lpBuffer = lpBuffer;
        stream.bLocked = false;
        AddToQueue(&gStreamQueue, streamId);
        if (!ReleaseSemaphore(gStreamSemaphore, 1, nullptr))
            DEV_LOG("Signal Sema Error");
        return true;
    }
    const DWORD numberOfBytesToRead = sectorCount * STREAMING_SECTOR_SIZE;
    const DWORD overlappedOffset = sectorOffset * STREAMING_SECTOR_SIZE;
    if (gOverlappedIO) {
        LPOVERLAPPED overlapped = &gCdStreams[streamId].overlapped;
        overlapped->Offset = overlappedOffset;
        if (ReadFile(stream.hFile, lpBuffer, numberOfBytesToRead, nullptr, overlapped) || GetLastError() == ERROR_IO_PENDING)
            return true;
        return false;
    }
    SetFilePointer(stream.hFile, overlappedOffset, nullptr, 0);
    DWORD numberOfBytesRead = 0;
    return ReadFile(stream.hFile, lpBuffer, numberOfBytesToRead, &numberOfBytesRead, nullptr);
}

// 0x406560
[[noreturn]] void WINAPI CdStreamThread(LPVOID lpParam) {
#ifdef TRACY_ENABLE
    tracy::SetThreadName("CdStreamThread");
#endif

    while (true) {
        WaitForSingleObject(gStreamSemaphore, INFINITE);

        ZoneScoped;

        const int32 streamId = GetFirstInQueue(&gStreamQueue);
        CdStream& stream = gCdStreams[streamId];
        stream.bInUse = true;
        if (stream.status == eCdStreamStatus::READING_SUCCESS) {
            const DWORD numberOfBytesToRead = stream.nSectorsToRead * STREAMING_SECTOR_SIZE;
            const DWORD overlappedOffset = stream.nSectorOffset * STREAMING_SECTOR_SIZE;
            if (gOverlappedIO) {
                stream.overlapped.Offset = overlappedOffset;
                if (ReadFile(stream.hFile, stream.lpBuffer, numberOfBytesToRead, nullptr, &stream.overlapped)) {
                    stream.status = eCdStreamStatus::READING_SUCCESS;
                } else if (GetLastError() != ERROR_IO_PENDING) {
                    stream.status = eCdStreamStatus::READING_FAILURE;
                } else {
                    DWORD numberOfBytesTransferred = 0;
                    if (GetOverlappedResult(stream.hFile, &stream.overlapped, &numberOfBytesTransferred, true))
                        stream.status = eCdStreamStatus::READING_SUCCESS;
                    else
                        stream.status = eCdStreamStatus::READING_FAILURE;
                }
            } else {
                SetFilePointer(stream.hFile, overlappedOffset, nullptr, 0u);
                DWORD numberOfBytesRead = 0;
                if (ReadFile(stream.hFile, stream.lpBuffer, numberOfBytesToRead, &numberOfBytesRead, nullptr))
                    stream.status = eCdStreamStatus::READING_SUCCESS;
                else
                    stream.status = eCdStreamStatus::READING_FAILURE;
            }
        }

        RemoveFirstInQueue(&gStreamQueue);
#ifdef APPLY_CD_STREAM_DEADLOCK_FIX
        CLockGuard lockGuard(cdStreamThreadSync);
#endif
        // locking is necessary here, so ReleaseSemaphore is not called before WaitForSingleObject
        // in CdStreamSync to avoid causing a deadlock.
        stream.nSectorsToRead = 0;
        if (stream.bLocked)
            ReleaseSemaphore(stream.sync.hSemaphore, 1, nullptr);
        stream.bInUse = false;
    }
}

// 0x4068F0
void CdStreamInitThread() {
    SetLastError(NO_ERROR);
    for (auto& stream : std::span{ gCdStreams, (size_t)gStreamCount }) {
        HANDLE hSemaphore = OS_SemaphoreCreate(2, nullptr);
        stream.sync.hSemaphore = hSemaphore;
        if (!hSemaphore) {
            DEV_LOG("cdvd_stream: failed to create sync semaphore");
            return;
        }
    }
    InitialiseQueue(&gStreamQueue, gStreamCount + 1);
    gStreamSemaphore = OS_SemaphoreCreate(5, "CdStream");
    if (gStreamSemaphore) {
        gStreamingThread = CreateThread(nullptr, 0x10000, (LPTHREAD_START_ROUTINE)CdStreamThread, nullptr, CREATE_SUSPENDED, &gStreamingThreadId);
        if (gStreamingThread) {
            SetThreadPriority(gStreamingThread, GetThreadPriority(GetCurrentThread()));
            ResumeThread(gStreamingThread);
        } else {
            DEV_LOG("cdvd_stream: failed to create streaming thread");
        }
    } else {
        DEV_LOG("cdvd_stream: failed to create stream semaphore");
    }
}

// 0x406B70
void CdStreamInit(int32 streamCount) {
    for (int32 i = 0; i < MAX_CD_STREAM_HANDLES; i++) {
        gStreamFileHandles[i] = nullptr;
        gCdImageNames[i][0] = 0;
    }
    DWORD bytesPerSector = 0;
    DWORD totalNumberOfClusters = 0;
    DWORD numberOfFreeClusters = 0;
    DWORD sectorsPerCluster = 0;
    GetDiskFreeSpace(nullptr, &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);

    gStreamFileCreateFlags = FILE_FLAG_OVERLAPPED;
#ifndef FIX_BUGS // this just slows down streaming
    if (bytesPerSector <= STREAMING_SECTOR_SIZE) {
        gStreamFileCreateFlags |= FILE_FLAG_NO_BUFFERING;
    }
#endif

    gOverlappedIO = 1;
    gStreamingInitialized = 0;
    auto* pAllocatedMemory = CMemoryMgr::MallocAlign(STREAMING_SECTOR_SIZE, bytesPerSector);
    SetLastError(NO_ERROR);
    gOpenStreamCount = 0;
    gStreamCount = streamCount;
    gCdStreams = (CdStream*)LocalAlloc(LPTR, sizeof(CdStream) * streamCount);
    CdStreamOpen("MODELS\\GTA3.IMG");
    bool bStreamRead = CdStreamRead(0, pAllocatedMemory, 0, 1);
    CdStreamRemoveImages();
    gStreamingInitialized = 1;
    if (!bStreamRead) {
        gOverlappedIO = 0;
        gStreamFileCreateFlags &= ~FILE_FLAG_OVERLAPPED;
    }
    CdStreamInitThread();
    CMemoryMgr::FreeAlign(pAllocatedMemory);
}

// 0x406690
void CdStreamRemoveImages() {
    for (int32 i = 0; i < gStreamCount; ++i) {
        CdStreamSync(i);
    }
    for (int32 i = 0; i < gOpenStreamCount; i++) {
        SetLastError(NO_ERROR);
        if (gStreamFileHandles[i]) {
            CloseHandle(gStreamFileHandles[i]);
        }
        gStreamFileHandles[i] = nullptr;
        gCdImageNames[i][0] = 0;
    }
    gOpenStreamCount = 0;
}

// 0x406370
void CdStreamShutdown() {
    if (gStreamingInitialized) {
        FinalizeQueue(&gStreamQueue);
        CloseHandle(gStreamSemaphore);
        CloseHandle(gStreamingThread);
        for (auto& stream : std::span{ gCdStreams, (size_t)gStreamCount }) {
            CloseHandle(stream.sync.hSemaphore);
        }
    }
    LocalFree(gCdStreams);
}
