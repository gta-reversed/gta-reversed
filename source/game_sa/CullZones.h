#pragma once

#include "Vector.h"

// https://gtamods.com/wiki/CULL

/* 
ZONE_ATTRIBUTE_CAMCLOSEIN_FORPLAYER:
    Camera close in into player using closest third-person view camera mode,
    does not close in if in first person or cinematic mode,
    camera mode cannot be changed while in the zone.
ZONE_ATTRIBUTE_CAMSTAIRS_FORPLAYER:
    Camera remotely placed outside the zone, no control of camera,
    camera mode cannot be changed while in the zone.
ZONE_ATTRIBUTE_CAM1STPERSONS_FORPLAYER:
    Lowers the camera angle on boats.
ZONE_ATTRIBUTE_CAMNORAIN:
    Rain-free, police helicopter-free zone.
ZONE_ATTRIBUTE_POLICES_ABANDONCARS:
    Police will not exit their vehicles voluntarily.
    They will only exit if you do something to them.
ZONE_ATTRIBUTE_INROOMS_FORAUDIO:
    Some visual ocean water effects are removed.
ZONE_ATTRIBUTE_MILITARYZONE:
    5-Star Military zone.
ZONE_ATTRIBUTE_EXTRA_AIRRESISTANCE: 
    PS2 Optimization hack: Doesn't allow cars to reach top speed.
ZONE_ATTRIBUTE_FEWERCARS:
    Spawn fewer cars in this area.

NOTSA:
ZONE_ATTRIBUTE_INROOMS_FEWERPEDS:
    Fewer pedestrians in the area. NOTSA but used in the game. ??
ZONE_ATTRIBUTE_UNKNOWN_1 & ZONE_ATTRIBUTE_UNKNOWN_2:
    Unknown. Related to tunnels.
ZONE_ATTRIBUTE_CAMNONE:
    No special cull attributes.
*/

enum eZoneAttributes : uint16 {
    ZONE_ATTRIBUTE_CAMNONE                 = 0x0, // NOTSA
    ZONE_ATTRIBUTE_CAMCLOSEIN_FORPLAYER    = 0x1,
    ZONE_ATTRIBUTE_CAMSTAIRS_FORPLAYER     = 0x2,
    ZONE_ATTRIBUTE_CAM1STPERSONS_FORPLAYER = 0x4,
    ZONE_ATTRIBUTE_CAMNORAIN               = 0x8,
    ZONE_ATTRIBUTE_UNKNOWN_1               = 0x80,
    ZONE_ATTRIBUTE_POLICES_ABANDONCARS     = 0x100,
    ZONE_ATTRIBUTE_INROOMS_FORAUDIO        = 0x200,
    ZONE_ATTRIBUTE_INROOMS_FEWERPEDS       = 0x400, // ? NOTSA
    ZONE_ATTRIBUTE_UNKNOWN_2               = 0x800,
    ZONE_ATTRIBUTE_MILITARYZONE            = 0x1000,
    ZONE_ATTRIBUTE_EXTRA_AIRRESISTANCE     = 0x4000,
    ZONE_ATTRIBUTE_FEWERCARS               = 0x8000,
};

struct CZoneDef {
    int16 m_x1; // start X
    int16 m_y1; // start Y

    int16 m_x2;
    int16 m_lenY;

    int16 m_lenX;
    int16 m_y3;

    int16 bottomZ;
    int16 topZ;

    void Init(
        const CVector& center,
        float x12, float fWidthY,
        float fBottomZ,
        float fWidthX, float y13,
        float fTopZ
    ) {
        m_x1    = (int16)(center.x - x12 - fWidthX);
        m_y1    = (int16)(center.y - fWidthY - y13);

        m_x2    = (int16)(x12 + x12);
        m_lenY  = (int16)(fWidthY + fWidthY);

        m_lenX  = (int16)(fWidthX + fWidthX);
        m_y3    = (int16)(y13 + y13);

        bottomZ = (int16)(fBottomZ);
        topZ    = (int16)(fTopZ);
    }

    // 0x72D850
    [[nodiscard]] bool IsPointWithin(const CVector& point) const {
        if ((float)bottomZ >= point.z || (float)topZ <= point.z)
            return false;

        float dx = point.x - (float)m_x1;
        float dy = point.y - (float)m_y1;

        float sqMag0 = (float)m_x2 * dx + (float)m_lenY * dy;
        if (sqMag0 < 0.0f || sqMag0 > (sq(m_lenY) + sq(m_x2)))
            return false;

        float sqMag1 = (float)m_lenX * dx + (float)m_y3 * dy;
        if (sqMag1 < 0.0f || sqMag1 > (sq(m_y3) + sq(m_lenX)))
            return false;

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

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); };
};
VALIDATE_SIZE(CCullZoneReflection, 0x18);

struct CCullZone {
    CZoneDef zoneDef;
    eZoneAttributes flags;

    [[nodiscard]] bool IsPointWithin(const CVector& point) const { return zoneDef.IsPointWithin(point); };
};
VALIDATE_SIZE(CCullZone, 0x12);

// CULL is a section in the item placement file.
// It is used to create zones affecting the different behaviour of the world, like
// mirrors, fixed camera positions, disabling rain, police behaviors, etc.,
// when the player is within the zone.
class CCullZones {
public:
    static inline int32& NumMirrorAttributeZones = *(int32*)0xC87AC4;
    static inline CCullZoneReflection (&aMirrorAttributeZones)[72] = *(CCullZoneReflection(*)[72])0xC815C0;

    static inline int32& NumTunnelAttributeZones = *(int32*)0xC87AC0;
    static inline CCullZone (&aTunnelAttributeZones)[40] = *(CCullZone(*)[40])0xC81C80;

    static inline int32& NumAttributeZones = *(int32*)0xC87AC8;
    static inline CCullZone (&aAttributeZones)[1300] = *(CCullZone(*)[1300])0xC81F50;

    static inline int32& CurrentFlags_Player = *(int32*)0xC87AB8;
    static inline int32& CurrentFlags_Camera = *(int32*)0xC87ABC;

    static inline bool& bMilitaryZonesDisabled = *(bool*)0xC87ACD;

public:
    static void InjectHooks();

    static void Init();
    static void Update();

    static void AddCullZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags);
    static void AddTunnelAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, uint16 flags);
    static void AddMirrorAttributeZone(const CVector& center, float unk1, float fWidthY, float fBottomZ, float fWidthX, float unk2, float fTopZ, eZoneAttributes flags, float cm, float vX, float vY, float vZ);

    // Flag check functions
    static bool CamCloseInForPlayer();
    static bool CamStairsForPlayer();
    static bool Cam1stPersonForPlayer();
    static bool PoliceAbandonCars();
    static bool InRoomForAudio();
    static bool FewerCars();
    static bool CamNoRain();
    static bool PlayerNoRain();
    static bool FewerPeds();
    static bool NoPolice();
    static bool DoExtraAirResistanceForPlayer();

    static eZoneAttributes FindTunnelAttributesForCoors(CVector point);
    static CCullZoneReflection* FindMirrorAttributesForCoors(CVector cameraPosition);
    static CCullZone* FindZoneWithStairsAttributeForPlayer();
    static eZoneAttributes FindAttributesForCoors(CVector pos);


};

[[nodiscard]] bool IsPointWithinArbitraryArea(const CVector2D& TestPoint, const CVector2D& Point1, const CVector2D& Point2, const CVector2D& Point3, const CVector2D& Point4);
