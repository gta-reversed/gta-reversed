#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace CommandLine {
    // Hook features
    extern bool s_UnhookAll;
    extern std::vector<std::string_view> s_UnhookSome;
    extern std::vector<std::string_view> s_UnhookExcept;
    extern fs::path s_DumpHooksPath;

    // Debug features
    extern bool s_WaitForDebugger;

    void Load(int argc, char** argv);
} // namespace CommandLine;
