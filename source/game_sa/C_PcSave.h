#pragma once

// Helper class for CGenericGameStorage

#include "Base.h"

class C_PcSave {
public:
    // TODO: Seems like this is a bit-field, not an enum actually
    enum class eErrorCode {
        NONE = 0,

        FAILED_TO_OPEN = 4,
        FAILED_TO_READ = 5,
        FAILED_TO_CLOSE = 6,
        SLOT_INVALID = 7,
        FAILED_TO_WRITE = 11,
    };
    
public:
    // Eg.: C:/Users/user/Documents/GTA San Andreas User Files/GTASAsf
    static inline char (&DefaultPCSaveFileName)[MAX_PATH] = *(char (*)[MAX_PATH])0xC16F18;
    static inline const char* MarketingPCSaveFileName = "GTASAmf";

    static void InjectHooks();

    static void SetSaveDirectory(const char* path);
    static void GenerateGameFilename(int32 slot, char* out);
    static void PopulateSlotInfo();
    static uint32 SaveSlot(int32 slot);
    static bool DeleteSlot(int32 slot);

public:
    eErrorCode error;
};

static inline C_PcSave& s_PcSaveHelper = *(C_PcSave*)0xC17034;
