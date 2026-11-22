#include "StdInc.h"

#include "ModelInfoAccelerator.h"

// 0x4C6A80
CModelInfoAccelerator::CModelInfoAccelerator() {
    Init();
}

// 0x4C6A80
void CModelInfoAccelerator::Init() {
    m_nNumIDs = 0;
    m_bFileRead = false;
    m_pIDs = nullptr;
    field_1A = false;
    m_szFilePath[0] = '\0';
}

// 0x4C6AA0
void CModelInfoAccelerator::AddModelInfoId(uint16 modelId) {
    m_pIDs[m_nNumIDs++] = modelId;
}

// 0x4C6AC0
uint16 CModelInfoAccelerator::GetNextModelInfoId() {
    return m_pIDs[m_nNumIDs++];
}

// 0x4C6AE0
void CModelInfoAccelerator::AllocModelInfoIds() {
    m_pIDs = new uint16[BUFFER_SIZE];
    memset(m_pIDs, 0, BUFFER_SIZE);
}

// 0x4C6B10
void CModelInfoAccelerator::FreeModelInfoIds() {
    delete[] m_pIDs;
    m_pIDs = nullptr;
}

// 0x4C6B30
void CModelInfoAccelerator::GetEntry(CBaseModelInfo** mi, int32* arg1, char* arg2) {
    // NOP
}

// 0x4C6C20
bool CModelInfoAccelerator::Begin(const char* filePath) {
    strcpy_s(m_szFilePath, filePath);
    if (field_1A)
        return field_1A;

    return GetModelInfoIdFile();
}

// 0x4C6B40
void CModelInfoAccelerator::End(const char* filePath) {
    // NOP
}

// 0x4C6B70
bool CModelInfoAccelerator::GetModelInfoIdFile() {
    auto file = CFileMgr::OpenFile(m_szFilePath, "rb");
    m_bFileRead = file != nullptr;

    AllocModelInfoIds();
    if (m_bFileRead) {
        CFileMgr::Read(file, m_pIDs, BUFFER_SIZE);
        CFileMgr::CloseFile(file);
    }

    return m_bFileRead;
}

// 0x4C6BD0
void CModelInfoAccelerator::EndOfLoadPhase() {
    if (!m_bFileRead) {
        auto file = CFileMgr::OpenFileForWriting(m_szFilePath);
        if (file) {
            CFileMgr::Write(file, m_pIDs, BUFFER_SIZE);
            CFileMgr::CloseFile(file);
        } else { // FIX_BUGS
            NOTSA_LOG_DEBUG("Failed to open file {} in CModelInfoAccelerator::EndOfLoadPhase", m_szFilePath);
            CFileMgr::CloseFile(file);
        }
    }
    FreeModelInfoIds();
}
