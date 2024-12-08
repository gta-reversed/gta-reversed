#include "StdInc.h"

#include "GenericGameStorage.h"
#include "SimpleVariablesSaveStructure.h"
#include "TheCarGenerators.h"
#include "PedType.h"
#include "C_PcSave.h"
#include "TagManager.h"
#include "Radar.h"
#include "StuntJumpManager.h"
#include "EntryExitManager.h"
#include "Rope.h"
#include "Ropes.h"
#include "TheScripts.h"
#include "Garages.h"

constexpr uint32 SIZE_OF_ONE_GAME_IN_BYTES = 202748;

void CGenericGameStorage::InjectHooks() {
    RH_ScopedClass(CGenericGameStorage);
    RH_ScopedCategoryGlobal();

    // Can't really test these yet. All mods I have interfere with it (WindowedMode and IMFast)
    // Also, these functions originally had the file pointer passed to them @ `ebp`
    // which is non-standard, so.. yeah, not really possible to reverse this garbage
    // until we reverse everything.

    RH_ScopedInstall(ReportError, 0x5D08C0);
    RH_ScopedInstall(DoGameSpecificStuffBeforeSave, 0x618F50, { .reversed = false });
    RH_ScopedInstall(DoGameSpecificStuffAfterSucessLoad, 0x618E90, { .reversed = false });
    RH_ScopedInstall(InitRadioStationPositionList, 0x618E70, { .reversed = false });
    RH_ScopedGlobalInstall(GetSavedGameDateAndTime, 0x618D00, { .reversed = false });
    RH_ScopedInstall(GenericLoad, 0x5D17B0, { .reversed = false });
    RH_ScopedInstall(GenericSave, 0x5D13E0, { .reversed = false });
    RH_ScopedInstall(CheckSlotDataValid, 0x5D1380, { .reversed = false });
    //RH_ScopedInstall(LoadDataFromWorkBuffer, 0x5D1300, { .reversed = false });
    //RH_ScopedInstall(SaveDataToWorkBuffer, 0x5D1270, { .reversed = false });
    RH_ScopedInstall(LoadWorkBuffer, 0x5D10B0, { .reversed = false });
    RH_ScopedInstall(SaveWorkBuffer, 0x5D0F80, { .reversed = false });
    RH_ScopedInstall(GetCurrentVersionNumber, 0x5D0F50, { .reversed = false });
    RH_ScopedInstall(MakeValidSaveName, 0x5D0E90, { .reversed = false });
    RH_ScopedInstall(CloseFile, 0x5D0E30, { .reversed = false });
    RH_ScopedInstall(OpenFileForWriting, 0x5D0DD0, { .reversed = false });
    RH_ScopedInstall(OpenFileForReading, 0x5D0D20, { .reversed = false });
    RH_ScopedInstall(CheckDataNotCorrupt, 0x5D1170, { .reversed = false });
    RH_ScopedInstall(RestoreForStartLoad, 0x619000, { .reversed = false });
}

// 0x5D08C0
void CGenericGameStorage::ReportError(eBlocks nBlock, eSaveLoadError nError) {
    char buffer[256]{};
    const auto GetErrorString = [nError] {
        switch (nError) {
        case eSaveLoadError::LOADING:
            return "Loading error: %s";
        case eSaveLoadError::SAVING:
            return "Saving error: %s";
        case eSaveLoadError::SYNC:
            return "Loading sync error: %s";
        default:
            return "Unknown error: %s";
        }
    };
    sprintf_s(buffer, GetErrorString(), GetBlockName(nBlock));

    // Yes, they don't do anything with `buffer`

#ifdef _DEBUG
    std::cerr << "[CGenericGameStorage]: " << buffer << std::endl; // NOTSA
#endif
}

// part from 0x5D08C0
const char* CGenericGameStorage::GetBlockName(eBlocks block) {
    switch (block) {
    case eBlocks::SIMPLE_VARIABLES:
        return "SIMPLE_VARIABLES";
    case eBlocks::SCRIPTS:
        return "SCRIPTS";
    case eBlocks::POOLS:
        return "POOLS";
    case eBlocks::GARAGES:
        return "GARAGES";
    case eBlocks::GAMELOGIC:
        return "GAMELOGIC";
    case eBlocks::PATHS:
        return "PATHS";
    case eBlocks::PICKUPS:
        return "PICKUPS";
    case eBlocks::PHONEINFO:
        return "PHONEINFO";
    case eBlocks::RESTART:
        return "RESTART";
    case eBlocks::RADAR:
        return "RADAR";
    case eBlocks::ZONES:
        return "ZONES";
    case eBlocks::GANGS:
        return "GANGS";
    case eBlocks::CAR_GENERATORS:
        return "CAR GENERATORS";
    case eBlocks::PED_GENERATORS:
        return "PED GENERATORS";
    case eBlocks::AUDIO_SCRIPT_OBJECT:
        return "AUDIO SCRIPT OBJECT";
    case eBlocks::PLAYERINFO:
        return "PLAYERINFO";
    case eBlocks::STATS:
        return "STATS";
    case eBlocks::SET_PIECES:
        return "SET PIECES";
    case eBlocks::STREAMING:
        return "STREAMING";
    case eBlocks::PED_TYPES:
        return "PED TYPES";
    case eBlocks::TAGS:
        return "TAGS";
    case eBlocks::IPLS:
        return "IPLS";
    case eBlocks::SHOPPING:
        return "SHOPPING";
    case eBlocks::GANGWARS:
        return "GANGWARS";
    case eBlocks::STUNTJUMPS:
        return "STUNTJUMPS";
    case eBlocks::ENTRY_EXITS:
        return "ENTRY EXITS";
    case eBlocks::RADIOTRACKS:
        return "RADIOTRACKS";
    case eBlocks::USER3DMARKERS:
        return "USER3DMARKERS";
    default:
        return "UNKNOWN";
    }
}

// 0x618F50
void CGenericGameStorage::DoGameSpecificStuffBeforeSave() {
    CRopes::Shutdown();
    CPickups::RemovePickupObjects();

    auto& pinfo = FindPlayerInfo(0);
    pinfo.m_pPed->m_fHealth = (float)std::min((uint8)200u, pinfo.m_nMaxHealth);

    CGangWars::EndGangWar(false);
    CStats::IncrementStat(eStats::STAT_SAFEHOUSE_VISITS, 1.f);
    CGameLogic::PassTime(6 * 60);
    FindPlayerInfo().m_nNumHoursDidntEat = 0;
    FindPlayerPed()->ResetSprintEnergy();
    FindPlayerPed()->SetWantedLevel(0);
    CGame::TidyUpMemory(true, false);
}

// 0x618E90
void CGenericGameStorage::DoGameSpecificStuffAfterSucessLoad() {
    TheText.Load(false);
    CCollision::SortOutCollisionAfterLoad();
    CStreaming::LoadSceneCollision(TheCamera.GetPosition());
    CStreaming::LoadScene(TheCamera.GetPosition());
    CStreaming::LoadAllRequestedModels(false);
    CGame::TidyUpMemory(true, false);
    JustLoadedDontFadeInYet = true;
    StillToFadeOut = true;
    TheCamera.Fade(0.f, eFadeFlag::FADE_IN);
    CTheScripts::Process();
    CTagManager::UpdateNumTagged();
    CClothes::RebuildPlayer(FindPlayerPed(0), false);
    CPopCycle::Update();
    CStreaming::RemoveInappropriatePedModels();
    CGangWars::ClearSpecificZonesToTriggerGangWar();
    CGangWars::bTrainingMission = false;
    CTheZones::FillZonesWithGangColours(CGangWars::bGangWarsActive);
}

// 0x618E70
void CGenericGameStorage::InitRadioStationPositionList() {
    // NOP
}

// 0x5D17B0
bool CGenericGameStorage::GenericLoad(bool& out_bVariablesLoaded) {
    out_bVariablesLoaded = false;

    ms_bFailed = false;
    ms_CheckSum = 0;
    CCheat::ResetCheats();
    if (!OpenFileForReading(nullptr, 0)) {
        return false;
    }
    ms_bLoading = true;

    CSimpleVariablesSaveStructure varsBackup{};

    for (auto block = 0u; block < (uint32)eBlocks::TOTAL; block++) {
        char header[std::size(ms_BlockTagName)]{};
        if (!LoadDataFromWorkBuffer(header, sizeof(header) - 1)) {
            CloseFile();
            return false;
        }

        if (std::string_view{header} != ms_BlockTagName) {
            if (block != 0) {
                ReportError((eBlocks)(block - 1), eSaveLoadError::LOADING);
                if (block == 1) {
                    uint32 version{};
                    varsBackup.Extract(version); // Restore state
                }
            }
            CloseFile();
            ms_bLoading = false;
            return false;
        }

        switch ((eBlocks)block) {
        case eBlocks::SIMPLE_VARIABLES: {
            varsBackup.Construct();

            CSimpleVariablesSaveStructure vars{};
            if (!LoadDataFromWorkBuffer(&vars, sizeof(vars))) {
                ms_bFailed = true;
                break;
            }

            uint32 varsVer{};
            vars.Extract(varsVer);
            if (GetCurrentVersionNumber() != varsVer) {
                fprintf(stderr, "[error] GenericGameStorage: Loading failed (wrong version number = 0x%08x)!", varsVer); // NOTSA
                varsBackup.Extract(varsVer); // Restore old state
                CloseFile();
                return false;
            }
            break;
        }
        case eBlocks::SCRIPTS:
            CTheScripts::Load();
            break;
        case eBlocks::POOLS:
            if (CPools::Load())
                CTheScripts::DoScriptSetupAfterPoolsHaveLoaded();
            break;
        case eBlocks::GARAGES:
            CGarages::Load();
            break;
        case eBlocks::GAMELOGIC:
            CGameLogic::Load();
            break;
        case eBlocks::PATHS:
            ThePaths.Load();
            break;
        case eBlocks::PICKUPS:
            CPickups::Load();
            break;
        case eBlocks::PHONEINFO: // Unused
             break;
        case eBlocks::RESTART:
            CRestart::Load();
            break;
        case eBlocks::RADAR:
            CRadar::Load();
            break;
        case eBlocks::ZONES:
            CTheZones::Load();
            break;
        case eBlocks::GANGS:
            CGangs::Load();
            break;
        case eBlocks::CAR_GENERATORS:
            CTheCarGenerators::Load();
            break;
        case eBlocks::PED_GENERATORS: // Unused
            break;
        case eBlocks::AUDIO_SCRIPT_OBJECT: // Unused
            break;
        case eBlocks::PLAYERINFO:
            FindPlayerInfo().Load();
            break;
        case eBlocks::STATS:
            CStats::Load();
            break;
        case eBlocks::SET_PIECES:
            CSetPieces::Load();
            break;
        case eBlocks::STREAMING:
            CStreaming::Load();
            break;
        case eBlocks::PED_TYPES:
            CPedType::Load();
            break;
        case eBlocks::TAGS:
            CTagManager::Load();
            break;
        case eBlocks::IPLS:
            CIplStore::Load();
            break;
        case eBlocks::SHOPPING:
            CShopping::Load();
            break;
        case eBlocks::GANGWARS:
            CGangWars::Load();
            break;
        case eBlocks::STUNTJUMPS:
            CStuntJumpManager::Load();
            break;
        case eBlocks::ENTRY_EXITS:
            CEntryExitManager::Load();
            break;
        case eBlocks::RADIOTRACKS:
            CAERadioTrackManager::Load();
            break;
        case eBlocks::USER3DMARKERS:
            C3dMarkers::LoadUser3dMarkers();
            break;
        default:
            assert(0 && "Invalid block"); // NOTSA
            break;
        }

        if (ms_bFailed) {
            ReportError(eBlocks(block), eSaveLoadError::LOADING);
            CloseFile();
            ms_bLoading = false;
            return false;
        }
    }

    ms_bLoading = false;
    if (!CloseFile()) {
        return false;
    }

    DoGameSpecificStuffAfterSucessLoad();

    return true;
}

// 0x5D13E0
bool CGenericGameStorage::GenericSave() {
    ms_bFailed = false;
    if (!OpenFileForWriting()) {
        return false;
    }

    ms_CheckSum = {};

    for (auto block = 0u; block < (uint32)eBlocks::TOTAL; block++) {
        if (!SaveDataToWorkBuffer((void*)ms_BlockTagName, strlen(ms_BlockTagName))) {
            CloseFile();
            return false;
        }

        switch ((eBlocks)block) {
        case eBlocks::SIMPLE_VARIABLES: {
            CSimpleVariablesSaveStructure vars{};
            vars.Construct();
            ms_bFailed = !SaveDataToWorkBuffer((void*)&vars, sizeof(vars));
            break;
        }
        case eBlocks::SCRIPTS:
            CTheScripts::Save();
            break;
        case eBlocks::POOLS:
            CPools::Save();
            break;
        case eBlocks::GARAGES:
            CGarages::Save();
            break;
        case eBlocks::GAMELOGIC:
            CGameLogic::Save();
            break;
        case eBlocks::PATHS:
            ThePaths.Save();
            break;
        case eBlocks::PICKUPS:
            CPickups::Save();
            break;
        case eBlocks::PHONEINFO: // Unused
            break;
        case eBlocks::RESTART:
            CRestart::Save();
            break;
        case eBlocks::RADAR:
            CRadar::Save();
            break;
        case eBlocks::ZONES:
            CTheZones::Save();
            break;
        case eBlocks::GANGS:
            CGangs::Save();
            break;
        case eBlocks::CAR_GENERATORS:
            CTheCarGenerators::Save();
            break;
        case eBlocks::PED_GENERATORS: // Unused
            break;
        case eBlocks::AUDIO_SCRIPT_OBJECT: // Unused
            break;
        case eBlocks::PLAYERINFO:
            FindPlayerInfo().Save();
            break;
        case eBlocks::STATS:
            CStats::Save();
            break;
        case eBlocks::SET_PIECES:
            CSetPieces::Save();
            break;
        case eBlocks::STREAMING:
            CStreaming::Save();
            break;
        case eBlocks::PED_TYPES:
            CPedType::Save();
            break;
        case eBlocks::TAGS:
            CTagManager::Save();
            break;
        case eBlocks::IPLS:
            CIplStore::Save();
            break;
        case eBlocks::SHOPPING:
            CShopping::Save();
            break;
        case eBlocks::GANGWARS:
            CGangWars::Save();
            break;
        case eBlocks::STUNTJUMPS:
            CStuntJumpManager::Save();
            break;
        case eBlocks::ENTRY_EXITS:
            CEntryExitManager::Save();
            break;
        case eBlocks::RADIOTRACKS:
            CAERadioTrackManager::Save();
            break;
        case eBlocks::USER3DMARKERS:
            C3dMarkers::SaveUser3dMarkers();
            break;
        default:
            assert(0 && "Invalid block"); // NOTSA
            break;
        }
    }

    while (ms_WorkBufferPos + ms_FilePos < SIZE_OF_ONE_GAME_IN_BYTES && (SIZE_OF_ONE_GAME_IN_BYTES - ms_FilePos) >= BUFFER_SIZE) {
        ms_WorkBufferPos = BUFFER_SIZE;
        if (!SaveWorkBuffer(false)) {
            CloseFile();
            return false;
        }
    }

    if (SaveWorkBuffer(true)) {
        strncpy_s(ms_SaveFileNameJustSaved, ms_SaveFileName, std::size(ms_SaveFileNameJustSaved) - 1);
        if (CloseFile()) {
            CPad::UpdatePads();
            return true;
        }
        return false;
    }
    CloseFile();
    return true;
}

// 0x5D1380
bool CGenericGameStorage::CheckSlotDataValid(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    char fileName[MAX_PATH]{};
    s_PcSaveHelper.GenerateGameFilename(slot, fileName);

    if (CheckDataNotCorrupt(slot, fileName)) {
        CStreaming::DeleteAllRwObjects();
        return true;
    } else {
        s_PcSaveHelper.error = C_PcSave::eErrorCode::SLOT_INVALID;
        return false;
    }
}

// 0x5D1300
bool CGenericGameStorage::LoadDataFromWorkBuffer(void* data, int32 size) {
    assert(data);

    if (ms_bFailed)
        return false;

    if (size <= 0)
        return true;

    auto pos = ms_WorkBufferPos;

    if (static_cast<uint32>(size + pos) > ms_WorkBufferSize) {
        const auto maxSize = BUFFER_SIZE - pos;
        if (LoadDataFromWorkBuffer(data, maxSize)) {
            if (LoadWorkBuffer()) {
                pos = ms_WorkBufferPos;
                data = (uint8*)data + maxSize;
                size -= maxSize;
            }
        }
    }

    assert(ms_WorkBuffer);

    memcpy(data, ms_WorkBuffer + pos, size);
    ms_WorkBufferPos += size;

    return true;

}

// 0x5D1270
int32 CGenericGameStorage::SaveDataToWorkBuffer(void* data, int32 size) {
    assert(data);

    if (ms_bFailed)
        return false;

    if (size <= 0)
        return true;

    if (ms_WorkBufferPos + size > BUFFER_SIZE) {
        // Make space for data

        const auto buffSizeRemaining = BUFFER_SIZE - ms_WorkBufferPos;
        if (!SaveDataToWorkBuffer(data, buffSizeRemaining)) { // Try writing what we have space for
            return false;
        }
        if (!SaveWorkBuffer(false)) { // Flush data to file
            return false;
        }

        // Adjust stuff, and write remaining data (if any) to work buffer
        data = reinterpret_cast<uint8*>(data) + buffSizeRemaining;
        size -= buffSizeRemaining;
    }

    memcpy(&ms_WorkBuffer[ms_WorkBufferPos], data, size);
    ms_WorkBufferPos += size;

    return true;
}

// 0x5D10B0
bool CGenericGameStorage::LoadWorkBuffer() {
    if (ms_bFailed)
        return false;

    uint32 toReadSize = BUFFER_SIZE;
    if (ms_FilePos + BUFFER_SIZE > ms_FileSize) {
        toReadSize = ms_FileSize - ms_FilePos;
        if (ms_FileSize == ms_FilePos) {
            return false;
        } else {
            if (toReadSize != ((toReadSize + 3) & 0xFFFFFFFC)) { // Not sure, I think it's a check if the read size is 4 byte aligned?
                return false;
            }
        }
    }

    assert(ms_FileHandle);
    assert(ms_WorkBuffer);

    if (!CFileMgr::GetErrorReadWrite(ms_FileHandle)) {
        if (CFileMgr::Read(ms_FileHandle, ms_WorkBuffer, toReadSize) == toReadSize) {
            ms_FilePos += toReadSize;
            ms_WorkBufferSize = toReadSize;
            ms_WorkBufferPos = 0;
            return true;
        }
    }

    s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_READ;
    if (!CloseFile()) {
        s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_CLOSE;
    }

    ms_bFailed = true;

    return false;
}

// 0x5D0F80
bool CGenericGameStorage::SaveWorkBuffer(bool bIncludeChecksum) {
    if (ms_bFailed)
        return false;

    if (ms_WorkBufferPos == 0)
        return true;

    for (auto i = 0; i < ms_WorkBufferPos; ++i) {
        ms_CheckSum += ms_WorkBuffer[i];
    }

    if (bIncludeChecksum) {
        if (ms_WorkBufferPos > BUFFER_SIZE - sizeof(uint32))
            SaveWorkBuffer(false);
        memcpy(&ms_WorkBuffer[ms_WorkBufferPos], &ms_CheckSum, sizeof(uint32));
        ms_WorkBufferPos += sizeof(uint32);
    }

    if (CFileMgr::Write(ms_FileHandle, ms_WorkBuffer, ms_WorkBufferPos) == ms_WorkBufferPos) {
        if (!CFileMgr::GetErrorReadWrite(ms_FileHandle)) {
            ms_FilePos += ms_WorkBufferPos;
            ms_WorkBufferPos = 0;
            return true;
        }
    }

    s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_WRITE;
    if (!CloseFile())
        s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_CLOSE;

    strncpy_s(ms_SaveFileNameJustSaved, ms_SaveFileName, std::size(ms_SaveFileNameJustSaved) - 1);

    ms_bFailed = true;
    return false;
}

// 0x5D0F50
uint32 CGenericGameStorage::GetCurrentVersionNumber() {
    char buffer[40]{};
    sprintf_s(buffer, "%s%s", "Apr 28 2005", "10:28:55");
    return CKeyGen::GetKey(buffer);
}

// 0x5D0E90
void CGenericGameStorage::MakeValidSaveName(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    char path[MAX_PATH]{};
    s_PcSaveHelper.GenerateGameFilename(slot, path);

    path[257] = 0; // Make sure there's space for the file extension

    strcat_s(path, ".b");

    for (auto it = path; *it && *it != '\n'; it++) {
        if (*it == '?')
            *it = ' ';
    }

    strcpy_s(ms_SaveFileName, path);
}

// 0x5D0E30
bool CGenericGameStorage::CloseFile() {
    if (ms_WorkBuffer) {
        delete[] ms_WorkBuffer;
        ms_WorkBuffer = nullptr;
    }
    return CFileMgr::CloseFile(ms_FileHandle) == 0;
}

// 0x5D0DD0
bool CGenericGameStorage::OpenFileForWriting() {
    ms_FileHandle = CFileMgr::OpenFile(ms_SaveFileName, "wb");
    if (ms_FileHandle) {
        ms_FilePos = 0;
        ms_WorkBufferPos = 0;
        if (!ms_WorkBuffer)
            ms_WorkBuffer = new uint8[BUFFER_SIZE + 1];
        return true;
    } else {
        s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_OPEN;
        return false;
    }
}

// 0x5D0D20
bool CGenericGameStorage::OpenFileForReading(const char* fileName, int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    if (fileName) {
        strcpy_s(ms_LoadFileName, fileName);
        s_PcSaveHelper.GenerateGameFilename(slot, ms_LoadFileNameWithPath);
    }

    ms_FileHandle = CFileMgr::OpenFile(ms_LoadFileName, "rb");

    if (ms_FileHandle) {
        ms_FileSize = CFileMgr::GetTotalSize(ms_FileHandle);
        ms_FilePos = 0;
        ms_WorkBufferSize = BUFFER_SIZE;
        ms_WorkBufferPos = BUFFER_SIZE;

        if (!ms_WorkBuffer)
            ms_WorkBuffer = new uint8[BUFFER_SIZE + 1];

        return true;
    }

    s_PcSaveHelper.error = C_PcSave::eErrorCode::FAILED_TO_OPEN;

    return false;
}

// 0x5D1170
bool CGenericGameStorage::CheckDataNotCorrupt(int32 slot, const char* fileName) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    ms_bFailed = false;
    if (!OpenFileForReading(fileName, slot)) {
        return false;
    }

    uint32 nCheckSum = 0;

    while (LoadWorkBuffer()) {
         for (auto i = 0; i != ms_WorkBufferSize; ++i) {
            nCheckSum += ms_WorkBuffer[i];
        }
    }

    if (ms_WorkBufferSize < sizeof(uint32) || s_PcSaveHelper.error != C_PcSave::eErrorCode::NONE) {
        CloseFile();
        return false;
    }

    const auto nFileCheckSum = *(uint32*)(&ms_WorkBuffer[ms_WorkBufferSize - sizeof(uint32)]);

    for (auto i = 0; i < sizeof(uint32); ++i) {
        nCheckSum -= ms_WorkBuffer[ms_WorkBufferSize - (i + 1)];
    }

    CloseFile();

    return nCheckSum == nFileCheckSum;
}

// 0x619000
bool CGenericGameStorage::RestoreForStartLoad() {
    return false;
}

// 0x618D00
const GxtChar* GetSavedGameDateAndTime(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);
    return CGenericGameStorage::ms_SlotSaveDate[slot];
}
