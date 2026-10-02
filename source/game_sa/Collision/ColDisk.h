/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "ColSurface.h"

class CColDisk : public CColSphere {
public:
    CVector     m_vThickness{};
    float       m_fThickness{};

public:
    // 0x40FD50
    void Set(float fRadius, const CVector& center, const CVector& thickness, float fThickness, eSurfaceType material, uint8 pieceType = 0, tColLighting lighting = tColLighting{ 0xFF }) {
        m_fRadius = fRadius;
        m_vecCenter = center;
        m_vThickness = thickness;
        m_fThickness = fThickness;

        m_Surface.m_nMaterial = material;
        m_Surface.m_nPiece = pieceType;
        m_Surface.m_nLighting = lighting;
    }
};
VALIDATE_SIZE(CColDisk, 0x24);
