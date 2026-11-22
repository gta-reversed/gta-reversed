/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Box.h"
#include "ColPoint.h"
#include "ColSurface.h"

class CColBox : public CBox {
public:
    CColSurface m_Surface;

public:
    constexpr CColBox() = default;
    constexpr CColBox(const CVector& min, const CVector& max) : CBox(min, max) {}
    constexpr CColBox(const CBox& box) : CBox(box) {}
    constexpr CColBox(const CBox& box, eSurfaceType material, uint8 pieceType, tColLighting lightning) : CBox(box) {
        m_Surface.m_nMaterial = material;
        m_Surface.m_nPiece = pieceType;
        m_Surface.m_nLighting = lightning;
    }

    void Set(const CVector& sup, const CVector& inf, eSurfaceType material, uint8 pieceType, tColLighting lighting);
    CColBox& operator=(const CColBox& right);

    auto GetSurfaceType() const { return m_Surface.m_nMaterial; }
};
VALIDATE_SIZE(CColBox, 0x1C);
