/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PtrListDoubleLink.h"

class CSector {
protected:
    CPtrListSingleLink<CBuilding*> Buildings{}; //!< Buildings in this sector [Yes, it's single-link]
    CPtrListDoubleLink<CDummy*>    Dummies{};   //!< Dummies in this sector

public: // inline
    CPtrListSingleLink<CBuilding*>& GetOverlapBuildingPtrList() { return Buildings; }
    CPtrListDoubleLink<CDummy*>&    GetOverlapDummyPtrList()    { return Dummies; }

public: // NOTSA:
    CSector(const CSector&)            = delete;
    CSector& operator=(const CSector&) = delete;
    CSector(CSector&&)                 = delete;
    CSector& operator=(CSector&&)      = delete;
};

VALIDATE_SIZE(CSector, 0x8);
