#include "StdInc.h"

void CPedGroupPlacer::InjectHooks() {
    RH_ScopedClass(CPedGroupPlacer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(PlaceFormationGroup, 0x5FC9B0, {.reversed = false});
    RH_ScopedInstall(PlaceChatGroup, 0x5FCE80, {.reversed = false});
    RH_ScopedInstall(PlaceRandomGroup, 0x5FD330, {.reversed = false});
    RH_ScopedInstall(PlaceGroup, 0x5FD810);
}

// 0x5FC9B0
bool CPedGroupPlacer::PlaceFormationGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return plugin::CallMethodAndReturn<bool, 0x5FC9B0, CPedGroupPlacer*, ePedType, uint32, CVector const&, ePedGroupDefaultTaskAllocatorType>(this, type, numOfPeds, origin, unused);
}

// 0x5FCE80
bool CPedGroupPlacer::PlaceChatGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return plugin::CallMethodAndReturn<bool, 0x5FCE80, CPedGroupPlacer*, ePedType, uint32, CVector const&, ePedGroupDefaultTaskAllocatorType>(this, type, numOfPeds, origin, unused);
}

// 0x5FD330
bool CPedGroupPlacer::PlaceRandomGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType unused) {
    return plugin::CallMethodAndReturn<bool, 0x5FD330, CPedGroupPlacer*, ePedType, uint32, CVector const&, ePedGroupDefaultTaskAllocatorType>(this, type, numOfPeds, origin, unused);
}

// 0x5FD810
bool CPedGroupPlacer::PlaceGroup(ePedType type, uint32 numOfPeds, const CVector& origin, ePedGroupDefaultTaskAllocatorType allocType) {
    using enum ePedGroupDefaultTaskAllocatorType;
    switch (allocType) {
    case FOLLOW_ANY_MEANS:
    case FOLLOW_LIMITED:   return PlaceFormationGroup(type, numOfPeds, origin, allocType);
    case STAND_STILL:
    case CHAT:             return PlaceChatGroup(type, numOfPeds, origin, allocType);
    case RANDOM:           return PlaceRandomGroup(type, numOfPeds, origin, allocType);
    default:               NOTSA_UNREACHABLE();
    }
}
