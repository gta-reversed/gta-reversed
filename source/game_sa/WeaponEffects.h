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
    bool    m_bActive;
    int32   m_nTimeWhenToDeactivate; // -1 default
    CVector m_vecPosn;
    CRGBA   m_color;
    float   m_fSize;
    int32   field_1C;
    int32   field_20;
    float   m_fRotation;
    bool    m_bClearImmediately;

public:
    static void InjectHooks();

    CWeaponEffects() = default;  // 0x742A90
    ~CWeaponEffects() = default; // 0x742AA0

    static void Init();
    static void Shutdown();
    static bool IsLockedOn(CrossHairId id);
    static void MarkTarget(CrossHairId id, CVector posn, uint8 red, uint8 green, uint8 blue, uint8 alpha, float size, bool bClearImmediately);
    static void ClearCrossHair(CrossHairId id);
    static void ClearCrossHairs();
    static void ClearCrossHairImmediately(CrossHairId id);
    static void ClearCrossHairsImmediately();
    static void Render();
};

VALIDATE_SIZE(CWeaponEffects, 0x2C);

constexpr auto MAX_NUM_WEAPON_CROSSHAIRS{ 2u };
static inline auto& gCrossHair = StaticRef<std::array<CWeaponEffects, MAX_NUM_WEAPON_CROSSHAIRS>>(0xC8A838);
static inline auto& gpCrossHairTex = StaticRef<RwTexture*>(0xC8A818);
static inline auto& gpCrossHairTexFlight = StaticRef<RwTexture*[2]>(0xC8A810);
