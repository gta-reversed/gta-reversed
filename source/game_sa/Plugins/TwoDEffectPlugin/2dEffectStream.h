#pragma once

// RW PLUGIN
// vvv https://gtamods.com/wiki/2d_Effect_(RW_Section)
struct t2dEffectPluginEntry {
    uint32    m_nObjCount;
    C2dEffect m_pObjects[];
};
struct t2dEffectPlugin {
    t2dEffectPluginEntry* m_pEffectEntries;
};
VALIDATE_SIZE(t2dEffectPlugin, 0x4);

struct t2dEffectLightStreamData {
    static constexpr auto SizeOfRequiredFields = 80 - 4;

    RwRGBA             Color;
    float              CoronaFarClip;
    float              PointlightRange;
    float              CoronaSize;
    float              ShadowSize;
    e2dCoronaFlashType CoronaFlashType;
    uint8              CoronaEnableReflection;
    uint8              CoronaFlareType;
    uint8              ShadowColorMultiplier;
    uint8              Flags1;
    RwChar             CoronaName[24];
    RwChar             ShadowName[24];
    uint8              ShadowZDistance;
    uint8              Flags2;

    // vvv optional fields vvv
    uint8              OffsetX;
    uint8              OffsetY;
    uint8              OffsetZ;
};
static_assert(std::is_trivially_copyable_v<t2dEffectLightStreamData>);
VALIDATE_SIZE(t2dEffectLightStreamData, 80);

struct t2dEffectRoadsignStreamData {
    RwV2d              Size;
    RwV3d              Rotation;
    CRoadsignAttrFlags Flags;
    RwChar             Text[4][16];
};
static_assert(std::is_trivially_copyable_v<t2dEffectRoadsignStreamData>);
VALIDATE_SIZE(t2dEffectRoadsignStreamData, 88);

struct t2dEffectEnExStreamData {
    static constexpr auto SizeOfRequiredFields = 44 - 4;

    float  EnterAngle;
    RwV2d  Radius;
    RwV3d  ExitPos;
    float  ExitAngle;
    int16  InteriorId;
    int8   Flags1;
    int8   SkyColor;
    RwChar InteriorName[8];

    // vvv optional fields vvv
    uint8  TimeOn;
    uint8  TimeOff;
    uint8  Flags2;
};
static_assert(std::is_trivially_copyable_v<t2dEffectEnExStreamData>);
VALIDATE_SIZE(t2dEffectEnExStreamData, 44);

struct t2dEffectCoverPointStreamData {
    RwV2d               Dir;
    CCoverPoint::eUsage Usage;
};
static_assert(std::is_trivially_copyable_v<t2dEffectCoverPointStreamData>);
VALIDATE_SIZE(t2dEffectCoverPointStreamData, 12);

struct t2dEffectSlotMachineWheelStreamData {
    int32 PointId;
};
static_assert(std::is_trivially_copyable_v<t2dEffectSlotMachineWheelStreamData>);
VALIDATE_SIZE(t2dEffectSlotMachineWheelStreamData, 4);

struct t2dEffectEscalatorStreamData {
    RwV3d Bottom;
    RwV3d Top;
    RwV3d End; // End.Z, matches Top.Z if goes up, Bottom.Z if goes down.
    uint8 Direction; // 0 - down, 1 - up
};
static_assert(std::is_trivially_copyable_v<t2dEffectEscalatorStreamData>);
VALIDATE_SIZE(t2dEffectEscalatorStreamData, 40);

struct t2dEffectPedAttractorStreamData {
    ePedAttractorType Type;
    RwV3d             QueueDir;
    RwV3d             UseDir;
    RwV3d             ForwardDir;
    RwChar            ExternalScriptName[8];
    int32             PedExistingProbability;
    uint8             unk1;
    uint8             __not_used1;
    uint8             unk2;
    uint8             __not_used2;
};
static_assert(std::is_trivially_copyable_v<t2dEffectPedAttractorStreamData>);
VALIDATE_SIZE(t2dEffectPedAttractorStreamData, 56);

using t2dEffectParticleStreamData = RwChar[24];
static_assert(std::is_trivially_copyable_v<t2dEffectParticleStreamData>);
VALIDATE_SIZE(t2dEffectParticleStreamData, 24);
