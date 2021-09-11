/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"

class CSpecialFX {
public:
    static bool& bVideoCam;
    static bool& bLiftCam;
    static uint32& SnapShotFrames;
    static bool& bSnapShotActive;

public:
    static void InjectHooks();

    static void Init();
    static void Update();
    static void Shutdown();
    static void AddWeaponStreak(eWeaponType weaponType);
    static void Render();
    static void Render2DFXs();
    static void ReplayStarted();
};

extern RwTexture*& gpFinishFlagTex;