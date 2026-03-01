#pragma once

namespace CommandLine {
    // Hook features
    extern bool s_UnhookAll;
    extern std::vector<std::string_view> s_UnhookSome;
    extern std::vector<std::string_view> s_UnhookExcept;

    // Debug features
    extern bool waitForDebugger;

    void Load(int argc, char** argv);
} // namespace CommandLine;
