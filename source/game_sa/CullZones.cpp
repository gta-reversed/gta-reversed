#include "StdInc.h"

#include "CullZones.h"

void CCullZones::InjectHooks() {
    RH_ScopedClass(CCullZones);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72D6B0);
    RH_ScopedInstall(AddCullZone, 0x72DF70);
    RH_ScopedInstall(AddTunnelAttributeZone, 0x72DB50);
    RH_ScopedInstall(AddMirrorAttributeZone, 0x72DC10);
    RH_ScopedInstall(FindTunnelAttributesForCoors, 0x72D9F0);
    RH_ScopedInstall(FindMirrorAttributesForCoors, 0x72DA70);
    RH_ScopedInstall(FindZoneWithStairsAttributeForPlayer, 0x72DAD0);
    RH_ScopedInstall(FindAttributesForCoors, 0x72D970);
    RH_ScopedInstall(Update, 0x72DEC0);
    RH_ScopedInstall(CamCloseInForPlayer, 0x72DD20);
    RH_ScopedInstall(CamStairsForPlayer, 0x72DD30);
    RH_ScopedInstall(Cam1stPersonForPlayer, 0x72DD40);
    RH_ScopedInstall(PoliceAbandonCars, 0x72DD60);
    RH_ScopedInstall(InRoomForAudio, 0x72DD70);
    RH_ScopedInstall(CamNoRain, 0x72DDB0);
    RH_ScopedInstall(FewerCars, 0x72DD80);
    RH_ScopedInstall(PlayerNoRain, 0x72DDC0);
    RH_ScopedInstall(FewerPeds, 0x72DD90);
    RH_ScopedInstall(NoPolice, 0x72DD50);
    RH_ScopedInstall(DoExtraAirResistanceForPlayer, 0x72DDD0);

    RH_ScopedGlobalInstall(IsPointWithinArbitraryArea, 0x72DDE0);
}

// 0x72D6B0
void CCullZones::Init() {
    ZoneScoped;

    NumAttributeZones = 0;
    CurrentFlags_Player = 0;
    CurrentFlags_Camera = 0;
}

// 0x72DDE0
bool IsPointWithinArbitraryArea(float TestPointX, float TestPointY, float Point1X, float Point1Y, float Point2X, float Point2Y, float Point3X, float Point3Y, float Point4X, float Point4Y) {
    if (-(((TestPointY - Point1Y) * (Point2X - Point1X)) - ((TestPointX - Point1X) * (Point2Y - Point1Y))) < 0.0) {
        return 0;
    }
    if (-(((TestPointY - Point2Y) * (Point3X - Point2X)) - ((TestPointX - Point2X) * (Point3Y - Point2Y))) < 0.0) {
        return 0;
    }
    if (-(((TestPointY - Point3Y) * (Point4X - Point3X)) - ((TestPointX - Point3X) * (Point4Y - Point3Y))) >= 0.0) {
        return -(((Point1X - Point4X) * (TestPointY - Point4Y)) - ((TestPointX - Point4X) * (Point1Y - Point4Y))) >= 0.0;
    }
    return 0;
}

// flags: see eZoneAttributeFlags
// 0x72DF70
void CCullZones::AddCullZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags) {
    if ((flags & (eZoneAttributeFlags::ZONE_ATTRIBUTE_UNKNOWN_1 | eZoneAttributeFlags::ZONE_ATTRIBUTE_UNKNOWN_2))) {
        AddTunnelAttributeZone(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ, flags);
        flags &= ~(eZoneAttributeFlags::ZONE_ATTRIBUTE_UNKNOWN_2 | eZoneAttributeFlags::ZONE_ATTRIBUTE_UNKNOWN_1);
    }

    // Will be computed if flags less than 0x880 (ZONE_ATTRIBUTE_UNKNOWN_1 + ZONE_ATTRIBUTE_UNKNOWN_2)
    if (flags) {
        CCullZone& zone = aAttributeZones[NumAttributeZones];
        zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
        zone.flags = static_cast<eZoneAttributeFlags>(flags);

        NumAttributeZones += 1;
    }
}

// flags: see eZoneAttributeFlags
// 0x72DB50
void CCullZones::AddTunnelAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags) {
    CCullZone& zone = aTunnelAttributeZones[NumTunnelAttributeZones];

    zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
    zone.flags = static_cast<eZoneAttributeFlags>(flags);

    NumTunnelAttributeZones += 1;
}

// 0x72DC10
void CCullZones::AddMirrorAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, eZoneAttributeFlags flags, float cm, float vX, float vY, float vZ) {
    CCullZoneReflection& zone = aMirrorAttributeZones[NumMirrorAttributeZones];

    zone.zoneDef.Init(center, unk1, fWidthY, fBottomZ, fWidthX, unk2, fTopZ);
    zone.flags = flags;
    zone.cm = cm;
    zone.vx = (int8)(vX * 100.0f);
    zone.vy = (int8)(vY * 100.0f);
    zone.vz = (int8)(vZ * 100.0f);

    NumMirrorAttributeZones += 1;
}

// 0x72DD20
bool CCullZones::CamCloseInForPlayer() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMCLOSEIN_FORPLAYER) != 0;
}

// 0x72DD30
bool CCullZones::CamStairsForPlayer() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMSTAIRS_FORPLAYER) != 0;
}

// 0x72DD40
bool CCullZones::Cam1stPersonForPlayer() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAM1STPERSONS_FORPLAYER) != 0;
}
// 0x72DD60
bool CCullZones::PoliceAbandonCars() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_POLICES_ABANDONCARS) != 0;
}

// 0x72DD70
bool CCullZones::InRoomForAudio() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_INROOMS_FORAUDIO) != 0;
}

// 0x72DD80
bool CCullZones::FewerCars() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_FEWERCARS) != 0;
}

// 0x72DDB0
bool CCullZones::CamNoRain() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNORAIN) != 0;
}

// 0x72DDC0
bool CCullZones::PlayerNoRain() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNORAIN) != 0;
}

// 0x72DD90
bool CCullZones::FewerPeds() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_INROOMS_FEWERPEDS) != 0;
}

// 0x72DD50
bool CCullZones::NoPolice() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_POLICES_ABANDONCARS) != 0;
}

// 0x72DDD0
bool CCullZones::DoExtraAirResistanceForPlayer() {
    return (CCullZones::CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_EXTRA_AIRRESISTANCE) != 0;
}

// 0x72D9F0
eZoneAttributeFlags CCullZones::FindTunnelAttributesForCoors(CVector point) {
    if (NumTunnelAttributeZones <= 0) {
        return eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNONE;
    }

    int32 out = eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNONE;
    for (CCullZone& zone : aTunnelAttributeZones) {
        if (zone.IsPointWithin(point)) {
            out |= zone.flags;
        }
    }

    return static_cast<eZoneAttributeFlags>(out);
}

// 0x72DA70
CCullZoneReflection* CCullZones::FindMirrorAttributesForCoors(CVector cameraPosition) {
    if (NumMirrorAttributeZones <= 0) {
        return nullptr;
    }

    for (CCullZoneReflection& zone : aMirrorAttributeZones) {
        if (zone.IsPointWithin(cameraPosition)) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72DAD0
CCullZone* CCullZones::FindZoneWithStairsAttributeForPlayer() {
    if (NumAttributeZones <= 0) {
        return nullptr;
    }

    for (CCullZone& zone : aAttributeZones) {
        if ((zone.flags & eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMSTAIRS_FORPLAYER) != 0 &&
            zone.IsPointWithin(FindPlayerCoors())
        ) {
            return &zone;
        }
    }

    return nullptr;
}

// 0x72D970
eZoneAttributeFlags CCullZones::FindAttributesForCoors(CVector pos) {
    if (NumAttributeZones <= 0) {
        return eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNONE;
    }

    int32 out = eZoneAttributeFlags::ZONE_ATTRIBUTE_CAMNONE;
    for (CCullZone& zone : aAttributeZones) {
        if (zone.IsPointWithin(pos)) {
            out |= zone.flags;
        }
    }

    return static_cast<eZoneAttributeFlags>(out);
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
        if (!bMilitaryZonesDisabled && (CurrentFlags_Player & eZoneAttributeFlags::ZONE_ATTRIBUTE_MILITARYZONE) != 0) {
            auto player = FindPlayerPed();
            if (player->IsAlive()) {
                player->SetWantedLevelNoDrop(5);
            }
        }
    }
}
