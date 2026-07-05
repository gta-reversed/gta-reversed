#pragma once

#include <Base.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <debugapi.h>


namespace notsa {
namespace debug {
bool IsDebuggerPresent() {
    return ::IsDebuggerPresent();
}

void WaitForDebugger() {
    while (!IsDebuggerPresent()) {
        NOTSA_LOG_INFO("[debug] Waiting for debugger\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void DisplayConsole() {
    // Support UTF-8 IO for Windows Terminal. (or CMD if a supported font is used)
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

#ifdef NOTSA_STANDALONE
    // In standalone mode try using the parent process's console if it has one
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        VERIFY(AllocConsole());
    }
#else
    // In asi mode always allocate a new console
    VERIFY(AllocConsole());
#endif

    FILESTREAM fs{};
    VERIFY(freopen_s(&fs, "CONIN$", "r", stdin) == NOERROR);
    VERIFY(freopen_s(&fs, "CONOUT$", "w", stdout) == NOERROR);
    VERIFY(freopen_s(&fs, "CONOUT$", "w", stderr) == NOERROR);
}
};
};