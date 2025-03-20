#include "StdInc.h"
#include "CommandLine.h"

#include <span>
#include <Utility.h>
#include <app_debug.h>

namespace CommandLine {
    bool s_UnhookAll{false};
    std::vector<std::string_view> s_UnhookSome{};
    std::vector<std::string_view> s_UnhookExcept{};

    bool waitForDebugger{false};

    void ProcessArgument(const char* arg) {
        const auto str = std::string_view{arg, std::strlen(arg)};

        if (str == "--debug") {
            waitForDebugger = true;
            return;
        }

        if (str == "--unhook-all") {
            if (!s_UnhookExcept.empty()) {
                NOTSA_LOG_WARN("--unhook-except has been called previously, unhook-all will be effective.");
                s_UnhookExcept.clear(); // we aren't gonna use it.
            }
            if (!s_UnhookSome.empty()) {
                NOTSA_LOG_WARN("--unhook-some has been called previously, unhook-all will be effective.");
                s_UnhookSome.clear(); // we aren't gonna use it.
            }
            s_UnhookAll = true;
            return;
        }

        if (str.starts_with("--unhook-except=")) {
            if (s_UnhookAll) {
                NOTSA_LOG_WARN("--unhook-all has been called previously, unhook-all will be effective.");
            } else {
                if (!s_UnhookSome.empty()) {
                    NOTSA_LOG_WARN("--unhook has been called previously, unhook-except will be effective.");
                    s_UnhookSome.clear(); // we aren't gonna use it.
                }

                for (auto hook : SplitStringView(str.substr(str.find('=') + 1), ",")) {
                    s_UnhookExcept.emplace_back(std::move(hook));
                }
            }
            return;
        }

        if (str.starts_with("--unhook=")) {
            if (s_UnhookAll) {
                NOTSA_LOG_WARN("--unhook-all has been called previously, unhook-all will be effective.");
            } else if (!s_UnhookExcept.empty()) {
                NOTSA_LOG_WARN("--unhook-except has been called previously, unhook-except will be effective.");
            } else {
                for (auto hook : SplitStringView(str.substr(str.find('=') + 1), ",")) {
                    s_UnhookSome.emplace_back(std::move(hook));
                }
            }
            return;
        }

        NOTSA_LOG_WARN("Unknown argument '{}'", str);
    }

    void CommandLine::Load(int argc, char** argv) {
        if (--argc <= 0)
            return; // No need to process, there is no argument supplied.
        ++argv;     // Skip first 'argument' which is the exec path.

        for (auto& arg : std::span{argv, (size_t)argc})
            ProcessArgument(arg);
    }
} // namespace CommandLine
