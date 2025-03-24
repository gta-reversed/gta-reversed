/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

constexpr auto NUM_POINTLIST_POINTS = 24;

class CPointList {
public:
    uint32                                    m_Count{};
    std::array<CVector, NUM_POINTLIST_POINTS> m_Coords{};
    std::array<bool, NUM_POINTLIST_POINTS>    m_UsedCoords{};

    // 0x699F00
    void Empty() {
        m_Count = 0;
        rng::fill(m_UsedCoords, false);
    }

    // NOTSA
    auto& Get(size_t i) {
        assert(m_UsedCoords[i]);
        return m_Coords[i];
    }

    auto GetPoints() const {
        return m_Coords | rng::views::take(m_Count) | rng::views::filter([this](auto& pt) {
            return m_UsedCoords[&pt - m_Coords.data()];
        });
    }
};

VALIDATE_SIZE(CPointList, 0x13C);
