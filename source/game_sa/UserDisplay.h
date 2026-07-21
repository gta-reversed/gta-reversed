/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CPlaceName;
#include "OnscreenTimer.h"
class CCurrentVehicle;

class CUserDisplay {
public:
    static inline auto& PlaceName = StaticRef<CPlaceName>(0xBA18F4);
    static inline auto& OnscnTimer = StaticRef<COnscreenTimer>(0xBA1788);
    static inline auto& CurrentVehicle = StaticRef<CCurrentVehicle>(0xBA18FC);

public:
    static void InjectHooks();

    static void Init();
    static void Process();
};
