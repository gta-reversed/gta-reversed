#pragma once

#include "Vector.h"

// https://gtamods.com/wiki/CULL

enum eZoneAttributes : uint16 {
    ZA_NONE = 0x0,

    // Camera close in into player using closest third-person view camera mode,
    // does not close in if in first person or cinematic mode,
    // camera mode cannot be changed while in the zone
    CAM_CLOSE_IN = 0x1,

    // Camera remotely placed outside the zone, no control of camera,
    // camera mode cannot be changed while in the zone
    STAIRS = 0x2,

    // Lowers the camera angle on boats
    STPERSON_1ST = 0x4,

    // Rain-free, police helicopter-free zone
    NO_RAIN = 0x8,

    // Police will not exit their vehicles voluntarily.
    // They will only exit if you do something to them (like shoot it).
    // Cops both on foot and in vehicles will not chase you but can shoot at you.
    NO_POLICE = 0x10,

    LOAD_COLLISION = 0x40,

    TUNNEL_TRANSITION = 0x80,

    // Police will always exit their vehicles
    // once they are spawned ONLY IF you have a wanted level.
    // If you don't, they'll drive normally.
    POLICE_ABANDON_CARS = 0x100,

    IN_ROOM_FOR_AUDIO = 0x200,

    // Some visual ocean water effects are removed
    // like the transparent waves and sparkles on the water.
    FEWER_PEDS = 0x400,

    TUNNEL = 0x800,

    // 5-Star Military zone.
    MILITARY_ZONE = 0x1000,

    // Doesn't allow cars to reach top speed.
    EXTRA_AIR_RESISTANCE = 0x4000,

    // Spawn fewer cars in this area.
    FEWER_CARS = 0x8000
};

enum eMirrorFlags : uint8 {
    MF_NONE = 0x0,

    MF_MIRROR = 0x1,

    MF_SCREENS_1 = 0x2,

    MF_SCREENS_2 = 0x4,

    MF_SCREENS_3 = 0x8,

    MF_SCREENS_4 = 0x10,

    MF_SCREENS_5 = 0x20,

    MF_SCREENS_6 = 0x40
};

struct CZoneDef {
    int16 m_cornerX, m_cornerY;
    int16 m_vec1X, m_vec1Y;
    int16 m_vec2X, m_vec2Y;
    int16 m_minZ, m_maxZ;

    void Init(
        const CVector& pos,
        float vec1X, float vec1Y, float minZ,
        float vec2X, float vec2Y, float maxZ
    ) {
        m_cornerX = int16(pos.x - vec1X - vec2X);
        m_cornerY = int16(pos.y - vec1Y - vec2Y);

        m_vec1X = int16(vec1X + vec1X);
        m_vec1Y = int16(vec1Y + vec1Y);

        m_vec2X = int16(vec2X + vec2X);
        m_vec2Y = int16(vec2Y + vec2Y);

        m_minZ = int16(minZ);
        m_maxZ = int16(maxZ);
    }

    // 0x72D850
    [[nodiscard]] bool IsPointWithin(const CVector& point) const {
        if ((float)m_minZ >= point.z || (float)m_maxZ <= point.z) {
            return false;
        }

        float dx = point.x - (float)m_cornerX;
        float dy = point.y - (float)m_cornerY;

        float sqMag0 = (float)m_vec1X * dx + (float)m_vec1Y * dy;
        if (sqMag0 < 0.0f || sqMag0 > (sq(m_vec1Y) + sq(m_vec1X))) {
            return false;
        }

        float sqMag1 = (float)m_vec2X * dx + (float)m_vec2Y * dy;
        if (sqMag1 < 0.0f || sqMag1 > (sq(m_vec2Y) + sq(m_vec2X))) {
            return false;
        }

        return true;
    }
};

VALIDATE_SIZE(CZoneDef, 0x10);

// https://gtaforums.com/topic/202532-sadoc-ipl-definitions/page/5/?tab=comments#comment-3875562
struct CMirrorAttributeZone {
    CZoneDef zoneDef;
    float mirrorV; // mirror plane coordinate in direction axis.
    int8 mirrorNormalX, mirrorNormalY, mirrorNormalZ; // mirror direction (like [-1 0 0] or [0 1 0] or [0 0 -1])
    eMirrorFlags flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CMirrorAttributeZone, 0x18);

struct CAttributeZone {
    CZoneDef zoneDef;
    eZoneAttributes flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); }
};

VALIDATE_SIZE(CAttributeZone, 0x12);

// CULL is a section in the item placement file.
// It is used to create zones affecting the different behaviour of the world, like
// mirrors, fixed camera positions, disabling rain, police behaviors, etc.,
// when the player is within the zone.
class CCullZones {
public:
    static inline int32 NumAttributeZones; // 0xC87AC8
    static inline int32 NumMirrorAttributeZones; // 0xC87AC4
    static inline int32 NumTunnelAttributeZones; // 0xC87AC0

    static inline int32 CurrentFlags_Camera; // 0xC87ABC
    static inline int32 CurrentFlags_Player; // 0xC87AB8

    static inline bool bMilitaryZonesDisabled; // 0xC87ACD

    static inline CAttributeZone aAttributeZones[1'300]; // 0xC81F50
    static inline CAttributeZone aTunnelAttributeZones[40]; // 0xC81C80
    static inline CMirrorAttributeZone aMirrorAttributeZones[72]; // 0xC815C0

    static inline bool bRenderCullzones; // 0xC87ACC, unused

public:
    static void InjectHooks();

    static void Init();
    static void Update();

    static eZoneAttributes FindAttributesForCoors(CVector pos);
    static eZoneAttributes FindTunnelAttributesForCoors(CVector pos);
    static CMirrorAttributeZone* FindMirrorAttributesForCoors(CVector pos);
    static CAttributeZone* FindZoneWithStairsAttributeForPlayer();
    static void AddCullZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, uint16 flags);
    static void AddMirrorAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags, float mirrorV, float mirrorNormalX, float mirrorNormalY, float mirrorNormalZ);
    static void AddTunnelAttributeZone(const CVector& pos, float vec1X, float vec1Y, float minZ, float vec2X, float vec2Y, float maxZ, eZoneAttributes flags);

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
    static bool DoINeedToLoadCollision();

    static bool DoExtraAirResistanceForPlayer();
};
