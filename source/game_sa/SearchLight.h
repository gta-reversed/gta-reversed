#pragma once
#include "Base.h"
class CEntity;

class CSearchLight {
public:

    static void InjectHooks();
    static void SetTravelToPoint();
    static void SetFollowEntity();
    static void SetPathBetween();
    static void IsLookingAtPos();
    static void GetOnEntity();
    static bool IsSpottedEntity(uint32 index, const CEntity& entity);
};
