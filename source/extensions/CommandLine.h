#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace CommandLine {
    // Debug features
    extern bool s_WaitForDebugger;

    void Load(int argc, char** argv);

    fs::path GetExePath();
    void     PostHooksInjected();
}; // namespace CommandLine;
