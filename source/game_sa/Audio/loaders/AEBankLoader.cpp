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
    if (m_bInitialised) {
        CMemoryMgr::Free(m_pBuffer);

        delete m_paBankSlots;
        delete m_paBankLookups;
        delete m_paStreamHandles;
    }
}

// 0x4E01B0
CAEBankLookupItem* CAEBankLoader::GetBankLookup(uint16 bankId) {
    if (m_bInitialised && bankId < m_nBankLookupCount)
        return &m_paBankLookups[bankId];

    return nullptr;
}

// 0x4DFBD0
bool CAEBankLoader::LoadBankLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\BANKLKUP.DAT", "rb");
    if (!file)
        return false;

    if (const auto size = CFileMgr::GetTotalSize(file)) {
        m_nBankLookupCount = size / sizeof(CAEBankLookupItem);
        m_paBankLookups = new CAEBankLookupItem[m_nBankLookupCount];

        if (CFileMgr::Read(file, m_paBankLookups, size) != size) {
            delete m_paBankLookups;
            m_paBankLookups = nullptr;
        }

        CFileMgr::CloseFile(file);
        return m_paBankLookups != nullptr;
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

    m_pBuffer = nullptr; // NOTSA: for return check

    if (auto size = CFileMgr::GetTotalSize(file); size > 2) {
        CFileMgr::Read(file, &m_nBankSlotCount, 2u);
        m_paBankSlots = new CAEBankSlot[m_nBankSlotCount];

        if (CFileMgr::Read(file, m_paBankSlots, size - 2) == size - 2) {
            CalculateBankSlotsInfosOffsets();
            assert(m_nBankSlotCount > 0);
            const auto& lastSlot = m_paBankSlots[m_nBankSlotCount - 1];
            m_nBufferSize = lastSlot.m_nOffset + lastSlot.m_nSize;
            m_pBuffer = (uint8*)CMemoryMgr::Malloc(m_nBufferSize);
        } else {
            delete m_paBankSlots;
            m_paBankSlots = nullptr;
        }
    }

    CFileMgr::CloseFile(file);
    return m_pBuffer != nullptr;
}

// 0x4DFC70
bool CAEBankLoader::LoadSFXPakLookupFile() {
    const auto file = CFileMgr::OpenFile("AUDIO\\CONFIG\\PAKFILES.DAT", "rb");
    bool failed = true;
    if (!file)
        return failed;

    if (auto totalSize = CFileMgr::GetTotalSize(file)) {
        m_nPakLookupCount = totalSize / sizeof(tPakLookup);
        m_paPakLookups = new tPakLookup[m_nPakLookupCount];

        if (CFileMgr::Read(file, m_paPakLookups, totalSize) == totalSize) {
            m_paStreamHandles = new int32[m_nPakLookupCount];

            for (auto i = 0; i < m_nPakLookupCount; i++) {
                // NOTSA: Originally a 128 char array allocated in the stack.
                m_paStreamHandles[i] = CdStreamOpen(std::format("AUDIO\\SFX\\{}", m_paPakLookups[i].m_szName).c_str());
            }
            failed = false;
        }

        delete m_paPakLookups;
        m_paPakLookups = nullptr;
    }
    
    CFileMgr::CloseFile(file);
    return !failed;
}

// 0x4DFBA0
void CAEBankLoader::CalculateBankSlotsInfosOffsets() {
    auto offset = 0u;
    for (auto& slot : std::span{m_paBankSlots, m_nBankSlotCount}) {
        slot.m_nOffset = offset;
        offset += slot.m_nSize;
    }
}
