#include "StdInc.h"

#include "CommandLine.h"

#include <span>
#include <Utility.h>
#include <app_debug.h>

namespace CommandLine {
bool                          s_UnhookAll{ false };
std::vector<std::string_view> s_UnhookSome{};
std::vector<std::string_view> s_UnhookExcept{};

/* Path to save the hooks dump file to */
fs::path s_DumpHooksPath{};

/* Wait for debugger before continuing */
bool s_WaitForDebugger{ false };

/* exe path */
fs::path s_ExePath{};

void ProcessArgument(std::string_view arg) {
    if (arg == "--debug") {
        s_WaitForDebugger = true;
#ifndef NOTSA_STANDALONE
    } else if (arg == "--all-use-gta-hooks") {
        if (!s_UnhookExcept.empty()) {
            NOTSA_LOG_WARN("--all-use-gta-hooks-except has been called previously, `all-use-gta-hooks` will be effective.");
            s_UnhookExcept.clear(); // we aren't gonna use it.
        }
        if (!s_UnhookSome.empty()) {
            NOTSA_LOG_WARN("--use-gta-hooks-for has been called previously, `all-use-gta-hooks` will be effective.");
            s_UnhookSome.clear(); // we aren't gonna use it.
        }
        s_UnhookAll = true;
    } else if (arg.starts_with("--all-use-gta-hooks-except=")) {
        if (s_UnhookAll) {
            NOTSA_LOG_WARN("--all-use-gta-hooks has been called previously, `all-use-gta-hooks` will be effective.");
        } else {
            if (!s_UnhookSome.empty()) {
                NOTSA_LOG_WARN("--use-gta-for has been called previously, `all-use-gta-hooks-except` will be effective.");
                s_UnhookSome.clear(); // we aren't gonna use it.
            }

            for (auto hook : SplitStringView(arg.substr(arg.find('=') + 1), ",")) {
                s_UnhookExcept.emplace_back(std::move(hook));
            }
        }
    } else if (arg.starts_with("--use-gta-for=")) {
        if (s_UnhookAll) {
            NOTSA_LOG_WARN("--all-use-gta-hooks has been called previously, `all-use-gta-hooks` will be effective.");
        } else if (!s_UnhookExcept.empty()) {
            NOTSA_LOG_WARN("--all-use-gta-hooks-except has been called previously, `all-use-gta-hooks-except` will be effective.");
        } else {
            for (auto hook : SplitStringView(arg.substr(arg.find('=') + 1), ",")) {
                s_UnhookSome.emplace_back(std::move(hook));
            }
        }
#endif
    } else if (arg.starts_with("--dump-hooks-to=")) {
        if (const auto eqPos = arg.find('='); eqPos != std::string_view::npos) {
            s_DumpHooksPath = arg.substr(eqPos + 1);
        } else {
            NOTSA_LOG_WARN("Invalid argument '{}', expected format --dump-hooks-to=path", arg);
        }
    } else {
        NOTSA_LOG_WARN("Unknown argument '{}'", arg);
    }
}

void CommandLine::Load(int argc, char** argv) {
    NOTSA_LOG_DEBUG("[Command Line]: Loading arguments");
    s_ExePath = argv[0];
    for (int32 i = 1; i < argc; i++) { // Skip first 'argument' which is the exec path.
        ProcessArgument(argv[i]);
    }
    NOTSA_LOG_DEBUG("[Command Line]: Arguments loaded");
}

fs::path GetExePath() {
    return s_ExePath;
}


void ApplyHookSettings() {
#ifndef NOTSA_STANDALONE
    using namespace ReversibleHooks;
    using HookState = ReversibleHooks::HookCategoryItem::HookState;

    auto* const rh        = &RHManager::GetInstance();

    // `entry` is a user-provided value
    // it should be in either of the following formats:
    // - Root/Category/SubCategory::HookName - To unhook a single item
    // - Root/Category/SubCategory - To unhook all items in a category (not including it's sub-categories)
    const auto ProcessEntry = [&] (std::string_view entry, HookState hookState) -> std::optional<std::string> {
        const auto itemSepPos = entry.find("::");
        const auto isItem     = itemSepPos != std::string_view::npos;

        const auto categoryName = isItem
            ? entry.substr(0, itemSepPos)
            : entry;
        const auto cat = rh->GetRootCategory()->FindCategoryByPath(categoryName, false);
        if (!cat) {
            return std::format("Category '{}' not found", categoryName);
        }

        if (isItem) {
            const auto itemName = entry.substr(itemSepPos + 2);
            const auto item = cat->FindItem(itemName);
            if (!item) {
                return std::format("Item '{}' not found in category '{}'", itemName, categoryName);
            }
            if (item->GetState() == hookState) {
                return std::nullopt; // Nothing to do
            }
            if (item->GetIsStateLocked()) {
                return std::format("Can't change state of locked item '{}'", entry);
            }
            if (!item->SetState(hookState)) {
                return std::format("Failed to change state of item '{}'", entry);
            }
        } else {
            for (auto item : cat->Items()) {
                item->SetState(hookState);
            }
        }

        return std::nullopt;
    };

    const auto ProcessOptions = [&] (const std::vector<std::string_view>& options, HookState hookState) {
        for (const auto& entry : options) {
            if (const auto err = ProcessEntry(entry, hookState)) {
                NOTSA_LOG_WARN("Failed to process entry '{}': {}", entry, *err);
            } else {
                NOTSA_LOG_DEBUG("Set state to {} of '{}'", hookState, entry);
            }
        }
    };

    if (CommandLine::s_UnhookAll || !CommandLine::s_UnhookExcept.empty()) {
        NOTSA_LOG_DEBUG("Unhooked all via command-line");
        rh->GetRootCategory()->ForEachItem([] (const auto item) {
            item->SetState(HookState::RedirectToGTA);
        });
        ProcessOptions(s_UnhookExcept, HookState::RedirectToGTA);
    } else if (!CommandLine::s_UnhookSome.empty()) {
        ProcessOptions(s_UnhookSome, HookState::RedirectToGTA);
    }
#endif
}

void DumpHooks() {
    if (const auto& path = CommandLine::s_DumpHooksPath; !path.empty()) {
        ReversibleHooks::RHManager::GetInstance().WriteHooksToFile(path);
    } else {
#if defined(NOTSA_STANDALONE) && defined(NOTSA_DUMP_HOOKS_ONLY)
        NOTSA_LOG_ERR("No path provided for dumping hooks, use `--dump-hooks-to` CLI argument");
        exit(1);
#endif
    }
}

void PostHooksInjected() {
    ApplyHookSettings();
    DumpHooks();
}
}; // namespace CommandLine
