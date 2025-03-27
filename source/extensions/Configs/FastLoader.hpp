#pragma once
#include <cassert>
#include "app_debug.h"
#include "AudioEngine.h"
#include "Common.h"
#include "FileMgr.h"
#include "GenericGameStorage.h"
#include "MenuManager.h"

#include "extensions/Configuration.hpp"

inline struct FastLoaderConfig {
    bool TriedLoadingSaveGame = false; //< [Runtime Var] Whenever `SaveGameToLoad` was tried to be loaded

    INI_CONFIG_SECTION("FastLoader");

    int32  SaveGameToLoad       = -2;         //< -2 - Don't load, -1 = Load first available, 0 = unused, 1 <= - load from save slot
    uint32 SkipSaveGameLoadKey  = VK_CONTROL; //< Skip auto-loading save game (If enabled)
    uint32 SoundDelay           = 50;

    bool  NoEAX            = false; //< Skip EAX splash
    bool  NoNVidia         = false; //< Skip nVidia splash
    bool  NoLogo           = false; //< Skip Logo.mpg
    bool  NoTitleOrIntro   = false; //< Skip GTAtitles.mpg
    bool  NoCopyright      = false; //< Skip Copyright screen
    bool  NoFading         = false; //< Skip fading (takes quite a bit of time)
    bool  NoLoadScreen     = false; //< Skip load pre-game screen 
    bool  NoLoadBar        = false; //< Skip load bar in pre-game load screen
    bool  NoLoadingTune    = false; //< Skip GTA theme music
    bool  NoDbgLogScreens  = true;  //< Don't display [notsa] loading screen debug messages to console
    bool  RenderAtAllTimes = true;  //< Render even when minimized
    float ScreenChangeTime = 5.f;   //< Time to change splash screens in the loading screen (seconds?)

    //! If there's a game to load, but not loaded yet...
    bool ShouldLoadSaveGame() const { return SaveGameToLoad >= -1 && !TriedLoadingSaveGame; }

    //! Start the game from a slot
    //! `slot` can have the same values as `SaveGameToLoad`
    bool StartGame(int32 slot) {
        const auto CheckIfSaveFileExists = [](int32 slot) {
            CFileMgr::SetDirMyDocuments();
            const bool exists = std::filesystem::exists(std::format("GTASAsf{}.b", slot));
            CFileMgr::ChangeDir("");
            return exists;
        };

        if (slot == -1) { // Find first valid slot and load that
            for (auto i = 1u; i <= MAX_SAVEGAME_SLOTS; i++) {
                if (CheckIfSaveFileExists(i)) {
                    return StartGame(i); // Load this slot
                }
            }
            NOTSA_LOG_TRACE("Couldn't find any savegame to load!");
            return false;
        }

        // Load game from slot
        assert(slot > 0);
        if (!CheckIfSaveFileExists(slot)) {
            NOTSA_LOG_WARN("Save slot {} is empty!", slot);
            return false;
        }
        NOTSA_LOG_TRACE("Loading game from slot {}", slot);

        if (!NoLoadingTune) {
            for (size_t i = 0; i < SoundDelay; i++) {
                AudioEngine.ServiceLoadingTune(1.f);
            }
        }

        // Actually load the game (By simulating the user going in the menu and doing it)
        FrontEndMenuManager.SimulateGameLoad(false, slot - 1);

        return true;
    }

    void Load() {
#ifdef EXT_FAST_LOADER
        if (!GET_INI_CONFIG_VALUE("Enable", true)) {
            return;
        }

        STORE_INI_CONFIG_VALUE(SaveGameToLoad, -1);
        STORE_INI_CONFIG_VALUE(SkipSaveGameLoadKey, VK_CONTROL);
        STORE_INI_CONFIG_VALUE(SoundDelay, 50);

        STORE_INI_CONFIG_VALUE(NoEAX, true);
        STORE_INI_CONFIG_VALUE(NoNVidia, true);
        STORE_INI_CONFIG_VALUE(NoLogo, true);
        STORE_INI_CONFIG_VALUE(NoTitleOrIntro, true);
        STORE_INI_CONFIG_VALUE(NoCopyright, true);
        STORE_INI_CONFIG_VALUE(NoFading, true);
        STORE_INI_CONFIG_VALUE(NoLoadScreen, true);
        STORE_INI_CONFIG_VALUE(NoLoadBar, false);
        STORE_INI_CONFIG_VALUE(NoLoadingTune, true);
        STORE_INI_CONFIG_VALUE(NoDbgLogScreens, true);
        STORE_INI_CONFIG_VALUE(RenderAtAllTimes, true);
        STORE_INI_CONFIG_VALUE(ScreenChangeTime, 5.f);
#endif
    }
} g_FastLoaderConfig{};
