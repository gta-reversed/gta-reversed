/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class CCreepingFire {
public:
    static inline auto (&m_aFireStatus)[32][32] = StaticRef<uint8[32][32]>(0xB71B68);

public:
    static void InjectHooks();

    static void SetReadyToBurn();
    static void Update();
    static bool TryToStartFireAtCoors(CVector pos, uint8 nGenerations, bool a5, bool bScriptFire, float fDistance);
};
