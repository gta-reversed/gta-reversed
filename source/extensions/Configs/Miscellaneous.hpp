#pragma once
#include <cassert>
#include "app_debug.h"
#include "AudioEngine.h"
#include "Common.h"
#include "FileMgr.h"
#include "GenericGameStorage.h"
#include "MenuManager.h"

#include "extensions/Configuration.hpp"

inline struct MiscConfig {
    INI_CONFIG_SECTION("Misc");

    std::optional<std::string> SaveDirectoryPath{};
    bool LoadSavesWithMismatchingVersion{};

    void Load() {
        STORE_INI_CONFIG_VALUE_OPT(SaveDirectoryPath);

        if (SaveDirectoryPath.has_value() && SaveDirectoryPath->size() >= 255) {
            SaveDirectoryPath->resize(255);
            NOTSA_LOG_WARN("Custom save directory path is too long! Truncated to: '{}'", *SaveDirectoryPath);
        }

        STORE_INI_CONFIG_VALUE(LoadSavesWithMismatchingVersion, false);
    }
} g_MiscConfig{};
