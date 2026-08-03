/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "Enums/eSurfaceType.h"

struct tColLighting {
    union {
        struct {
            uint8 day : 4;
            uint8 night : 4;
        };
        uint8 value;
    };

    tColLighting() = default;
    constexpr explicit tColLighting(const uint8 ucLighting) {
        day = ucLighting & 0xF;
        night = (ucLighting >> 4) & 0xF;
    }

    [[nodiscard]] float GetCurrentLighting(float fScale = 0.5F) const;
};
VALIDATE_SIZE(tColLighting, 0x1);

class CColPoint {
public:
    /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CColPointSA.h */
    CVector      m_vecPoint;        // 0x00
    float        field_C;           // 0x0C
    CVector      m_vecNormal;       // 0x10
    float        field_1C;          // 0x1C

    // col shape 1 info
    eSurfaceType m_nSurfaceTypeA;   // 0x20
    uint8        m_nPieceTypeA;     // 0x21
    tColLighting m_nLightingA;      // 0x22

    // col shape 2 info
    eSurfaceType m_nSurfaceTypeB;   // 0x23
    uint8 m_nPieceTypeB;            // 0x24
    tColLighting m_nLightingB;      // 0x25

    char _align0x26[2];             // 0x26
                            
    float m_fDepth;                 // 0x28

public:
    CColPoint& operator=(CColPoint const&) = default;
};
VALIDATE_SIZE(CColPoint, 0x2C);
