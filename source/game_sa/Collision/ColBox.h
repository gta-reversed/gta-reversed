/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Box.h"
#include "ColPoint.h"

class CColBox : public CBox {
public:
    uint8        m_nMaterial;
    uint8        m_nFlags;
    tColLighting m_nLighting;
    uint8        m_nBrightness;

public:
    void     Set(CVector const& sup, CVector const& inf, uint8 material, uint8 flags, uint8 lighting);
    CColBox& operator=(CColBox const& right);
};

VALIDATE_SIZE(CColBox, 0x1C);
