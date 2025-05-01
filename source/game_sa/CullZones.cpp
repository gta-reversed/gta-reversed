#include "StdInc.h"

#include "CullZones.h"

void CCullZones::InjectHooks() {
    RH_ScopedClass(CCullZones);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72D6B0);
    RH_ScopedInstall(AddCullZone, 0x72DF70);
    RH_ScopedInstall(AddTunnelAttributeZone, 0x72DB50);
    RH_ScopedInstall(AddMirrorAttributeZone, 0x72DC10);
    RH_ScopedInstall(InRoomForAudio, 0x72DD70);
    RH_ScopedInstall(FewerCars, 0x72DD80);
    RH_ScopedInstall(CamNoRain, 0x72DDB0);
    RH_ScopedInstall(PlayerNoRain, 0x72DDC0);
    RH_ScopedInstall(FewerPeds, 0x72DD90);
    RH_ScopedInstall(NoPolice, 0x72DD50);
    RH_ScopedInstall(DoExtraAirResistanceForPlayer, 0x72DDD0);
    RH_ScopedInstall(FindTunnelAttributesForCoors, 0x72D9F0);
    RH_ScopedInstall(FindMirrorAttributesForCoors, 0x72DA70);
    RH_ScopedInstall(FindZoneWithStairsAttributeForPlayer, 0x72DAD0);
    RH_ScopedInstall(FindAttributesForCoors, 0x72D970);
    RH_ScopedInstall(Update, 0x72DEC0);
}

// 0x72D6B0
void CCullZones::Init() {
    ZoneScoped;

    NumAttributeZones = 0;
    CurrentFlags_Player = 0;
    CurrentFlags_Camera = 0;
}

// flags: see eZoneAttributes
// 0x72DF70
void CCullZones::AddCullZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags) {
    if ((flags & (eZoneAttributes::UNKNOWN_1 | eZoneAttributes::UNKNOWN_2))) {
        AddTunnelAttributeZone(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ, flags);
        flags &= ~(eZoneAttributes::UNKNOWN_2 | eZoneAttributes::UNKNOWN_1);
    }

    // Will be computed if flags less than 0x880 (UNKNOWN_1 + UNKNOWN_2)
    if (flags) {
        CCullZone& zone = aAttributeZones[NumAttributeZones];
        zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
        zone.flags = static_cast<eZoneAttributes>(flags);

        NumAttributeZones += 1;
    }
}

// flags: see eZoneAttributes
// 0x72DB50
void CCullZones::AddTunnelAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags) {
    CCullZone& zone = aTunnelAttributeZones[NumTunnelAttributeZones];

    zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
    zone.flags = static_cast<eZoneAttributes>(flags);

    NumTunnelAttributeZones += 1;
}

// 0x72DC10
void CCullZones::AddMirrorAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, eZoneAttributes flags, float cm, float vX, float vY, float vZ) {
    CCullZoneReflection& zone = aMirrorAttributeZones[NumMirrorAttributeZones];

    zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
    zone.flags = flags;
    zone.cm = cm;
    zone.vx = (int8)(vX * 100.0f);
    zone.vy = (int8)(vY * 100.0f);
    zone.vz = (int8)(vZ * 100.0f);

    NumMirrorAttributeZones += 1;
}

// 0x72DD70
bool CCullZones::InRoomForAudio() {
    return (CurrentFlags_Camera & eZoneAttributes::IN_ROOMS_FOR_AUDIO) != 0;
}

// 0x72DD80
bool CCullZones::FewerCars() {
    return (CurrentFlags_Player & eZoneAttributes::FEWER_CARS) != 0;
}

// 0x72DDB0
bool CCullZones::CamNoRain() {
    return (CurrentFlags_Camera & eZoneAttributes::CAM_NO_RAIN) != 0;
}

// 0x72DDC0
bool CCullZones::PlayerNoRain() {
    return (CurrentFlags_Player & eZoneAttributes::CAM_NO_RAIN) != 0;
}

// 0x72DD90
bool CCullZones::FewerPeds() {
    return (CurrentFlags_Player & eZoneAttributes::IN_ROOMS_FEWER_PEDS) != 0;
}

// 0x72DD50
bool CCullZones::NoPolice() {
    return (CurrentFlags_Player & eZoneAttributes::NO_POLICE) != 0;
}

// 0x72DDD0
bool CCullZones::DoExtraAirResistanceForPlayer() {
    return (CurrentFlags_Player & eZoneAttributes::EXTRA_AIR_RESISTANCE) != 0;
}

// 0x72D9F0
eZoneAttributes CCullZones::FindTunnelAttributesForCoors(CVector point) {
    if (NumTunnelAttributeZones <= 0)
        return eZoneAttributes::NONE;

    int32 out = eZoneAttributes::NONE;
    for (CCullZone& zone : aTunnelAttributeZones) {
        if (zone.IsPointWithin(point)) {
            out |= zone.flags;
        }
    }

    return static_cast<eZoneAttributes>(out);
}

// 0x72DA70
CCullZoneReflection* CCullZones::FindMirrorAttributesForCoors(CVector cameraPosition) {
    if (NumMirrorAttributeZones <= 0)
        return nullptr;

    for (CCullZoneReflection& zone : aMirrorAttributeZones) {
        if (zone.IsPointWithin(cameraPosition)) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72DAD0
CCullZone* CCullZones::FindZoneWithStairsAttributeForPlayer() {
    if (NumAttributeZones <= 0)
        return nullptr;

    for (CCullZone& zone : aAttributeZones) {
        if ((zone.flags & eZoneAttributes::CAM_STAIRS_FOR_PLAYER) != 0 &&
            zone.IsPointWithin(FindPlayerCoors())
        ) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72D970
eZoneAttributes CCullZones::FindAttributesForCoors(CVector pos) {
    if (NumAttributeZones <= 0)
        return eZoneAttributes::NONE;

    int32 out = eZoneAttributes::NONE;
    for (CCullZone& zone : aAttributeZones) {
        if (zone.IsPointWithin(pos)) {
            out |= zone.flags;
        }
    }

    return static_cast<eZoneAttributes>(out);
}

// 0x72DEC0
void CCullZones::Update() {
    ZoneScoped;

    if ((CTimer::GetFrameCounter() & 7) == 2) {
        CurrentFlags_Camera = FindAttributesForCoors(*TheCamera.GetGameCamPosition());
        return;
    }

    if ((CTimer::GetFrameCounter() & 7) == 6) {
        CurrentFlags_Player = FindAttributesForCoors(FindPlayerCoors());
        if (!bMilitaryZonesDisabled && (CurrentFlags_Player & eZoneAttributes::MILITARY_ZONE) != 0) {
            auto player = FindPlayerPed();
            if (player->IsAlive()) {
                player->SetWantedLevelNoDrop(5);
            }
        }
    }
}
