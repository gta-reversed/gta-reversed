#pragma once

#include "Vector.h"

// https://gtamods.com/wiki/CULL

enum eZoneAttributes : uint16 {
    NONE = 0x0,

    // Camera close in into player using closest third-person view camera mode,
    // does not close in if in first person or cinematic mode,
    // camera mode cannot be changed while in the zone
    CAM_CLOSE_IN_FOR_PLAYER = 0x1,

    // Camera remotely placed outside the zone, no control of camera,
    // camera mode cannot be changed while in the zone
    CAM_STAIRS_FOR_PLAYER = 0x2,

    // Lowers the camera angle on boats
    CAM_1ST_PERSONS_FOR_PLAYER = 0x4,

    // Rain-free, police helicopter-free zone
    CAM_NO_RAIN = 0x8,

    // Police will not exit their vehicles voluntarily.
    // They will only exit if you do something to them (like shoot it).
    // Cops both on foot and in vehicles will not chase you but can shoot at you.
    NO_POLICE = 0x10,

    DO_I_NEED_TO_LOAD_COLLISION = 0x40,

    UNKNOWN_1 = 0x80,

    // Police will always exit their vehicles
    // once they are spawned ONLY IF you have a wanted level.
    // If you don't, they'll drive normally.
    POLICES_ABANDON_CARS = 0x100,

    IN_ROOMS_FOR_AUDIO = 0x200,

    // Some visual ocean water effects are removed
    // like the transparent waves and sparkles on the water.
    IN_ROOMS_FEWER_PEDS = 0x400,

    UNKNOWN_2 = 0x800,

    // 5-Star Military zone.
    MILITARY_ZONE = 0x1000,

    // Doesn't allow cars to reach top speed.
    EXTRA_AIR_RESISTANCE = 0x4000,

    // Spawn fewer cars in this area.
    FEWER_CARS = 0x8000,
};

struct CZoneDef {
    int16 cornerX, cornerY;
    int16 vec1X, vec1Y;
    int16 vec2X, vec2Y;
    int16 minZ, maxZ;

    void Init(
        const CVector& center,
        float x12, float fWidthY,
        float fBottomZ,
        float fWidthX, float y13,
        float fTopZ
    ) {
        cornerX = (int16)(center.x - x12 - fWidthX);
        cornerY = (int16)(center.y - fWidthY - y13);

        vec1X = (int16)(x12 + x12);
        vec1Y = (int16)(fWidthY + fWidthY);

        vec2X = (int16)(fWidthX + fWidthX);
        vec2Y = (int16)(y13 + y13);

        minZ = (int16)(fBottomZ);
        maxZ = (int16)(fTopZ);
    }

    // 0x72D850
    [[nodiscard]] bool IsPointWithin(const CVector& point) const {
        if ((float)minZ >= point.z || (float)maxZ <= point.z) {
            return false;
        }

        float dx = point.x - (float)cornerX;
        float dy = point.y - (float)cornerY;

        float sqMag0 = (float)vec1X * dx + (float)vec1Y * dy;
        if (sqMag0 < 0.0f || sqMag0 > (sq(vec1Y) + sq(vec1X))) {
            return false;
        }

        float sqMag1 = (float)vec2X * dx + (float)vec2Y * dy;
        if (sqMag1 < 0.0f || sqMag1 > (sq(vec2Y) + sq(vec2X))) {
            return false;
        }

        return true;
    }
};

VALIDATE_SIZE(CZoneDef, 0x10);

// https://gtaforums.com/topic/202532-sadoc-ipl-definitions/page/5/?tab=comments#comment-3875562
struct CCullZoneReflection {
    CZoneDef zoneDef;
    float    cm; // mirror plane coordinate in direction axis.
    int8     vx; // mirror direction (like [-1 0 0] or [0 1 0] or [0 0 -1])
    int8     vy;
    int8     vz;
    uint8    flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CCullZoneReflection, 0x18);

struct CCullZone {
    CZoneDef zoneDef;
    eZoneAttributes flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CCullZone, 0x12);

// CULL is a section in the item placement file.
// It is used to create zones affecting the different behaviour of the world, like
// mirrors, fixed camera positions, disabling rain, police behaviors, etc.,
// when the player is within the zone.
class CCullZones {
public:
    static inline int32& NumAttributeZones = *(int32*)0xC87AC8;
    static inline int32& NumMirrorAttributeZones = *(int32*)0xC87AC4;
    static inline int32& NumTunnelAttributeZones = *(int32*)0xC87AC0;

    static inline int32& CurrentFlags_Camera = *(int32*)0xC87ABC;
    static inline int32& CurrentFlags_Player = *(int32*)0xC87AB8;

    static inline bool& bMilitaryZonesDisabled = *(bool*)0xC87ACD;

    static inline CCullZone (&aAttributeZones)[1'300] = *(CCullZone(*)[1'300])0xC81F50;
    static inline CCullZone (&aTunnelAttributeZones)[40] = *(CCullZone(*)[40])0xC81C80;
    static inline CCullZoneReflection (&aMirrorAttributeZones)[72] = *(CCullZoneReflection(*)[72])0xC815C0;

public:
    static void InjectHooks();

    static void Init();
    static void Update();

    static eZoneAttributes FindAttributesForCoors(CVector pos);
    static eZoneAttributes FindTunnelAttributesForCoors(CVector point);
    static CCullZoneReflection* FindMirrorAttributesForCoors(CVector cameraPosition);
    static CCullZone* FindZoneWithStairsAttributeForPlayer();
    static void AddCullZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags);
    static void AddTunnelAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags);
    static void AddMirrorAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, eZoneAttributes flags, float cm, float vX, float vY, float vZ);

    static bool CamCloseInForPlayer();
    static bool CamStairsForPlayer();
    static bool Cam1stPersonForPlayer();
    static bool NoPolice();
    static bool PoliceAbandonCars();
    static bool InRoomForAudio();
    static bool FewerCars();
    static bool FewerPeds();
    static bool CamNoRain();
    static bool PlayerNoRain();

    static bool DoExtraAirResistanceForPlayer();
};
