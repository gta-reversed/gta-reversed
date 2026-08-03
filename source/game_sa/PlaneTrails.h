/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PlaneTrail.h"

class CPlaneTrails {
public:
    static inline auto& aArray = StaticRef<std::array<CPlaneTrail, 3>>(0xC713A8);

public:
    static void InjectHooks();

public:
    static void Init();
    static void Render();
    static void RegisterPoint(CVector point, uint32 trailIndex);
    static void Update();
};
