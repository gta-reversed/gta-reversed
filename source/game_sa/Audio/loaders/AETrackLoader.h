#pragma once
#include "AEDataStream.h"

struct tTrackLookup {
    uint8 m_nbPackId;
    uint32 m_nOffset;
    uint32 m_nSize;
};
VALIDATE_SIZE(tTrackLookup, 0xC);

struct StreamPack {
    char m_szName[16];
};
VALIDATE_SIZE(StreamPack, 0x10);

struct tTrackInfo {
    struct tBeat {
        uint32 m_nTime{};
        uint32 m_nKey{};
    } m_aBeats[1000];

    uint32 field_1F40[16];
    uint16 field_1F80;
};
VALIDATE_SIZE(tTrackInfo, 0x1F84);
VALIDATE_SIZE(tTrackInfo::tBeat, 0x8);

class CAETrackLoader {
public:
    bool          m_bInitialised;
    uint32        m_nTrackCount;
    uint32        m_nStreamPackCount;
    tTrackLookup* m_paTrackLookups;
    StreamPack*   m_paStreamPacks;
    bool          m_bStreamingFromDVD;
    char*         m_pszDvdDrivePath;

public:
    static void InjectHooks();

    CAEDataStream* GetDataStream(uint32 trackId);
};
VALIDATE_SIZE(CAETrackLoader, 0x1C);
