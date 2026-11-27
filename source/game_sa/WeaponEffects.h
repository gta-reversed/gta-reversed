/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "RGBA.h"

typedef int32 CrossHairId;

enum eWeaponEffectsLockTexture {
    WEAPONEFFECTS_LOCK_ON = 0,
    WEAPONEFFECTS_LOCK_ON_FIRE = 1
};

class CWeaponEffects {
public:
    static constexpr uint32 CROSSHAIR_MARKED = (uint32)(-1); //!< Value of `m_TimeWhenToDeactivate` for a crosshair that was marked (And isn't being cleared)

public:
    CWeaponEffects() = default;  // 0x742A90
    ~CWeaponEffects() = default; // 0x742AA0

    static void Init();
    static void Shutdown();
    static bool IsLockedOn(CrossHairId id);
    static void MarkTarget(CrossHairId id, CVector pos, uint8 red, uint8 green, uint8 blue, uint8 alpha, float size, bool isFlightCrossHair);
    static void ClearCrossHair(CrossHairId id);
    static void ClearCrossHairs();
    static void ClearCrossHairImmediately(CrossHairId id);
    static void ClearCrossHairsImmediately();
    static void Render();

    static void InjectHooks();

public:
    bool    m_IsActive{};
    uint32  m_TimeWhenToDeactivate{}; //!< 0: Not set, `CROSSHAIR_MARKED`: Marked
    CVector m_Pos{};
    CRGBA   m_Color{};
    float   m_Size{};
    float   m_Rotation{};          //!< Of the 3 triangles
    float   m_LockOnDist{};        //!< Distance of the lock-on sprite from the target on the screen (Flight crosshair)
    float   m_IsLockedOn{};        //!< 1 if locked on, 0 otherwise
    bool    m_IsFlightCrossHair{}; //!< Drawn with the lock-on sprites (Heat-seeking missiles) instead of the 3 triangles
};

VALIDATE_SIZE(CWeaponEffects, 0x2C);

constexpr auto MAX_NUM_WEAPON_CROSSHAIRS{ 2u };
static inline auto& gCrossHair = StaticRef<std::array<CWeaponEffects, MAX_NUM_WEAPON_CROSSHAIRS>>(0xC8A838);
static inline auto& gpCrossHairTex = StaticRef<RwTexture*>(0xC8A818);
static inline auto& gpCrossHairTexFlight = StaticRef<RwTexture*[2]>(0xC8A810);
