#pragma once

enum eAdhesionGroup {
    ADHESION_GROUP_RUBBER = 0,
    ADHESION_GROUP_HARD,
    ADHESION_GROUP_ROAD,
    ADHESION_GROUP_LOOSE,
    ADHESION_GROUP_SAND,
    ADHESION_GROUP_WET,
};

enum eFrictionEffect {
    FRICTION_EFFECT_NONE = 0,
    FRICTION_EFFECT_SPARKS,
    FRICTION_EFFECT_MORE // not sure what this is, but it's probably not used
};

enum eBulletFX {
    BULLET_FX_NONE = 0,
    BULLET_FX_SPARKS,
    BULLET_FX_SAND,
    BULLET_FX_WOOD,
    BULLET_FX_DUST,
};

struct SurfaceInfo {
    char     SurfaceName[64];
    char     AdhesionGroup[32];
    float    TyreGrip;
    float    WetGrip;
    char     SkidMark[32];
    char     FrictionEffect[32];
    int32    SoftLand;
    int32    SeeThrough;
    int32    ShootThrough;
    int32    Sand;
    int32    Water;
    int32    ShallowWater;
    int32    Beach;
    int32    SteepSlope;
    int32    Glass;
    int32    Stairs;
    int32    Skateable;
    int32    Pavement;
    int32    Roughness;
    int32    Flame;
    int32    Sparks;
    int32    Sprint;
    int32    Footsteps;
    int32    FootDust;
    int32    CarDirt;
    int32    CarClean;
    int32    WheelGrass;
    int32    WheelGravel;
    int32    WheelMud;
    int32    WheelDust;
    int32    WheelSand;
    int32    WheelSpray;
    int32    ProcPlant;
    int32    ProcObj;
    int32    Climbable;
    char     BulletFx[32];

    void Read(const char* line) {
        // FIX_BUGS: Possible buffer overflow
        VERIFY(sscanf_s(
            line, "%s %s %f %f %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s",
            SCANF_S_STR(SurfaceName), SCANF_S_STR(AdhesionGroup),
            &TyreGrip, &WetGrip, SCANF_S_STR(SkidMark), SCANF_S_STR(FrictionEffect),
            &SoftLand, &SeeThrough, &ShootThrough, &Sand, &Water, &ShallowWater, &Beach, &SteepSlope, &Glass, &Stairs, &Skateable, &Pavement,
            &Roughness, &Flame, &Sparks, &Sprint, &Footsteps, &FootDust, &CarDirt, &CarClean, &WheelGrass, &WheelGravel, &WheelMud, &WheelDust, &WheelSand, &WheelSpray,
            &ProcPlant, &ProcObj, &Climbable,
            SCANF_S_STR(BulletFx)
        ) == 36);
    }
};

class SurfaceInfo_c {
public:
    int8 tyreGrip; // tyre grip - will override friction values for vehicle tyres on each material
    int8 wetGrip;  // wet multiplier on tyre grip
    union {
        struct {
            uint32 ucAdhesionGroup  : 3; // see eAdhesionGroup. Used to calculate friction between materials
            uint32 ucSkidmarkType   : 2; // see eSkidMarkType
            uint32 ucFrictionEffect : 3; // see eFrictionEffect

            uint32 ucBulletFX : 3;       // see eBulletFX. The effect that is played when a bullet hits this surface.
            uint32 bIsSoftLand : 1;
            uint32 bIsSeeThrough : 1;    // no camera collision
            uint32 bIsShootThrough : 1;  // bullets go through
            uint32 bIsSand : 1;          // car tyres sink in and can get bogged down
            uint32 bIsWater : 1;

            uint32 bIsShallowWater : 1;
            uint32 bIsBeach : 1;
            uint32 bIsSteepSlope : 1;   // code won't let player run or jump up slope - used to stop the player getting up a hill
            uint32 bIsGlass : 1;        // will shatter when shot
            uint32 bIsStairs : 1;       // don't tilt peds as if they're walking up a slope
            uint32 bIsSkateable : 1;
            uint32 bIsPavement : 1;     // used so peds know when it's safe to stop and chat, or when they're crossing a road
            uint32 ucRoughness : 2;     // (0:not, 1:quite, 2:rough, 3:very) (does pad vibration)
            uint32 ucFlammability : 2;  // (0:not, 1:flammable, 2:very)
            uint32 bCreatesSparks : 1;  // NOT USED ANYMORE
            uint32 bCantSprintOn : 1;
            uint32 bLeavesFootsteps : 1;
            uint32 bProducesFootDust : 1;
            uint32 bMakesCarDirty : 1;

            uint32 bMakesCarClean : 1;
            uint32 bCreatesWheelGrass : 1;
            uint32 bCreatesWheelGravel : 1;
            uint32 bCreatesWheelMud : 1;
            uint32 bCreatesWheelDust : 1;
            uint32 bCreatesWheelSand : 1;
            uint32 bCreatesWheelSpray : 1;
            uint32 bCreatesPlants : 1;

            uint32 bCreatesObjects : 1;
            uint32 bCanClimb : 1;
            uint32 bIsAudioConcrete : 1;
            uint32 bIsAudioGrass : 1;
            uint32 bIsAudioSand : 1;
            uint32 bIsAudioGravel : 1;
            uint32 bIsAudioWood : 1;
            uint32 bIsAudioWater : 1;

            uint32 bIsAudioMetal : 1;
            uint32 bIsAudioLongGrass : 1;
            uint32 bIsAudioTile : 1;
            uint32 bPad : 13;
        };
        uint32 m_nFlags1;
        uint32 m_nFlags2;
    };
};

VALIDATE_SIZE(SurfaceInfo_c, 0xC);
