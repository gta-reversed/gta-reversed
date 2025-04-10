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
    static inline auto& SunBlockedByClouds = StaticRef<bool, 0xC3E030>();
    // change coronas brightness immediately
    static bool& bChangeBrightnessImmediately;
    // num of registered coronas in frame
    static uint32& NumCoronas;
    // coronas intensity multiplier
    static float& LightsMult;
    // this is used to control moon size when you shooting it with sniper
    static uint32& MoonSize;
    // Coronas array. count: MAX_NUM_CORONAS (default: 64)
    static constexpr int32 MAX_NUM_CORONAS = 64;
    static CRegisteredCorona (&aCoronas)[MAX_NUM_CORONAS];

    static uint16 (&ms_aEntityLightsOffsets)[8];

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

    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, RwTexture* texture, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);
    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 red, uint8 green, uint8 blue, uint8 alpha, const CVector& posn,
                               float radius, float farClip, eCoronaType coronaType, eCoronaFlareType flareType, bool enableReflection, bool checkObstacles, int32 _param_not_used,
                               float angle, bool longDistance, float nearClip, uint8 fadeState, float fadeSpeed, bool onlyFromBelow, bool reflectionDelay);

    static void UpdateCoronaCoors(uint32 id, const CVector& posn, float farClip, float angle);
    static void DoSunAndMoon();
};

extern uint32 MAX_CORONAS;
constexpr int32 CORONA_TEXTURES_COUNT = 10;
extern RwTexture* (&gpCoronaTexture)[CORONA_TEXTURES_COUNT];
