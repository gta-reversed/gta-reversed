/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "WeaponEffects.h"

// Statics of this file (addresses are from the original executable)
static inline auto& s_afCrossHairScale  = StaticRef<std::array<float, MAX_NUM_WEAPON_CROSSHAIRS>>(0xC8A8A0);      // Per crosshair animated size offset
static inline auto& s_abPulseOutwards   = StaticRef<std::array<bool, MAX_NUM_WEAPON_CROSSHAIRS>>(0x8D6144);       // { true, true } - Whether the target's radius grows or shrinks
static inline auto& s_fFlightOffsetX    = StaticRef<float>(0xC8A89C);                                            // Flight (lock-on) crosshair offset from it's position
static inline auto& s_fFlightOffsetY    = StaticRef<float>(0xC8A898);
static inline auto& s_nTargetChangeTime = StaticRef<int32>(0xC8A890);                                            // Time (in ms) the lock-on target was last changed
static inline auto& s_pLastTarget       = StaticRef<CEntity*>(0xC8A894);                                         // The last lock-on target

void CWeaponEffects::InjectHooks() {
    RH_ScopedClass(CWeaponEffects);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x742AB0);
    RH_ScopedInstall(Shutdown, 0x742B80);
    RH_ScopedInstall(IsLockedOn, 0x742BD0);
    RH_ScopedInstall(MarkTarget, 0x742BF0);
    RH_ScopedInstall(ClearCrossHair, 0x742C60);
    RH_ScopedInstall(ClearCrossHairs, 0x742C80);
    RH_ScopedInstall(ClearCrossHairImmediately, 0x742CA0);
    RH_ScopedInstall(ClearCrossHairsImmediately, 0x742CC0);
    RH_ScopedInstall(Render, 0x742CF0, { .reversed = false });
}

// 0x742AB0
void CWeaponEffects::Init() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_vecPosn = CVector();
        crossHair.m_bActive = false;
        crossHair.m_color = CRGBA(255, 0, 0, 127);
        crossHair.m_fSize = 1.0f;
        crossHair.field_1C = 0;
        crossHair.m_nTimeWhenToDeactivate = 0;
        crossHair.m_bClearImmediately = false;
        crossHair.m_fRotation = 0.0f;
    }

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    gpCrossHairTex          = RwTextureRead("target256",  "target256m");
    gpCrossHairTexFlight[0] = RwTextureRead("lockon",     "lockonA");
    gpCrossHairTexFlight[1] = RwTextureRead("lockonFire", "lockonFireA");
    CTxdStore::PopCurrentTxd();
}

// 0x742B80
void CWeaponEffects::Shutdown() {
    for (auto& crossHair : gpCrossHairTexFlight) {
        RwTextureDestroy(crossHair);
        crossHair = nullptr;
    }

    RwTextureDestroy(gpCrossHairTex);
    gpCrossHairTex = nullptr;
}

// 0x742BD0
bool CWeaponEffects::IsLockedOn(CrossHairId id) {
    return gCrossHair[id].m_fRotation;
}

// 0x742BF0
void CWeaponEffects::MarkTarget(CrossHairId id, CVector posn, uint8 red, uint8 green, uint8 blue, uint8 alpha, float size, bool bClearImmediately) {
    auto& crossHair = gCrossHair[id];
    crossHair.m_vecPosn               = posn;
    crossHair.m_color.r               = red;
    crossHair.m_color.g               = green;
    crossHair.m_color.b               = blue;
    crossHair.m_color.a               = alpha;
    crossHair.m_bActive               = true;
    crossHair.m_fSize                 = size;
    crossHair.m_nTimeWhenToDeactivate = -1;
    crossHair.m_bClearImmediately     = bClearImmediately;
}

// 0x742C60
void CWeaponEffects::ClearCrossHair(CrossHairId id) {
    gCrossHair[id].m_nTimeWhenToDeactivate = static_cast<int32>(CTimer::GetTimeInMS() + 400);
}

// 0x742C80
void CWeaponEffects::ClearCrossHairs() {
    for (auto& crossHair : gCrossHair) {
        crossHair.m_bActive = false;
    }
}

// 0x742CA0
void CWeaponEffects::ClearCrossHairImmediately(CrossHairId id) {
    gCrossHair[id].m_nTimeWhenToDeactivate = static_cast<int32>(CTimer::GetTimeInMS() - 100);
    gCrossHair[id].m_bActive = false;
}

// 0x742CC0
void CWeaponEffects::ClearCrossHairsImmediately() {
    for (auto i = 0u; i < gCrossHair.size(); i++) {
        ClearCrossHair(i);
    }
}

// 0x742CF0
void CWeaponEffects::Render() {
    plugin::Call<0x742CF0>();
}
