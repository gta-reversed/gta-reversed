#pragma once

# ifndef OSWAPI
#   if defined(_OSWDLL)
#     ifdef _OSWEXPORTS
#       define OSWAPI extern __declspec(dllexport)
#     else
#       define OSWAPI extern __declspec(dllimport)
#     endif
#   else
#     define OSWAPI extern
#   endif
# endif

enum OSFileAccessType {
    FILE_ACCESS_READ     = 0,
    FILE_ACCESS_WRITE    = 1, // "wb"
    FILE_ACCESS_EXISTING = 2, // "rb+"
};

enum OSThreadPriority {
    OS_THREAD_PRIORITY_LOW      = 0,
    OS_THREAD_PRIORITY_NORMAL   = 1,
    OS_THREAD_PRIORITY_HIGH     = 2,
    OS_THREAD_PRIORITY_HIGHEST  = 3,
};

enum OSFileDataArea {
    FILE_DATA_AREA_DEFAULT = 0,
    FILE_DATA_AREA_MYDOC   = 1,
};

#define OS_CREATE_SUSPENDED 0x3

#define OS_ThreadAPI __stdcall
typedef uint32(OS_ThreadAPI* OS_ThreadRoutine)(void* param);

OSWAPI void        OS_DebugOut(const char* msg);
OSWAPI void        OS_DebugBreak();
OSWAPI const char* OS_FileGetArchiveName(int32 archive);
OSWAPI int32       OS_FileSize(void* file);
OSWAPI int32       OS_FileOpen(OSFileDataArea dataArea, void** pfile, const char* file, OSFileAccessType access);
OSWAPI int32       OS_FileClose(void* file);
OSWAPI int32       OS_FileDelete(const char* file);
OSWAPI int32       OS_FileRead(void* file, void * dest, int32 nDestSize);
OSWAPI int32       OS_FileGetPosition(void* file);
OSWAPI void        OS_FileSetPosition(void* file, int32 nPos);
OSWAPI int32       OS_FileWrite(void* file, const void* src, int32 nSrcSize);
OSWAPI void*       OS_MutexCreate(const char* name);
OSWAPI void        OS_MutexObtain(void* mutex);
OSWAPI void        OS_MutexRelease(void* mutex);
OSWAPI void        OS_MutexDelete(void* mutex);
OSWAPI void*       OS_SemaphoreCreate(int32 iMaximumCount = 1, const char* name = nullptr);
OSWAPI bool        OS_SemaphorePost(void* semaphore);
OSWAPI void        OS_SemaphoreWait(void* semaphore);
OSWAPI void        OS_SemaphoreDelete(void* semaphore);
OSWAPI void        OS_SetFilePathOffset(const char* path);

OSWAPI void* OS_ThreadLaunch(
    OS_ThreadRoutine  pfnStart,
    void*             pParam,
    uint32            nFlags,
    const char*       name      = nullptr,
    void*             unk       = nullptr,
    OSThreadPriority  nPriority = OS_THREAD_PRIORITY_NORMAL
);

OSWAPI void      OS_ThreadSleep(uint32 nMSec);
OSWAPI void      OS_ThreadClose(void* thread);
OSWAPI void      OS_ThreadWait(void* thread);
OSWAPI void      OS_ThreadResume(void* thread);

OSWAPI double    OS_TimeAccurate();
OSWAPI uint32    OS_TimeMS();

uint64 GetOSWPerformanceTime();
int64 GetOSWPerformanceFrequency();

bool IsWin7OrGreater();
