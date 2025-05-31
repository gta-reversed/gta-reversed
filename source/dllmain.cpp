#include "StdInc.h"
#include "config.h"

#include "extensions/CommandLine.h"
#include "extensions/Configuration.hpp"
#include "reversiblehooks/RootHookCategory.h"

void InjectHooksMain(HMODULE hThisDLL);

void DisplayConsole()
{
    if (AllocConsole()) {
        FILESTREAM fs{};
        VERIFY(freopen_s(&fs, "CONIN$",  "r", stdin)  == NOERROR);
        VERIFY(freopen_s(&fs, "CONOUT$", "w", stdout) == NOERROR);
        VERIFY(freopen_s(&fs, "CONOUT$", "w", stderr) == NOERROR);
    }
}

void WaitForDebugger() {
    while (!::IsDebuggerPresent()) {
        printf("Debugger not present\n");
        ::Sleep(100);
    }
}

static constexpr auto DEFAULT_INI_FILENAME = "gta-reversed.ini";

#include "extensions/Configs/FastLoader.hpp"

void LoadConfigurations() {
    // Firstly load the INI into the memory.
    g_ConfigurationMgr.Load(DEFAULT_INI_FILENAME);

    // Then load all specific configurations.
    g_FastLoaderConfig.Load();
    // ...
}

static void ApplyCommandLineHookSettings() {
    using namespace ReversibleHooks;

    const auto ResultText = [](SetCatOrItemStateResult res) {
        switch (res) {
        case SetCatOrItemStateResult::NotFound: return "not found";
        case SetCatOrItemStateResult::Locked:   return "locked";
        case SetCatOrItemStateResult::Done:     return "done";
        default: NOTSA_UNREACHABLE();
        }
    };

    if (CommandLine::s_UnhookAll || !CommandLine::s_UnhookExcept.empty()) {
        GetRootCategory().SetAllItemsEnabled(false);

        NOTSA_LOG_DEBUG("Unhooked all via command-line");
        for (const auto& item : CommandLine::s_UnhookExcept) {
            const auto res = SetCategoryOrItemStateByPath(item, true);

            if (res == SetCatOrItemStateResult::Done) {
                NOTSA_LOG_DEBUG("Rehooked '{}' via command-line.", item);
            } else {
                NOTSA_LOG_WARN("Couldn't rehook '{}' via command-line: {}", item, ResultText(res));
            }
        }
        return;
    }

    if (!CommandLine::s_UnhookSome.empty()) {
        for (const auto& item : CommandLine::s_UnhookSome) {
            const auto res = SetCategoryOrItemStateByPath(item, false);

            if (res == SetCatOrItemStateResult::Done) {
                NOTSA_LOG_DEBUG("Unhooked '{}' via command-line.", item);
            } else {
                NOTSA_LOG_WARN("Couldn't unhook '{}' via command-line: {}", item, ResultText(res));
            }
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // Fail if RenderWare has already been started
        if (*(RwCamera**)0xC1703C)
        {
            MessageBox(NULL, "gta_reversed failed to load (RenderWare has already been started)", "Error", MB_ICONERROR | MB_OK);
            return FALSE;
        }

        std::setlocale(LC_ALL, "en_US.UTF-8");
        // Support UTF-8 IO for Windows Terminal. (or CMD if a supported font is used)
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);

        DisplayConsole();
        CommandLine::Load(__argc, __argv);

        if (CommandLine::waitForDebugger)
            WaitForDebugger();

        LoadConfigurations();

        InjectHooksMain(hModule);
        ApplyCommandLineHookSettings();
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
