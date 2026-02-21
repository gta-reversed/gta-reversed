/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "eWeaponType.h"

static inline RwTexture*& gpFinishFlagTex = StaticRef<RwTexture*>(0xC7C718);

class CSpecialFX {
public:
    static void Init();
    static void Update();
    static void Shutdown();
    static void AddWeaponStreak(eWeaponType weaponType);
    static void Render();
    static void Render2DFXs();
    static void ReplayStarted();

public:
    static inline auto& bVideoCam       = *(bool*)0xC7C70C;
    static inline auto& bLiftCam        = *(bool*)0xC7C70D;
    static inline auto& bSnapShotActive = *(bool*)0xC7C714;
    static inline auto& SnapShotFrames  = *(uint32*)0xC7C710;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
