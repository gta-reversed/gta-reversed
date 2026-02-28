/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CPointList {
public:
    bool Empty() const noexcept;
    void AddPoint(CVector pt) noexcept;
    void MergeListsRemovingDoubles(CPointList* main, CPointList* toBeMerged) noexcept;

public:
    uint32                  m_Count{};
    std::array<CVector, 24> m_Points{};
    bool                    m_PointHasBeenClaimed[24]{};
};
VALIDATE_SIZE(CPointList, 0x13C);
