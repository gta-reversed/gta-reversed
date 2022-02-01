#pragma once

static constexpr auto MAX_SAVEGAME_SLOTS{ 8u };

class CGenericGameStorage {
    static constexpr auto BUFFER_SIZE{ (uint32)(50u * 1024u) };

    using tSlotSaveDate = char[70];
    using tSlotFileName = char[MAX_PATH];

    enum class eBlocks {
        SIMPLE_VARIABLES,
        SCRIPTS,
        POOLS,
        GARAGES,
        GAMELOGIC,
        PATHS,
        PICKUPS,
        PHONEINFO,
        RESTART,
        RADAR,
        ZONES,
        GANGS,
        CAR_GENERATORS,
        PED_GENERATORS,
        AUDIO_SCRIPT_OBJECT,
        PLAYERINFO,
        STATS,
        SET_PIECES,
        STREAMING,
        PED_TYPES,
        TAGS,
        IPLS,
        SHOPPING,
        GANGWARS,
        STUNTJUMPS,
        ENTRY_EXITS,
        RADIOTRACKS,
        USER3DMARKERS,

        TOTAL,
    };

    enum class eSaveLoadError {
        SYNC,
        LOADING,
        SAVING
    };

public:
    enum class eSlotState {
        IN_USE,
        EMPTY,
        CORRUPT
    };
        
public:
    static inline uint32& ms_WorkBufferSize = *(uint32*)0x8D2BE0;
    static inline char(&ms_SaveFileNameJustSaved)[MAX_PATH] = *(char(*)[MAX_PATH])0xC16030;
    static inline char(&ms_SaveFileName)[256] = *(char(*)[256])0xC16DB8;
    static inline char(&ms_LoadFileName)[104] = *(char(*)[104])0xC15FC8;
    static inline char(&ms_LoadFileNameWithPath)[104] = *(char(*)[104])0xC15F60;
    static inline uint32& ms_CheckSum = *(uint32*)0xC16134;
    static inline tSlotSaveDate(&ms_SlotSaveDate)[MAX_SAVEGAME_SLOTS] = *(tSlotSaveDate(*)[MAX_SAVEGAME_SLOTS])0xC16138;
    static inline tSlotFileName(&ms_SlotFileName)[MAX_SAVEGAME_SLOTS] = *(tSlotFileName(*)[MAX_SAVEGAME_SLOTS])0xC16368;
    static inline uint8*& ms_WorkBuffer = *(uint8**)0xC16EE8;
    static inline int32& ms_WorkBufferPos = *(int32*)0xC16EEC;
    static inline FILE*& ms_FileHandle = *(FILE**)0xC16EF0;
    static inline uint32& ms_FilePos = *(uint32*)0xC16EF4;
    static inline uint32& ms_FileSize = *(uint32*)0xC16EF8;
    static inline bool& ms_bFailed = *(bool*)0xC16EFC;
    static inline eSlotState(&ms_Slots)[MAX_SAVEGAME_SLOTS] = *(eSlotState(*)[MAX_SAVEGAME_SLOTS])0xC16EBC;
    static inline bool& ms_bLoading = *(bool*)0xC16EFD;
    static inline const char ms_BlockTagName[] =  "BLOCK" ;

public:
    static void InjectHooks();

    static void ReportError(eBlocks nBlock, eSaveLoadError nError);
    static void DoGameSpecificStuffBeforeSave();
    static void DoGameSpecificStuffAfterSucessLoad();
    static void InitRadioStationPositionList();
    static bool GenericLoad(bool& outbVariablesLoaded);
    static bool GenericSave();
    static bool CheckSlotDataValid(int32 slot);
    static bool LoadDataFromWorkBuffer(void* data, int32 size);
    static int32 SaveDataToWorkBuffer(void* data, int32 Size);
    static bool LoadWorkBuffer();
    static bool SaveWorkBuffer(bool bIncludeChecksum);
    static uint32 GetCurrentVersionNumber();
    static void MakeValidSaveName(int32 saveNum);
    static bool CloseFile();
    static bool OpenFileForWriting();
    static bool OpenFileForReading(const char* fileName, int32 slot);
    static bool CheckDataNotCorrupt(int32 slot, const char* fileName);
    static bool RestoreForStartLoad();

private:
    static const char* GetBlockName(eBlocks);
};

const char* GetSavedGameDateAndTime(int32 slot);
