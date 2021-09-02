/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CSphere.h"

class CColSphere : public CSphere {
public:
    union {
        uint8 m_nMaterial;
        uint8 m_nColSlot;
    };
    union {
        uint8 m_nFlags;
        struct {
            uint8 m_bFlag0x01 : 1;
            uint8 m_bIsSingleColDataAlloc : 1;
            uint8 m_bIsActive : 1;
            uint8 m_bFlag0x08 : 1;
            uint8 m_bFlag0x10 : 1;
            uint8 m_bFlag0x20 : 1;
            uint8 m_bFlag0x40 : 1;
            uint8 m_bFlag0x80 : 1;
        };
    };
    uint8 m_nLighting;
    uint8 m_nLight;

public:
    static void InjectHooks();

    void Set(float radius, CVector const& center);
    void Set(float radius, CVector const& center, uint8 material, uint8 flags, uint8 lighting);
    bool IntersectRay(CVector const& rayOrigin, CVector const& direction, CVector& intersectPoint1, CVector& intersectPoint2);
    bool IntersectEdge(CVector const& startPoint, CVector const& endPoint, CVector& intersectPoint1, CVector& intersectPoint2);
    bool IntersectSphere(CColSphere const& right);
    bool IntersectPoint(CVector const& point);
};

VALIDATE_SIZE(CColSphere, 0x14);
