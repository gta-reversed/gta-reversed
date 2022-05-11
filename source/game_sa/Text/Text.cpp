#include "StdInc.h"

#include "GxtChar.h"

#include "Text.h"
#include "Data.h"
#include "KeyArray.h"
#include "MissionTextOffsets.h"

#include "eLanguage.h"

constexpr auto CHUNK_TKEY = "TKEY";
constexpr auto CHUNK_TDAT = "TDAT";
constexpr auto CHUNK_TABL = "TABL";
constexpr auto GAME_ENCODING = (sizeof(GxtChar) << 3);

static constexpr uint8 UpperCaseTable[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x41, 0x41, 0x41, 0x41, 0x84, 0x85, 0x45, 0x45, 0x45,
    0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xAD, 0xAD, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static constexpr uint8 FrenchUpperCaseTable[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
    0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0xAD, 0xAD, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

void CText::InjectHooks() {
    RH_ScopedClass(CText);
    RH_ScopedCategory("Text");

    RH_ScopedInstall(Constructor, 0x6A00F0);
    RH_ScopedInstall(Destructor, 0x6A0140);
    RH_ScopedInstall(Get, 0x6A0050);
    RH_ScopedInstall(GetNameOfLoadedMissionText, 0x69FBD0);
    RH_ScopedInstall(ReadChunkHeader, 0x69F940);
    RH_ScopedInstall(LoadMissionPackText, 0x69F9A0);
    // RH_ScopedInstall(LoadMissionText, 0x69FBF0);
    RH_ScopedInstall(Load, 0x6A01A0);
    RH_ScopedInstall(Unload, 0x69FF20);

    {
        RH_ScopedClass(CMissionTextOffsets);
        RH_ScopedInstall(Load, 0x69F670);
    }

    {
        RH_ScopedClass(CData);
        RH_ScopedInstall(Unload, 0x69F640);
        RH_ScopedInstall(Load, 0x69F5D0);
    }

    {
        RH_ScopedClass(CKeyArray);
        RH_ScopedCategory("Text");
        RH_ScopedInstall(Unload, 0x69F510);
        RH_ScopedInstall(BinarySearch, 0x69F570);
        RH_ScopedInstall(Search, 0x6A0000);
        RH_ScopedInstall(Load, 0x69F490);
    }
}

// 0x6A00F0
CText::CText() {
    m_MissionText.Unload();
    m_MissionKeyArray.Unload();
    m_MainText.Unload();
    m_MainKeyArray.Unload();

    m_bIsMissionPackLoaded = false;
    m_bIsMissionTextOffsetsLoaded = false;
    m_nLangCode = 'e'; // english

    m_szCdErrorText[0] = '\0';
    m_szMissionName[0] = '\0';
    GxtErrorString[0] = '\0';
}

// 0x6A0140
CText::~CText() {
    m_MissionText.Unload();
    m_MissionKeyArray.Unload();
    m_MainText.Unload();
    m_MainKeyArray.Unload();
}

// Unloads GXT file
// 0x69FF20
void CText::Unload(bool bUnloadMissionData) {
    CMessages::ClearAllMessagesDisplayedByGame(false);

    m_MainKeyArray.Unload();
    m_MainText.Unload();
    m_bCdErrorLoaded = false;
    if (!bUnloadMissionData) {
        m_MissionKeyArray.Unload();
        m_MissionText.Unload();

        m_bIsMissionPackLoaded = false;
        m_szMissionName[0] = '\0';
    }
}

// NOTSA
const char* GetGxtName() {
    switch (FrontEndMenuManager.m_nPrefsLanguage) {
    case eLanguage::AMERICAN:    return "AMERICAN.GXT";
    case eLanguage::FRENCH:      return "FRENCH.GXT";
    case eLanguage::GERMAN:      return "GERMAN.GXT";
    case eLanguage::ITALIAN:     return "ITALIAN.GXT";
    case eLanguage::SPANISH:     return "SPANISH.GXT";
#ifdef MORE_LANGUAGES
    case eLanguage::RUSSIAN:     return "RUSSIAN.GXT";
    case eLanguage::JAPANESE:    return "JAPANESE.GXT";
#endif
    }
}

// Loads GXT file
// 0x6A01A0
void CText::Load(bool bKeepMissionPack) {
    m_bIsMissionTextOffsetsLoaded = false;
    Unload(bKeepMissionPack);

    CFileMgr::SetDir("TEXT");
    auto file = CFileMgr::OpenFile(GetGxtName(), "rb");

    uint16 version = 0;
    uint16 encoding = 0;
    CFileMgr::Read(file, &version, sizeof(version));
    CFileMgr::Read(file, &encoding, sizeof(encoding));

    uint32 offset = sizeof(uint16) * 2; // skip version and encoding
    bool bTKEY = false;
    bool bTDAT = false;
    ChunkHeader header{};
    while (!bTKEY || !bTDAT) {
        ReadChunkHeader(&header, file, &offset, false);
        if (header.size == 0)
            continue;

        if (strncmp(header.magic, CHUNK_TABL, sizeof(header.magic)) == 0) {
            m_MissionTextOffsets.Load(header.size, file, &offset, 0x58000); // todo: magic. Android have different value 0x64000
            m_bIsMissionTextOffsetsLoaded = true;
            continue;
        }

        if (strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic)) == 0) {
            m_MainKeyArray.Load(header.size, file, &offset, false);
            bTKEY = true;
            continue;
        }

        if (strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic)) == 0) {
            m_MainText.Load(header.size, file, &offset, false);
            bTDAT = true;
            continue;
        }

        CFileMgr::Seek(file, header.size, SEEK_CUR);
        offset += header.size;
    }

    m_MainKeyArray.Update(m_MainText.m_data);
    CFileMgr::CloseFile(file);

    const auto text = Get("CDERROR");
    strcpy(m_szCdErrorText, GxtCharToAscii(text, 0));
    m_bCdErrorLoaded = true;

    CFileMgr::SetDir("");

    if (bKeepMissionPack)
        return;

    while (!m_bIsMissionPackLoaded && CGame::bMissionPackGame) {
        if (FrontEndMenuManager.CheckMissionPackValidMenu()) {
            CTimer::Suspend();
            LoadMissionPackText();
            CFileMgr::SetDir("");
            CTimer::Resume();
        }
    }
}

// 0x69FBF0
void CText::LoadMissionText(const char* mission) {
    // plugin::CallMethod<0x69FBF0, CText*, char*>(this, mission);

    if (CGame::bMissionPackGame != 0)
        return;

    CMessages::ClearAllMessagesDisplayedByGame(true);

    m_MissionKeyArray.Unload();
    m_MissionText.Unload();

    m_bIsMissionPackLoaded = false;
    std::memset(m_szMissionName, '\0', sizeof(m_szMissionName));

    if (m_MissionTextOffsets.GetSize() == 0) {
        return;
    }

    auto v21 = 0;
    auto offsetId = 0;
    while (!v21) {
        if (strlen(mission) == strlen(m_MissionTextOffsets.GetTextOffset(offsetId).szMissionName) &&
            !strncmp(m_MissionTextOffsets.GetTextOffset(offsetId).szMissionName, mission, strlen(mission))
        ) {
            v21 = 1;
        } else {
            ++offsetId;
        }

        if (offsetId >= m_MissionTextOffsets.GetSize()) {
            if (!v21)
                return;
            break;
        }
    }

    CFileMgr::SetDir("TEXT");

    CTimer::Suspend();

    auto textOffset = m_MissionTextOffsets.GetTextOffset(offsetId);
    auto file = CFileMgr::OpenFile(GetGxtName(), "rb");
    CFileMgr::Seek(file, textOffset.offset, 0);

    char buf[8];
    CFileMgr::Read(file, buf, sizeof(buf)); // OG: CFileMgr::Read(file, buf++, 1u);
    strncmp(buf, mission, sizeof(buf));     // ?

    uint32 offset = sizeof(uint16) * 2; // skip version and encoding
    auto missionKeyArrayLoaded = false;
    auto missionTextLoaded = false;
    ChunkHeader header{};
    while (!missionKeyArrayLoaded || !missionTextLoaded) {
        ReadChunkHeader(&header, file, &offset, false);
        if (header.size == 0)
            continue;

        if (strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic)) == 0) {
            m_MissionKeyArray.Load(header.size, file, &offset, 0);
            missionKeyArrayLoaded = true;
            continue;
        }

        if (strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic)) == 0) {
            m_MissionText.Load(header.size, file, &offset, 0);
            missionTextLoaded = true;
            continue;
        }

        CFileMgr::Seek(file, header.size, SEEK_CUR);
        offset += header.size;
    }

    m_MissionKeyArray.Update(m_MissionText.m_data);
    CFileMgr::CloseFile(file);
    CTimer::Resume();
    CFileMgr::SetDir("");

    strncpy(m_szMissionName, mission, sizeof(m_szMissionName));
    m_bIsMissionPackLoaded = true;
}

// Loads mission table from GXT file
// 0x69F9A0
void CText::LoadMissionPackText() {
    CMessages::ClearAllMessagesDisplayedByGame(true);
    CFileMgr::SetDir("");

    m_MissionKeyArray.Unload();
    m_MissionText.Unload();

    m_bIsMissionPackLoaded = false;
    std::memset(m_szMissionName, '\0', sizeof(m_szMissionName));

    CFileMgr::SetDirMyDocuments();
    char filename[64];
    sprintf(filename, "MPACK//MPACK%d//TEXT.GXT", CGame::bMissionPackGame);

    auto* file = CFileMgr::OpenFile(filename, "rb");
    if (!file) {
        return;
    }

    uint16 version = 0, encoding = 0;
    CFileMgr::Read(file, &version, sizeof(version));
    CFileMgr::Read(file, &encoding, sizeof(encoding));

    DEV_LOG("[CText]: Loading '%s' version=%02d (%d-bit)\n", filename, version, encoding);
    assert(GAME_ENCODING == encoding && ("File %s was compiled with %d-bit char but %d-bit is required.", filename, encoding, GAME_ENCODING));

    uint32 offset = sizeof(uint16) * 2;
    bool bkey = false, btext = false;
    ChunkHeader header{};
    while (!bkey && !btext) {
        if (ReadChunkHeader(&header, file, &offset, false)) {
            m_bIsMissionPackLoaded = false;
            CFileMgr::CloseFile(file);
            return;
        }

        if (!strncmp(header.magic, CHUNK_TKEY, sizeof(header.magic))) {
            m_MissionKeyArray.Load(header.size, file, &offset, false);
            bkey = true;
            continue;
        }

        if (!strncmp(header.magic, CHUNK_TDAT, sizeof(header.magic))) {
            m_MissionText.Load(header.size, file, &offset, false);
            btext = true;
            continue;
        }

        // Skip any other data
        int8 bTmp;
        for (auto i = 0; i < header.size; ++i) {
            if (!CFileMgr::Read(file, &bTmp, sizeof(int8))) {
                m_bIsMissionPackLoaded = false;
                CFileMgr::CloseFile(file);
                return;
            }
        }
    }
    m_MainKeyArray.Update(m_MissionText.m_data);
    m_bIsMissionPackLoaded = true;
    strcpy(m_szMissionName, "MPNAME");
    CFileMgr::CloseFile(file);
}

// Returns text pointer by GXT key
// 0x6A0050
char* CText::Get(const char* key) {
    if (key[0] && key[0] != ' ') {
        bool found = false;

        char* str = m_MainKeyArray.Search(key, &found);
        if (found) {
            return str;
        }

        // check mission keys block if no entry found yet
        if ((CGame::bMissionPackGame || m_bIsMissionTextOffsetsLoaded) && m_bIsMissionPackLoaded) {
            str = m_MissionKeyArray.Search(key, &found);
            if (found) {
                return str;
            }
        }
    }

    char buf[32];
    sprintf(buf, "");
    AsciiToGxtChar(buf, GxtErrorString);
    return GxtErrorString;
}

// Writes loaded mission text into outStr
// 0x69FBD0
void CText::GetNameOfLoadedMissionText(char* outStr) {
    strcpy(outStr, m_szMissionName);
}

// 0x69F940
bool CText::ReadChunkHeader(ChunkHeader* header, FILESTREAM file, uint32* offset, uint8 unknown) {
    // ***: original code loops 8 times to read 1 byte with CFileMgr::Read, that's retarded
    // Same as in Android 2.0
    if (!CFileMgr::Read(file, header, sizeof(ChunkHeader))) {
        return false;
    }
    *offset += sizeof(ChunkHeader);
    return true;
}

// 0x69F750
char CText::GetUpperCase(char c) const {
    switch (m_nLangCode) {
    case 'e': // english
        if (c >= 'a' && c <= 'z')
            return c - 32;
        break;

    case 'f':
        if (c >= 'a' && c <= 'z')
            return c - 32;

        if (c >= 128 && c <= 255)
            return FrenchUpperCaseTable[c - 128];
        break;

    case 'g': // german
    case 'i': // italian
    case 's': // spanish
        if (c >= 'a' && c <= 'z')
            return c - 32;

        if (c >= 128 && c <= 255)
            return UpperCaseTable[c - 128];
        break;

    default:
        break;
    }
    return c;
}

CText* CText::Constructor() {
    this->CText::CText();
    return this;
}

CText* CText::Destructor() {
    this->CText::~CText();
    return this;
}
