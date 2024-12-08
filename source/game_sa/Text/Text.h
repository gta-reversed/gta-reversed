/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "FileMgr.h"
#include "GxtChar.h"
#include "Data.h"
#include "KeyArray.h"
#include "MissionTextOffsets.h"

struct ChunkHeader {
    char  magic[4];
    int32 size;
};
VALIDATE_SIZE(ChunkHeader, 0x8);

enum eTextLangCode : char {
    ENGLISH = 'e',
    GERMAN  = 'g',
    ITALIAN = 'i',
    SPANISH = 's',
    FRENCH  = 'f',
};

class CText {
public:
    CText();
    ~CText();

    void Load(bool keepMissionPack);
    void Unload(bool unloadMissionData);

    /*!
     * @brief Get translated text by it's key
     *
     * @param key The text's key
     *
     * @return The text identified by the given key or the GXT error string.
     */
    [[nodiscard]] const GxtChar* Get(const char* key);
    void GetNameOfLoadedMissionText(char* outStr);

    void LoadMissionText(const char* mission);
    void LoadMissionPackText();

private:
    CKeyArray           m_MainKeyArray;
    CData               m_MainText;

    CKeyArray           m_MissionKeyArray;
    CData               m_MissionText;

public:
    eTextLangCode       m_nLangCode;
    bool                m_bIsMissionTextOffsetsLoaded;
    bool                m_bCdErrorLoaded;
    bool                m_bIsMissionPackLoaded;
    char                m_szMissionName[8];
    char                m_szCdErrorText[256];
    CMissionTextOffsets m_MissionTextOffsets;

private:
    bool ReadChunkHeader(ChunkHeader* header, FILESTREAM file, uint32* offset, uint8 unknown);
    [[nodiscard]] char GetUpperCase(const char c) const;

public:
    auto GetKeys() { return std::span{ m_MainKeyArray.m_data, m_MainKeyArray.m_size }; }
    auto GetMissionKeys() { return std::span{ m_MissionKeyArray.m_data, m_MissionKeyArray.m_size }; }

    // NOTSA
    auto& GetMissionName() const { return m_szMissionName; }

private:
    friend class TextDebugModule;
    friend void InjectHooksMain();
    static void InjectHooks();

    CText* Constructor();
    CText* Destructor();

};
VALIDATE_SIZE(CText, 0xA90);

static inline CText& TheText = *(CText*)0xC1B340;
