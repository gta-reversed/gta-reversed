/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

/*
    http://www.gtamodding.com/wiki/IMG_archive
*/

class CDirectory {
public:
    struct DirectoryInfo {
        uint32_t m_nOffset;
        uint16_t m_nStreamingSize;
        uint16_t m_nSizeInArchive;
        char     m_szName[24];
    };

    DirectoryInfo* m_pEntries{};
    uint32_t       m_nCapacity{};
    uint32_t       m_nNumEntries{};
    bool           m_bOwnsEntries{};

public:
    CDirectory();
    CDirectory(size_t capacity);
    ~CDirectory();

    void Init(int32_t capacity, DirectoryInfo* entries);
    void AddItem(const DirectoryInfo& dirInfo);
    void ReadDirFile(const char* filename);
    bool WriteDirFile(const char* fileName);
    DirectoryInfo* FindItem(const char* itemName);
    bool FindItem(const char* name, uint32_t& outOffset, uint32_t& outStreamingSize);
    bool FindItem(uint32_t hashKey, uint32_t& outOffset, uint32_t& outStreamingSize);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CDirectory* Destructor();
    CDirectory* Constructor();
    CDirectory* Constructor(size_t capacity);
};

VALIDATE_SIZE(CDirectory, 0x10);
VALIDATE_SIZE(CDirectory::DirectoryInfo, 0x20);
