/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RegisteredCorona.h"

class CCoronas {
public:
    // are there any obstacles between sun and camera
    static inline auto& SunBlockedByClouds = StaticRef<bool>(0xC3E030);

    // change coronas brightness immediately (TODO: Most likely some enum type)
    static inline auto& bChangeBrightnessImmediately = StaticRef<uint8>(0xC3E034);

    // num of registered coronas in frame
    static inline auto& NumCoronas = StaticRef<uint32>(0xC3E038);

    // coronas intensity multiplier
    // this is used to control moon size when you shooting it with sniper
    static inline auto& MoonSize = StaticRef<uint32>(0x8D4B60); // 3

    static inline auto& SunScreenY = StaticRef<float>(0xC3E02C); // unused
    static inline auto& SunScreenX = StaticRef<float>(0xC3E028); // unused
    static inline auto& LightsMult = StaticRef<float>(0x8D4B5C); // 1.0f
    static inline auto& ms_aEntityLightsOffsets = StaticRef<std::array<uint16, 8>>(0x8D5028);

    // Coronas array. count: MAX_NUM_CORONAS (default: 64)
    static inline auto& aCoronas = StaticRef<std::array<CRegisteredCorona, 64>>(0xC3E058);
   
    inline static struct { // NOTSA
        bool DisableWetRoadReflections;
        bool AlwaysRenderWetRoadReflections; // Ignored if if `DisableReflections == false`
    } s_DebugSettings{};

public:
    static void InjectHooks();

    static void Init();
    static void Shutdown();
    static void Update();

    /*!
    * @addr 0x6FAEC0
    * Renders the registered coronas
    */
    static void Render();

    /*!
    * @addr 0x6FB630
    * Renders registered coronas reflections on a wet roads ground
    */
    static void RenderReflections();

    /*!
    * @addr 0x6FBAA0
    * Renders sun's reflection on the water [sea]
    */
    static void RenderSunReflection();

    // TODO: var r, g, b, a -> class RGBA
    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

    static void UpdateCoronaCoors(uint32 id, const CVector& posn, float farClip, float angle);
    static void DoSunAndMoon();
    // NOTSA
    static CRegisteredCorona* GetCoronaByID(int32 id);
    // NOTSA
    static CRegisteredCorona* GetFree();
};

inline auto& gpCoronaTexture = StaticRef<std::array<RwTexture*, eCoronaType::CORONATYPE_COUNT-->>(0xC3E000);

