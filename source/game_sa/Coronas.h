/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RegisteredCorona.h"

constexpr auto MAX_NUM_CORONAS = 64;

struct CFlareDefinition {
    float                       Position;
    float                       Size;
    FixedVector<int16, 65535.f> ColorMult;
    FixedFloat<int16, 256.f>    IntensityMult;
    int16                       Sprite; // Only used for array-end checking
};

class CCoronas {
public:
    static inline float LightsMult = 1.0f; // 0x8D4B5C
    static inline float SunScreenX, SunScreenY; // 0xC3E028, 0xC3E02C
    // are there any obstacles between sun and camera
    static inline bool SunBlockedByClouds; // 0xC3E030
    // frame counter for immediate corona brightness updates after camera turn (3-frame duration).
    static inline int32 bChangeBrightnessImmediately; // 0xC3E034
    // coronas intensity multiplier
    // this is used to control moon size when you shooting it with sniper
    static inline uint32 MoonSize = 3; // 0x8D4B60
    // num of registered coronas in frame
    static inline uint32 NumCoronas; // 0xC3E038

    static inline std::array<CRegisteredCorona, MAX_NUM_CORONAS> aCoronas; // 0xC3E058
   
    inline static struct { // NOTSA
        bool DisableWetRoadReflections;
        bool AlwaysRenderWetRoadReflections; // Ignored if if `DisableReflections == false`
    } s_DebugSettings{};

public:
    static void Update();
    // Renders the registered coronas
    static void Render();

    // Renders registered coronas reflections on a wet roads ground
    static void RenderReflections();
    static void RenderOutGeometryBufferForReflections();
    // Renders sun's reflection on the water [sea]
    static void RenderSunReflection();
    static void Init();
    static void Shutdown();

    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 r, uint8 g, uint8 b, uint8 intensity,
                               const CVector& pos, float size, float range,
                               eCoronaType coronaType, eCoronaFlareType flareType, eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails,
                               float normalAngle, bool neonFade,
                               float pullTowardsCam, bool fullBrightAtStart,
                               float fadeSpeed, bool onlyFromBelow, bool whiteCore);

    static void RegisterCorona(uint32 id, CEntity* attachTo, uint8 r, uint8 g, uint8 b, uint8 intensity,
                               const CVector& pos, float size, float range,
                               RwTexture* texture, eCoronaFlareType flareType, eCoronaReflType reflType, eCoronaLOSCheck checkLOS, eCoronaTrail usesTrails,
                               float normalAngle, bool neonFade,
                               float pullTowardsCam, bool fullBrightAtStart,
                               float fadeSpeed, bool onlyFromBelow, bool whiteCore);


    static void UpdateCoronaCoors(uint32 id, const CVector& pos, float range, float normalAngle);
    static void DoSunAndMoon();

    // NOTSA:

    static void InjectHooks();

    // Inlined
    static CRegisteredCorona* GetCoronaByID(int32 id);
    // Inlined
    static CRegisteredCorona* GetFree();
};

inline std::array<RwTexture*, eCoronaType::CORONATYPE_COUNT> gpCoronaTexture; // 0xC3E000, in source file
