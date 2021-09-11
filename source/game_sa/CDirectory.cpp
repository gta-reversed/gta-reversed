#include "StdInc.h"

#include "CDirectory.h"


void CDirectory::InjectHooks() {
    ReversibleHooks::Install("CDirectory", "CDirectory_Empty", 0x532290, static_cast<CDirectory*(CDirectory::*)()>(&CDirectory::Constructor));
    ReversibleHooks::Install("CDirectory", "CDirectory_Capacity", 0x5322A0, static_cast<CDirectory*(CDirectory::*)(size_t)>(&CDirectory::Constructor));
    ReversibleHooks::Install("CDirectory", "~CDirectory", 0x5322D0, &CDirectory::Destructor); 
    ReversibleHooks::Install("CDirectory", "Init", 0x5322F0, &CDirectory::Init); 
    ReversibleHooks::Install("CDirectory", "AddItem", 0x532310, &CDirectory::AddItem); 
    ReversibleHooks::Install("CDirectory", "ReadDirFile", 0x532350, &CDirectory::ReadDirFile); 
    ReversibleHooks::Install("CDirectory", "WriteDirFile", 0x532410, &CDirectory::WriteDirFile); 
    ReversibleHooks::Install("CDirectory", "FindItem", 0x532450, static_cast<DirectoryInfo*(CDirectory::*)(const char*)>(&CDirectory::FindItem)); 
    ReversibleHooks::Install("CDirectory", "FindItem_ByName", 0x5324A0, static_cast<bool(CDirectory::*)(const char*, uint32&, uint32&)>(&CDirectory::FindItem));
    ReversibleHooks::Install("CDirectory", "FindItem_ByHash", 0x5324D0, static_cast<bool(CDirectory::*)(uint32, uint32&, uint32&)>(&CDirectory::FindItem));
}

// 0x532290
CDirectory::CDirectory() {
    /* done by the compiler */
}

// 0x532290
CDirectory* CDirectory::Constructor() {
    this->CDirectory::CDirectory();
    return this;
}

// 0x5322A0
CDirectory::CDirectory(size_t capacity) :
    m_nCapacity(capacity),
    m_pEntries(new DirectoryInfo[capacity]),
    m_bOwnsEntries(true)
{
    /* rest done by the compiler */
}

// 0x5322A0
CDirectory* CDirectory::Constructor(size_t capacity) {
    this->CDirectory::CDirectory(capacity);
    return this;
}

// 0x5322D0
CDirectory::~CDirectory() {
    if (m_pEntries && m_bOwnsEntries)
        delete[] m_pEntries;
}

// 0x5322D0
CDirectory* CDirectory::Destructor() {
    this->CDirectory::~CDirectory();
    return this;
}

// 0x5322F0
void CDirectory::Init(int32 capacity, DirectoryInfo* entries) {
    m_nCapacity = capacity;
    m_pEntries = entries;
    m_nNumEntries = 0;
    m_bOwnsEntries = false;
}

// 0x532310
void CDirectory::AddItem(const DirectoryInfo& dirInfo) {
    if (m_nNumEntries < m_nCapacity) {
        m_pEntries[m_nNumEntries++] = dirInfo;
    } else {
        printf("Too many objects without modelinfo structures\n");
    }
}

// 0x532350
void CDirectory::ReadDirFile(const char* filename) {
    auto file = CFileMgr::OpenFile(filename, "rb");
    {
        /* Unused stuff */
        byte unused[4];
        CFileMgr::Read(file, &unused, sizeof(unused));
    }
    uint32 nNumEntries{};
    CFileMgr::Read(file, &nNumEntries, sizeof(nNumEntries));

    for (size_t i = 0; i < nNumEntries; i++) {
        DirectoryInfo info;
        CFileMgr::Read(file, &info, sizeof(info));
        AddItem(info);
        /* Possible optimization: Read directly into m_pEntries */
    }
    CFileMgr::CloseFile(file);
}

// 0x532410
bool CDirectory::WriteDirFile(const char* fileName) {
    auto file = CFileMgr::OpenFileForWriting(fileName);
    const auto nNumBytesToWrite = sizeof(*m_pEntries) * m_nNumEntries;
    const auto nBytesWritten = CFileMgr::Write(file, m_pEntries, nNumBytesToWrite);
    CFileMgr::CloseFile(file);
    return nNumBytesToWrite == nBytesWritten;
}

// 0x532450
CDirectory::DirectoryInfo* CDirectory::FindItem(const char* itemName) {
    if (m_nNumEntries <= 0)
        return nullptr;

    for (DirectoryInfo* it = m_pEntries; it != m_pEntries + m_nNumEntries; it++) {
        if (_stricmp(it->m_szName, itemName) == 0)
            return it;
    }

    return nullptr;
}

// 0x5324A0
bool CDirectory::FindItem(const char* name, uint32& outOffset, uint32& outStreamingSize) {
    if (DirectoryInfo* pInfo = FindItem(name)) {
        outOffset = pInfo->m_nOffset;
        outStreamingSize = pInfo->m_nStreamingSize;
        return true;
    }
    return false;
}

// 0x5324D0
bool CDirectory::FindItem(uint32 hashKey, uint32& outOffset, uint32& outStreamingSize) {
    if (m_nNumEntries <= 0)
        return false;

    for (DirectoryInfo* it = m_pEntries; it != m_pEntries + m_nNumEntries; it++) {
        if (CKeyGen::GetUppercaseKey(it->m_szName) == hashKey) {
            outOffset = it->m_nOffset;
            outStreamingSize = it->m_nStreamingSize;
            return true;
        }
    }

    return false;
}
