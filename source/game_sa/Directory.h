/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/*
    http://www.gtamodding.com/wiki/IMG_archive
*/

class CDirectory { // Also referred to as an img file
public:
    struct DirectoryInfo {
        uint32 m_nOffset;           // Offset (in sectors)
        uint16 m_nStreamingSize;    // Size (in sectors)
        uint16 m_nSizeInArchive;    // Size in archive (in sectors) (always 0 when read)
        char   m_szName[24];        // Name of the file with extension (null terminated).
    };

    DirectoryInfo* m_pEntries{};
    uint32         m_nCapacity{};
    uint32         m_nNumEntries{};
    bool           m_bOwnsEntries{};

public:
    CDirectory() = default; // 0x532290
    explicit CDirectory(size_t capacity);
    ~CDirectory();

    void Init(int32 capacity, DirectoryInfo* entries);
    void AddItem(const DirectoryInfo& dirInfo);
    void AddItem(const DirectoryInfo &dirInfo, int32 imgId);
    void ReadDirFile(const char* filename);
    bool WriteDirFile(const char* fileName);
    DirectoryInfo* FindItem(const char* itemName) const;
    bool FindItem(const char* name, uint32& outOffset, uint32& outStreamingSize) const;
    bool FindItem(uint32 hashKey, uint32& outOffset, uint32& outStreamingSize) const;

    // notsa
    bool HasLoaded() const { return m_nNumEntries != 0; }
    void Clear() { m_nNumEntries = 0; }
private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CDirectory* Destructor();
    CDirectory* Constructor();
    CDirectory* Constructor(size_t capacity);
};

VALIDATE_SIZE(CDirectory, 0x10);
VALIDATE_SIZE(CDirectory::DirectoryInfo, 0x20);
