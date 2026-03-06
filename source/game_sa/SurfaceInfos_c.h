#pragma once

#include "SurfaceInfo_c.h"

class CColPoint;

typedef uint32 SurfaceId;

class SurfaceInfos_c {
public:
    float m_adhesiveLimits[6][6];
    // fuck c*, they did shit
    // sizeof of this class exactly 0x8F4, but then they enable *magic*
    // and do m_surfaces[id < 195]
    // todo: std::array<SurfaceInfo_c, TOTAL_NUM_SURFACE_TYPES> m_surfaces;
    SurfaceInfo_c m_surfaces[TOTAL_NUM_SURFACE_TYPES];

    static constexpr const char* cDefaultName = "DEFAULT"; // 0x85C658 😏

public:
    static void InjectHooks();

    SurfaceId GetSurfaceIdFromName(const char* cName);
    void LoadAdhesiveLimits();
    void LoadSurfaceAudioInfos();
    void LoadSurfaceInfos();
    void Init();
    eAdhesionGroup GetAdhesionGroup(SurfaceId id);
    float GetTyreGrip(SurfaceId id);
    float GetWetMultiplier(SurfaceId id);
    uint32 GetSkidmarkType(SurfaceId id);
    eFrictionEffect GetFrictionEffect(SurfaceId id);
    uint32 GetBulletFx(SurfaceId id);
    bool IsSoftLanding(SurfaceId id);
    bool IsSeeThrough(SurfaceId id);
    bool IsShootThrough(SurfaceId id);
    bool IsSand(SurfaceId id);
    bool IsWater(SurfaceId id);
    bool IsShallowWater(SurfaceId id);
    bool IsBeach(SurfaceId id);
    bool IsSteepSlope(SurfaceId id);
    bool IsGlass(SurfaceId id);
    bool IsStairs(SurfaceId id);
    bool IsSkateable(SurfaceId id);
    bool IsPavement(SurfaceId id);
    uint32 GetRoughness(SurfaceId id);
    uint32 GetFlammability(SurfaceId id);
    bool CreatesSparks(SurfaceId id);
    bool CantSprintOn(SurfaceId id);
    bool LeavesFootsteps(SurfaceId id);
    bool ProducesFootDust(SurfaceId id);
    bool MakesCarDirty(SurfaceId id);
    bool MakesCarClean(SurfaceId id);
    bool CreatesWheelGrass(SurfaceId id);
    bool CreatesWheelGravel(SurfaceId id);
    bool CreatesWheelMud(SurfaceId id);
    bool CreatesWheelDust(SurfaceId id);
    bool CreatesWheelSand(SurfaceId id);
    bool CreatesWheelSpray(SurfaceId id);
    bool CreatesPlants(SurfaceId id);
    bool CreatesObjects(SurfaceId id);
    bool CanClimb(SurfaceId id);
    bool IsAudioConcrete(SurfaceId id);
    bool IsAudioGrass(SurfaceId id);
    bool IsAudioSand(SurfaceId id);
    bool IsAudioGravel(SurfaceId id);
    bool IsAudioWood(SurfaceId id);
    bool IsAudioWater(SurfaceId id);
    bool IsAudioMetal(SurfaceId id);
    bool IsAudioLongGrass(SurfaceId id);
    bool IsAudioTile(SurfaceId id);
    float GetAdhesiveLimit(CColPoint* colPoint);

    bool IsAudioGravelConcreteOrTile(SurfaceId id) {
        return IsAudioGravel(id) || IsAudioConcrete(id) || IsAudioTile(id);
    }
};
VALIDATE_SIZE(SurfaceInfos_c, 0x8F4);

static inline auto& g_surfaceInfos = StaticRef<SurfaceInfos_c>(0xB79538);
