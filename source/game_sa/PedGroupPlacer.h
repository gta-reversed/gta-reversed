/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "ePedType.h"

//! To use functions from this class, just default construct (Eg.: `CPedGroupPlacer{}`) an object of this type
class CPedGroupPlacer {
public:
    static const int32& ms_minGangSize; // 1
    static const int32& ms_maxGangSize; // 5

public:
    static void InjectHooks();

    bool PlaceChatGroup     (ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType = {});
    bool PlaceFormationGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType = {});
    bool PlaceGroup         (ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType nGroupPlacerType);
    bool PlaceRandomGroup   (ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType = {});
};
