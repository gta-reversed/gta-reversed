#pragma once

#include "Vector.h"

class CPlantLocTri {
public:
    CVector m_V1;
    CVector m_V2;
    CVector m_V3;
    CVector m_Center;
    float m_SphereRadius;
    std::array<float, 3> m_Seed;
    std::array<uint16, 3> m_nMaxNumPlants;
    uint8 m_SurfaceId;
    tColLighting m_nLighting;

    union {
        struct {
            uint8 m_createsPlants : 1;  // 0x1
            uint8 m_createsObjects : 1; // 0x2
            uint8 m_createdObjects : 1; // 0x4
            uint8 m_flagPad : 5;
        };
        uint8_t m_Flags;
    };

    CPlantLocTri* m_NextTri;
    CPlantLocTri* m_PrevTri;

public:
    static void InjectHooks();

    CPlantLocTri* Add(const CVector& p1, const CVector& p2, const CVector& p3, uint8 surface, tColLighting lightning, bool createsPlants, bool createsObjects);
    void Release();
};
VALIDATE_SIZE(CPlantLocTri, 0x54);
