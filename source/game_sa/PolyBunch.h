/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CPolyBunch {
public:
    std::array<CVector, 7> m_avecPosn;
    CPolyBunch*            m_pNext;
    int16                  m_wNumVerts; // 100% it's signed
    std::array<uint8, 7>   m_aU; /// Divide by 200 to get the actual coords
    std::array<uint8, 7>   m_aV; /// // Divide by 200 to get the actual coords

    auto GetVerts() const { return m_avecPosn | rng::views::take((size_t)m_wNumVerts); }
};

VALIDATE_SIZE(CPolyBunch, 0x68);
