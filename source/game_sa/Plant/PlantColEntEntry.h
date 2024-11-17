#pragma once


class CEntity;
class CPlantLocTri;

class CPlantColEntEntry {
public:
    CEntity*           m_pEntity;
    CPlantLocTri**     m_LocTriArray;
    uint16             m_nNumTris;
    CPlantColEntEntry* m_pNextEntry;
    CPlantColEntEntry* m_pPrevEntry;

public:
    static void InjectHooks();

    CPlantColEntEntry* AddEntry(CEntity* entity);
    void ReleaseEntry();
};
VALIDATE_SIZE(CPlantColEntEntry, 0x14);
