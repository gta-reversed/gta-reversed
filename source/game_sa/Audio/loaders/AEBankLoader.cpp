#include "StdInc.h"
#include "AEBankLoader.h"

void CAEBankLoader::InjectHooks() {
    RH_ScopedClass(CAEBankLoader);
    RH_ScopedCategory("Audio/Loaders");

    RH_ScopedInstall(Deconstructor, 0x4DFB20);
    RH_ScopedInstall(GetBankLookup, 0x4E01B0);
    RH_ScopedInstall(LoadBankLookupFile, 0x4DFBD0);
    RH_ScopedInstall(LoadBankSlotFile, 0x4E0590);
    RH_ScopedInstall(LoadSFXPakLookupFile, 0x4DFC70, {.reversed = true});
    RH_ScopedInstall(CalculateBankSlotsInfosOffsets, 0x4DFBA0);
}

// 0x4DFB20
CAEBankLoader::~CAEBankLoader() {
    if (m_IsInitialised) {
        CMemoryMgr::Free(m_Buffer);

        delete m_BankSlots;
        delete m_BankLkups;
        delete m_StreamHandles;
    }
}

// 0x4E01B0
CAEBankLookupItem* CAEBankLoader::GetBankLookup(uint16 bankId) {
    if (m_IsInitialised && bankId < m_BankLkupCnt)
        return &m_BankLkups[bankId];

    return nullptr;
}

// 0x4DFBD0
bool CAEBankLoader::LoadBankLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\BANKLKUP.DAT", "rb");
    if (!file)
        return false;

    if (const auto size = CFileMgr::GetTotalSize(file)) {
        m_BankLkupCnt = size / sizeof(CAEBankLookupItem);
        m_BankLkups = new CAEBankLookupItem[m_BankLkupCnt];

        if (CFileMgr::Read(file, m_BankLkups, size) != size) {
            delete m_BankLkups;
            m_BankLkups = nullptr;
        }

        CFileMgr::CloseFile(file);
        return m_BankLkups != nullptr;
    } else {
        CFileMgr::CloseFile(file);
    }

    return false;
}

// 0x4E0590
bool CAEBankLoader::LoadBankSlotFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\BANKSLOT.DAT", "rb");
    if (!file)
        return false;

    m_Buffer = nullptr; // NOTSA: for return check

    if (auto size = CFileMgr::GetTotalSize(file); size > 2) {
        CFileMgr::Read(file, &m_BankSlotCnt, 2u);
        m_BankSlots = new CAEBankSlot[m_BankSlotCnt];

        if (CFileMgr::Read(file, m_BankSlots, size - 2) == size - 2) {
            CalculateBankSlotsInfosOffsets();
            assert(m_BankSlotCnt > 0);
            const auto& lastSlot = m_BankSlots[m_BankSlotCnt - 1];
            m_BufferSize = lastSlot.Offset + lastSlot.NumBytes;
            m_Buffer = (uint8*)CMemoryMgr::Malloc(m_BufferSize);
        } else {
            delete m_BankSlots;
            m_BankSlots = nullptr;
        }
    }

    CFileMgr::CloseFile(file);
    return m_Buffer != nullptr;
}

// 0x4DFC70
bool CAEBankLoader::LoadSFXPakLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\PAKFILES.DAT", "rb");
    bool failed = true;
    if (!file)
        return failed;

    if (auto totalSize = CFileMgr::GetTotalSize(file)) {
        m_PakLkupCount = totalSize / sizeof(tPakLookup);
        m_PakLkups = new tPakLookup[m_PakLkupCount];

        if (CFileMgr::Read(file, m_PakLkups, totalSize) == totalSize) {
            m_StreamHandles = new int32[m_PakLkupCount];

            for (auto i = 0; i < m_PakLkupCount; i++) {
                // NOTSA: Originally a 128 char array allocated in the stack.
                m_StreamHandles[i] = CdStreamOpen(std::format("AUDIO\\SFX\\{}", m_PakLkups[i].BaseFilename).c_str());
            }
            failed = false;
        }

        delete m_PakLkups;
        m_PakLkups = nullptr;
    }
    
    CFileMgr::CloseFile(file);
    return !failed;
}

// 0x4DFBA0
void CAEBankLoader::CalculateBankSlotsInfosOffsets() {
    auto offset = 0u;
    for (auto& slot : std::span{m_BankSlots, m_BankSlotCnt}) {
        slot.Offset = offset;
        offset += slot.NumBytes;
    }
}
