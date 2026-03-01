#pragma once


class CEntity;
class CPlantLocTri;

class CPlantColEntEntry {
public:
    CEntity*           m_Entity;
    CPlantLocTri**     m_Objects;
    uint16             m_numTriangles;
    CPlantColEntEntry* m_NextEntry;
    CPlantColEntEntry* m_PrevEntry;

public:
    static void InjectHooks();

    CPlantColEntEntry* AddEntry(CEntity* entity);
    void ReleaseEntry();
};
VALIDATE_SIZE(CPlantColEntEntry, 0x14);
