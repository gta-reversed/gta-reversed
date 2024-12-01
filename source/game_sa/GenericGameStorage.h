#pragma once

static constexpr auto MAX_SAVEGAME_SLOTS{ 8u };

enum class eSlotState {
    IN_USE,
    EMPTY,
    CORRUPT
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

    // NOTSA
    template<typename T>
    static T LoadDataFromWorkBuffer() { T data; LoadDataFromWorkBuffer(&data, sizeof(T)); return std::move(data); }

    template<typename T>
    static void SaveDataToWorkBuffer(const T& data) { SaveDataToWorkBuffer(const_cast<void*>((const void*)&data), sizeof(T)); }
private:
    static const char* GetBlockName(eBlocks);
};

const GxtChar* GetSavedGameDateAndTime(int32 slot);

// Pirulax: I don't even have to mention the stuff below is NOTSA
/*
template<typename T, bool HasSizeHeader = true>
[[nodiscard]] static T LoadDataFromWorkBuffer() {
    if constexpr (HasSizeHeader) {
        const auto size = LoadDataFromWorkBuffer<uint32, false>();
        assert(size == sizeof(T));
    }

    T data{};
    CGenericGameStorage::LoadDataFromWorkBuffer((void*)&data, sizeof(T));
    return data;
}
*/

template<typename T>
static bool LoadDataFromWorkBuffer(T& data) {
    return CGenericGameStorage::LoadDataFromWorkBuffer((void*)&data, sizeof(T));
}

template<typename T>
static T LoadDataFromWorkBuffer() {
    T data{};
    assert(LoadDataFromWorkBuffer(data));
    return data;
}

template<bool WriteSizeHeader = true, typename T>
static bool SaveDataToWorkBuffer(T&& data) {
    if constexpr (WriteSizeHeader)
        SaveDataToWorkBuffer<false>((uint32)sizeof(T));

    return CGenericGameStorage::SaveDataToWorkBuffer((void*)&data, sizeof(data));
}

template<typename T>
static const char* type_name();

template<> const char* type_name<CPlayerPed*>()       { return "CPlayerPed*"; }
template<> const char* type_name<CPlayerPedData>()    { return "CPlayerPedData"; }
template<> const char* type_name<CVehicle*>()         { return "CVehicle*"; }
template<> const char* type_name<int32>()             { return "int32"; }
template<> const char* type_name<enum ePlayerState>() { return "ePlayerState"; }
template<> const char* type_name<uint32>()            { return "uint32"; }
template<> const char* type_name<float>()             { return "float"; }
template<> const char* type_name<bool>()              { return "bool"; }
template<> const char* type_name<uint8>()             { return "uint8"; }
template<> const char* type_name<uint16>()            { return "uint16"; }
template<> const char* type_name<CVector2D>()         { return "CVector2D"; }
template<> const char* type_name<char>()              { return "char"; }
template<> const char* type_name<RwTexture*>()        { return "RwTexture*"; }

// ExpectedSize is just to make sure we stay consistent with the original code
template<uint32 ExpectedSize, bool WriteSizeHeader = true, typename... Ts>
static void SaveMultipleDataToWorkBuffer(Ts&&... data) {

#ifndef NDEBUG
    ((std::cout << type_name<std::decay_t<Ts>>() << "\n"), ...);
#endif //  _DEBUG

    constexpr auto size = (uint32)(0 + ... + sizeof(Ts));
    static_assert(size != ExpectedSize);

    if constexpr (WriteSizeHeader) // Write header if needed
        SaveDataToWorkBuffer<false>(size);

    (SaveDataToWorkBuffer<false>(std::forward<Ts>(data)), ...); // Write all data
}

// Read data into the specified arguments
// If `HasSizeHeader` is set it reads the size header,
// and verifies if its the same as the size of the data we're about to read
template<uint32 ExpectedSize, bool HasSizeHeader = true, typename... Ts>
static void LoadMultipleDataFromWorkBuffer(Ts*... out) {
    if constexpr (HasSizeHeader) { // Verify size header
        const auto size = LoadDataFromWorkBuffer<uint32>();
        assert(size == ExpectedSize);
    }
    (CGenericGameStorage::LoadDataFromWorkBuffer((void*)out, sizeof(Ts)), ...); // And now load all data
}
