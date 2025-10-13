/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Box.h"

class CBoundingBox : public CBox {
public:
    constexpr CBoundingBox() : CBox(CVector{1.0f}, CVector{-1.0f}) {}
    constexpr CBoundingBox(CVector min, CVector max) : CBox(min, max) {}
    constexpr explicit CBoundingBox(const CBox& box) : CBox(box) {}

    [[nodiscard]] bool IsPointWithin(const CVector& point) const;
    inline void SetMinMax(CVector min, CVector max);
};

VALIDATE_SIZE(CBoundingBox, 0x18);

// orig in entity.cpp
static int32 numBBFailed; // 0xB71808
