#pragma once

#include "Vehicle.h"

enum eTrafficLightsState {
    LIGHT_GREEN = 0,
    LIGHT_YELLOW = 1,
    LIGHT_RED = 2,
    LIGHT_OFF = 3
};

enum eTrafficLightsDirection {
    DIR_DISABLED = 0,
    DIR_NORTH_SOUTH = 1,
    DIR_WEST_EAST = 2
};

class CTrafficLights {
private:
    static inline auto& aTrafficLightColoursR = StaticRef<uint8[3]>(0x8A6214);
    static inline auto& aTrafficLightColoursG = StaticRef<uint8[3]>(0x8A6218);
    static inline auto& aTrafficLightColoursB = StaticRef<uint8[3]>(0xA9AD94);
    static inline auto& fLightMult = StaticRef<float>(0x8A621C);

    static inline auto& vecTrafficLights5_1 = StaticRef<CVector>(0xA9ADD4);
    static inline auto& vecTrafficLights5_2 = StaticRef<CVector>(0xA9ADC8);
    static inline auto& vecTrafficLights_1 = StaticRef<CVector>(0xA9ADBC);
    static inline auto& vecTrafficLights_2 = StaticRef<CVector>(0xA9ADB0);
    static inline auto& uiPedLightFlags = StaticRef<uint32>(0xA9ADE0);

public:
    static void InjectHooks();

public:
    static bool ShouldCarStopForLight(CVehicle* vehicle, bool bUnkn);
    static bool ShouldCarStopForBridge(CVehicle* vehicle);
    static void DisplayActualLight(CEntity* entity);
    static bool IsMITrafficLight(int32 modelIndex);
    static uint8 LightForPeds();

private:
    static uint8 LightForCars1();
    static uint8 LightForCars2();
    static uint8 LightForCars1_Visual();
    static uint8 LightForCars2_Visual();
    static int32 FindTrafficLightType(CEntity* entity);
    static float FindOrientationForTrafficLightType(CEntity* entity);
    static int32 FindTrafficLightTypeFromOrientation(float fOrientation);
};
