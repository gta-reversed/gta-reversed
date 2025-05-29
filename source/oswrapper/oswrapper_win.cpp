#include "StdInc.h"

#include "oswrapper.h"

#include <Windows.h>

CHAR BasePath[_MAX_PATH] = { 0 };

// 0x5617A0
uint64 GetOSWPerformanceTime() {
    LARGE_INTEGER PerformanceCount = { 0 };
    ::QueryPerformanceCounter(&PerformanceCount);
    return PerformanceCount.QuadPart;
}

int64 GetOSWPerformanceFrequency() {
    LARGE_INTEGER frequency;
    ::QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}

void OS_DebugOut(const char* msg) {
#ifdef _DEBUG
    static HANDLE hFile = CreateFile("OS_DebugOut.log", GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, NULL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD dwWritten = 0;
        WriteFile(hFile, msg, strlen(msg), &dwWritten, nullptr);
        WriteFile(hFile, "\r\n", 2, &dwWritten, nullptr);
    }
#endif

    OutputDebugStringA(msg);
    OutputDebugStringA("\n");
}

const char* AND_FileGetArchiveName() {
    return nullptr;
}

const char* OS_FileGetArchiveName(int32 archive) {
    return AND_FileGetArchiveName();
}

int32 OS_FileSize(void* file) {
    return GetFileSize(file, nullptr);
}

int32 OS_FileDelete(const char* file) {
    return DeleteFileA(file);
}

// todo: ("2014/09/15: Check access type flags.")
int32 OS_FileOpen(OSFileDataArea dataArea, void** pfile, const char* file, OSFileAccessType access) {
    DWORD dwDesiredAccess       = GENERIC_READ;
    DWORD dwShareMode           = FILE_SHARE_READ;
    DWORD dwCreationDisposition = OPEN_EXISTING;

    if (access == FILE_ACCESS_WRITE) {
        dwCreationDisposition = CREATE_ALWAYS;
        dwDesiredAccess |= GENERIC_WRITE;
    }

    CHAR path[_MAX_PATH * 2];
    strcpy_s(path, BasePath);
    if (strlen(BasePath) && BasePath[strlen(BasePath) - 1] != '\\') {
        strcat_s(path, "\\");
    }
    strcat_s(path, file);

    HANDLE hFile = CreateFileA(path, dwDesiredAccess, dwShareMode, nullptr, dwCreationDisposition, NULL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE) {
        *pfile = nullptr;
        return GetLastError();
    }

    memcpy(pfile, &hFile, sizeof(hFile));
    SetLastError(0);

    return S_OK;
}

int32 OS_FileGetPosition(void* file) {
    return (int32)SetFilePointer(&file, 0, nullptr, SEEK_CUR);
}

void OS_FileSetPosition(void* file, int32 pos) {
    SetFilePointer(&file, pos, nullptr, SEEK_SET);
}

// todo: ("2014/09/15: !!! Rewrite as it was implemented originally !!!")
int32 OS_FileRead(void* file, void* dest, int32 destSize) {
    DWORD dwRead = 0;
    //SetLastError(0);
    VERIFY(ReadFile(&file, dest, destSize, &dwRead, nullptr));
    return dwRead == destSize ? 0 : 3;
}

int32 OS_FileWrite(void* file, const void* src, int32 srcSize) {
    DWORD dwWrite = 0;
    //SetLastError(0);
    WriteFile(&file, src, srcSize, &dwWrite, nullptr);
    return dwWrite == srcSize ? 0 : 3;
}

int32 OS_FileClose(void* file) {
    //SetLastError(0);
    return CloseHandle(&file);
}

void OS_DebugBreak() {
    DebugBreak();
}

void OS_ThreadSleep(uint32 sec) {
    Sleep(sec);
}

void* OS_SemaphoreCreate(int32 iMaximumCount, const char* name) {
    return CreateSemaphore(nullptr, 0, iMaximumCount, name);
}

bool OS_SemaphorePost(void* semaphore) {
    return ReleaseSemaphore(&semaphore, 1, nullptr);
}

void OS_SemaphoreWait(void* semaphore) {
    WaitForSingleObject(&semaphore, INFINITE);
}

void OS_SemaphoreDelete(void* semaphore) {
    VERIFY(CloseHandle(static_cast<HANDLE>(&semaphore)));
}

void* OS_MutexCreate(const char* name) {
    UNUSED(name);
    return CreateMutex(nullptr, false, nullptr);
}

void OS_MutexRelease(void* mutex) { // TODO: `VERIFY` fails here - Fix
    //VERIFY(ReleaseMutex(&mutex));
    (void)ReleaseMutex(&mutex);
}

void OS_MutexObtain(void* mutex) {
    WaitForSingleObject(&mutex, INFINITE);
}

void OS_MutexDelete(void* mutex) {
    if (mutex) {
        VERIFY(CloseHandle(static_cast<HANDLE>(&mutex)));
    }
}

void OS_SetFilePathOffset(const char* path) {
    if (!path) {
        BasePath[0] = '\0';
        return;
    }
    strcpy_s(BasePath, path);
}

// todo: ("2014/09/14: fix priorities and other shit")
void* OS_ThreadLaunch(OS_ThreadRoutine pfnStart, void* param, uint32 nFlags, const char* name, void* hUnknown, OSThreadPriority priority) {
    DWORD dwFlags = 0;

    if (nFlags & OS_CREATE_SUSPENDED) {
        dwFlags |= CREATE_SUSPENDED;
    }

    DWORD dwThreadID = 0;
    HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)pfnStart, param, dwFlags, &dwThreadID);
    assert(hThread && hThread != INVALID_HANDLE_VALUE);
    switch (priority) {
    case OS_THREAD_PRIORITY_LOW:
        SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
        break;
    case OS_THREAD_PRIORITY_NORMAL:
        SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
        break;
    case OS_THREAD_PRIORITY_HIGH:
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
        break;
    case OS_THREAD_PRIORITY_HIGHEST:
        SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
        break;
    }

    NOTSA_LOG_DEBUG("OS: Thread {} ({}) created", dwThreadID, name);

    return hThread;
}

void OS_ThreadClose(void* thread) {
    VERIFY(CloseHandle(static_cast<HANDLE>(thread)));
}

void OS_ThreadWait(void* thread) {
    WaitForSingleObject(&thread, INFINITE);
}

void OS_ThreadResume(void* thread) {
    ResumeThread(&thread);
}

double OS_TimeAccurate() {
    LARGE_INTEGER PerformanceCount;
    QueryPerformanceCounter(&PerformanceCount);
    return PerformanceCount.QuadPart / double(1000.f);
}

uint32 OS_TimeMS() {
    return uint32(GetOSWPerformanceTime() / GetOSWPerformanceFrequency());
}
