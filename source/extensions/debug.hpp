#pragma once

#include <Base.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <debugapi.h>
#include <cstdio>


namespace notsa {
namespace debug {
inline bool IsDebuggerPresent() {
    return ::IsDebuggerPresent();
}

inline void WaitForDebugger() {
    while (!IsDebuggerPresent()) {
        NOTSA_LOG_INFO("[debug] Waiting for debugger\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

inline void DisplayConsole() {
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

/*!
 * @return function information (module, function name, line number) at the given address.
 */
std::string GetFunctionInfoAtAddress(uintptr_t address, bool compact = false, HANDLE hProcess = GetCurrentProcess());

/*!
 * @brief Initialize debugging symbols
 * @note Call after the CommandLine has initialized, as it uses the exe path to initialize symbols.
 */
void LoadSymbols();

/*!
 * @brief Unload symbols
 */
void UnloadSymbols();

};
};
