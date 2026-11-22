#pragma once

static constexpr auto MAX_SAVEGAME_SLOTS{ 8u };

enum class eSlotState {
    SLOT_FILLED,
    SLOT_FREE,
    SLOT_CORRUPTED,
    SLOT_DATA_OLD,
};

class CGenericGameStorage {
    static constexpr auto BUFFER_SIZE{ (uint32)(50u * 1024u) };

    using tSlotSaveDate = GxtChar[70];
    using tSlotFileName = GxtChar[MAX_PATH];

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
    static inline auto& ms_WorkBufferSize = StaticRef<uint32>(0x8D2BE0);
    static inline auto& ms_SaveFileNameJustSaved = StaticRef<char[MAX_PATH]>(0xC16030);
    static inline auto& ms_SaveFileName = StaticRef<char[256]>(0xC16DB8);
    static inline auto& ms_LoadFileName = StaticRef<char[104]>(0xC15FC8);
    static inline auto& ms_LoadFileNameWithPath = StaticRef<char[104]>(0xC15F60);
    static inline auto& ms_CheckSum = StaticRef<uint32>(0xC16134);
    static inline auto& ms_WorkBuffer = StaticRef<uint8*>(0xC16EE8);
    static inline auto& ms_WorkBufferPos = StaticRef<int32>(0xC16EEC);
    static inline auto& ms_FileHandle = StaticRef<FILE*>(0xC16EF0);
    static inline auto& ms_FilePos = StaticRef<uint32>(0xC16EF4);
    static inline auto& ms_FileSize = StaticRef<uint32>(0xC16EF8);
    static inline auto& ms_bFailed = StaticRef<bool>(0xC16EFC);
    static inline auto& ms_bLoading = StaticRef<bool>(0xC16EFD);
    static inline const char ms_BlockTagName[] = "BLOCK";
    static inline auto& ms_SlotSaveDate = StaticRef<std::array<tSlotSaveDate, MAX_SAVEGAME_SLOTS>>(0xC16138);
    static inline auto& ms_SlotFileName = StaticRef<std::array<tSlotFileName, MAX_SAVEGAME_SLOTS>>(0xC16368);
    static inline auto& ms_Slots = StaticRef<std::array<eSlotState, MAX_SAVEGAME_SLOTS>>(0xC16EBC);

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
    static bool SaveDataToWorkBuffer(void* data, int32 Size);
    static bool LoadWorkBuffer();
    static bool SaveWorkBuffer(bool bIncludeChecksum);
    static uint32 GetCurrentVersionNumber();
    static void MakeValidSaveName(int32 saveNum);
    static bool CloseFile();
    static bool OpenFileForWriting();
    static bool OpenFileForReading(const char* fileName, int32 slot);
    static bool CheckDataNotCorrupt(int32 slot, const char* fileName);
    static bool RestoreForStartLoad();

    // NOTSA
    template<typename T>
    static bool LoadDataFromWorkBuffer(T& data) { return LoadDataFromWorkBuffer((void*)&data, sizeof(T)); }

    template<typename T>
    static T LoadDataFromWorkBuffer() { T data; LoadDataFromWorkBuffer((void*)&data, sizeof(T)); return data; }

    template<typename T>
    static bool SaveDataToWorkBuffer(const T& data) { return SaveDataToWorkBuffer(const_cast<void*>((const void*)&data), sizeof(T)); }
private:
    static const char* GetBlockName(eBlocks);
};

const GxtChar* GetSavedGameDateAndTime(int32 slot);

 // Maybe NOTSA
 inline const GxtChar* GetNameOfSavedGame(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);
    return CGenericGameStorage::ms_SlotFileName[slot];
}

 // Maybe NOTSA
inline const eSlotState GetSavedGameState(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);
    return CGenericGameStorage::ms_Slots[slot];
}
