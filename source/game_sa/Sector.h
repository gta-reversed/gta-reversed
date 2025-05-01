/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrListDoubleLink.h"

class CSector {
public:
    CPtrListSingleLink<CBuilding*> m_buildings{}; //!< Buildings in this sector [Yes, it's single-link]
    CPtrListDoubleLink<CDummy*>    m_dummies{};   //!< Dummies in this sector
};
VALIDATE_SIZE(CSector, 8);
