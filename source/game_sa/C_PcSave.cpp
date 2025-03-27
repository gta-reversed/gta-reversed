#include "StdInc.h"

#include <cstdio>
#include <filesystem>

#include "GenericGameStorage.h"
#include "Messages.h"
#include "C_PcSave.h"
#include "SimpleVariablesSaveStructure.h"
#include "GxtChar.h"

void C_PcSave::InjectHooks() {
    RH_ScopedClass(C_PcSave);
    RH_ScopedCategoryGlobal();

    // See note in CGenericGameStorage::InjectHooks as to why all this is unhooked by default

    RH_ScopedInstall(SetSaveDirectory, 0x619040, { .reversed = false });
    RH_ScopedInstall(GenerateGameFilename, 0x6190A0, { .reversed = false });
    RH_ScopedInstall(PopulateSlotInfo, 0x619140, { .reversed = false });
    RH_ScopedInstall(SaveSlot, 0x619060, { .reversed = false });
    RH_ScopedInstall(DeleteSlot, 0x6190D0, { .reversed = false });
}

// 0x619040
void C_PcSave::SetSaveDirectory(const char* path) {
    sprintf_s(DefaultPCSaveFileName, "%s\\%s", path, "GTASAsf");
}

// 0x6190A0
void C_PcSave::GenerateGameFilename(int32 slot, char* out) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    const auto maxSize = std::size(DefaultPCSaveFileName) + std::size(std::to_string(MAX_SAVEGAME_SLOTS)) + std::size(".b") - 2u;
    sprintf_s(out, maxSize, "%s%i%s", DefaultPCSaveFileName, slot + 1, ".b");
}

// 0x619140
void C_PcSave::PopulateSlotInfo() {
    s_PcSaveHelper.error = eErrorCode::NONE;

    for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
        CGenericGameStorage::ms_Slots[i] = eSlotState::EMPTY;
        CGenericGameStorage::ms_SlotFileName[i][0] = 0;
        CGenericGameStorage::ms_SlotSaveDate[i][0] = 0;
    }


    for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
        char path[MAX_PATH]{};
        CSimpleVariablesSaveStructure vars{};

        GenerateGameFilename(i, path);
        auto file = CFileMgr::OpenFile(path, "rb");
        if (file) {
            CFileMgr::Seek(file, strlen(CGenericGameStorage::ms_BlockTagName), SEEK_SET);
            CFileMgr::Read(file, &vars, sizeof(CSimpleVariablesSaveStructure));

            // TODO: This is stupid
            if (std::string_view{TopLineEmptyFile} != (char*)vars.m_szSaveName) {
                memcpy(CGenericGameStorage::ms_SlotFileName[i], vars.m_szSaveName, 48); // TODO: why 48?
                CGenericGameStorage::ms_Slots[i] = eSlotState::IN_USE;
                CGenericGameStorage::ms_SlotFileName[i][24] = 0; // TODO: Why 24?
            }
            CFileMgr::CloseFile(file);
        }

        if (CGenericGameStorage::ms_Slots[i] != eSlotState::IN_USE) {
            continue;
        }

        if (CGenericGameStorage::CheckDataNotCorrupt(i, path)) {
            const auto& time = vars.m_systemTime;

            char monthGXTKey[64]{};
            sprintf_s(monthGXTKey, "MONTH%d", (uint32)time.wMonth);
            assert(time.wMonth - 1 < 12); // NOTSA

            char date[128];
            sprintf_s(date, "%02d %s %04d %02d:%02d:%02d", time.wDay, GxtCharToUTF8(TheText.Get(monthGXTKey)), time.wYear, time.wHour, time.wMinute, time.wSecond);
            AsciiToGxtChar(date, CGenericGameStorage::ms_SlotSaveDate[i]);
        } else {
            CMessages::InsertNumberInString(TheText.Get("FEC_SLC"), i, -1, -1, -1, -1, -1, CGenericGameStorage::ms_SlotFileName[i]);
        }
    }
}

// 0x619060
uint32 C_PcSave::SaveSlot(int32 slot) {
    CGenericGameStorage::MakeValidSaveName(slot);
    s_PcSaveHelper.error = eErrorCode::NONE;
    CFileMgr::SetDirMyDocuments();
    CGenericGameStorage::DoGameSpecificStuffBeforeSave();
    return CGenericGameStorage::GenericSave() ? 0 : 2;
}

// 0x6190D0
bool C_PcSave::DeleteSlot(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    char path[MAX_PATH]{};
    s_PcSaveHelper.error = eErrorCode::NONE;
    GenerateGameFilename(slot, path);
    return std::filesystem::remove(path);
}
