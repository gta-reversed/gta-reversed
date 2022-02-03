/*
    Plugin-SDK file
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
    uint8        m_nFlags; // TODO: This aren't actually `flags`, it's `piece` (as in CColPoint::piceType)
    tColLighting m_nLighting;
    uint8        m_nBrightness;

public:
    CColBox() = default;
    
    CColBox(const CBox& box) :
        CBox(box)
    {
    }

    CColBox(const CBox& box, uint8 material, uint8 flags, uint8 lightning) :
        CBox(box),
        m_nMaterial(material),
        m_nFlags(flags),
        m_nLighting(lightning)
    {

    }

    void     Set(const CVector& sup, const CVector& inf, uint8 material, uint8 flags, uint8 lighting);
    CColBox& operator=(const CColBox& right);
};

VALIDATE_SIZE(CColBox, 0x1C);
