#include "StdInc.h"

#include "Directory.h"

void CDirectory::InjectHooks() {
    RH_ScopedClass(CDirectory);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(Constructor, "Empty", 0x532290, CDirectory*(CDirectory::*)());
    RH_ScopedOverloadedInstall(Constructor, "Capacity", 0x5322A0, CDirectory*(CDirectory::*)(size_t));
    RH_ScopedInstall(Destructor, 0x5322D0);
    RH_ScopedInstall(Init, 0x5322F0);
    RH_ScopedOverloadedInstall(AddItem, "", 0x532310, void(CDirectory::*)(const DirectoryInfo&));
    RH_ScopedInstall(ReadDirFile, 0x532350);
    RH_ScopedInstall(WriteDirFile, 0x532410);
    RH_ScopedOverloadedInstall(FindItem, "", 0x532450, DirectoryInfo*(CDirectory::*)(const char*) const);
    RH_ScopedOverloadedInstall(FindItem, "ByName", 0x5324A0, bool(CDirectory::*)(const char*, CdStreamPos&, uint32&) const);
    RH_ScopedOverloadedInstall(FindItem, "ByHash", 0x5324D0, bool(CDirectory::*)(uint32, CdStreamPos&, uint32&) const);
}

CDirectory* CDirectory::Constructor() { this->CDirectory::CDirectory(); return this; }
CDirectory* CDirectory::Constructor(size_t capacity) { this->CDirectory::CDirectory(capacity); return this; }
CDirectory* CDirectory::Destructor() { this->CDirectory::~CDirectory(); return this; }

// 0x5322A0
CDirectory::CDirectory(size_t capacity) :
    m_nCapacity(capacity),
    m_pEntries(new DirectoryInfo[capacity]), // todo: Cppcheck: (warning) noCopyConstructor: Class 'CDirectory' does not have a copy constructor which is recommended since it has dynamic memory/resource allocation(s).
    m_bOwnsEntries(true)
{
    /* rest done by the compiler */
}

// 0x5322D0
CDirectory::~CDirectory() {
    if (m_pEntries && m_bOwnsEntries) {
        delete[] m_pEntries;
    }
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
#ifdef FIX_BUGS
        // don't add if already exists
        uint32 size;
        CdStreamPos pos;
        if (FindItem(dirInfo.Name, pos, size)) {
            return;
        }
#endif
        m_pEntries[m_nNumEntries++] = dirInfo;
    } else {
        NOTSA_LOG_DEBUG("Too many objects without modelinfo structures");
    }
}

// NOTSA
void CDirectory::AddItem(const DirectoryInfo& dirInfo, CdStreamID streamID) {
    DirectoryInfo di = dirInfo;
    di.Pos.FileID = streamID;
    AddItem(di);
}

// 0x532350
void CDirectory::ReadDirFile(const char* filename) {
    auto file = CFileMgr::OpenFile(filename, "rb");
    {
        /* Unused stuff */
        byte unused[4];
        CFileMgr::Read(file, &unused, sizeof(unused));
    }
    uint32 numEntries{};
    CFileMgr::Read(file, &numEntries, sizeof(numEntries));

    for (size_t i = 0; i < numEntries; i++) {
        DirectoryInfo info{};
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
CDirectory::DirectoryInfo* CDirectory::FindItem(const char* itemName) const {
    if (m_nNumEntries <= 0) {
        return nullptr;
    }
    for (DirectoryInfo* it = m_pEntries; it != m_pEntries + m_nNumEntries; it++) {
        if (_stricmp(it->Name, itemName) == 0) {
            return it;
        }
    }
    return nullptr;
}

// 0x5324A0
bool CDirectory::FindItem(const char* name, CdStreamPos& pos, uint32& outSize) const {
    if (DirectoryInfo* info = FindItem(name)) {
        pos     = info->Pos;
        outSize = info->Size;
        return true;
    }
    return false;
}

// 0x5324D0
bool CDirectory::FindItem(uint32 hashKey, CdStreamPos& pos, uint32& outSize) const {
    if (m_nNumEntries <= 0) {
        return false;
    }
    for (DirectoryInfo* it = m_pEntries; it != m_pEntries + m_nNumEntries; it++) {
        if (CKeyGen::GetUppercaseKey(it->Name) == hashKey) {
            pos     = it->Pos;
            outSize = it->Size;
            return true;
        }
    }
    return false;
}
