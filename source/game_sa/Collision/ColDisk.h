/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "ColSurface.h"

class CColDisk {
public:
    CVector     m_vecStart;
    float       m_fStartRadius;
    CColSurface m_Surface;
    CVector     m_vecEnd;
    float       m_fEndRadius;

public:
    // 0x40FD50
    void Set(float startRadius, const CVector& start, const CVector& end, float endRadius, eSurfaceType material, uint8 pieceType, tColLighting lighting) {
        m_fStartRadius = startRadius;
        m_vecStart = start;
        m_vecEnd = end;
        m_fEndRadius = endRadius;
        m_Surface.m_nMaterial = material;
        m_Surface.m_nPiece = pieceType;
        m_Surface.m_nLighting = lighting;
    }
};
VALIDATE_SIZE(CColDisk, 0x24);
