/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <Base.h>
#include "Vector.h"

class CEntity;

enum ePointLightType : uint8 {
    PLTYPE_POINTLIGHT = 0,
    PLTYPE_SPOTLIGHT = 1,
    PLTYPE_DARKLIGHT = 2,
};

class CPointLight {
public:
    CVector         m_vecPosn;
    CVector         m_vecDirection;
    float           m_fRadius;
    float           m_fColorRed;
    float           m_fColorGreen;
    float           m_fColorBlue;
    CEntity*        m_pEntityToLight;
    ePointLightType m_nType;
    uint8           m_nFogType;
    bool            m_bGenerateShadows;
};
VALIDATE_SIZE(CPointLight, 0x30);

static constexpr auto MAX_POINT_LIGHTS = 32;

class CPointLights {
public:
    static inline uint32& NumLights = *(uint32*)0xC3F0D0; // num of registered lights in frame
    static inline CPointLight (&aLights)[MAX_POINT_LIGHTS] = *(CPointLight(*)[MAX_POINT_LIGHTS])0xC3F0E0;

    static inline float (&aCachedMapReadResults)[MAX_POINT_LIGHTS] = *(float(*)[MAX_POINT_LIGHTS])0xC3F050;
    static inline int32& NextCachedValue = *(int32*)0xC3F0D4;
    static inline CVector (&aCachedMapReads)[MAX_POINT_LIGHTS] = *(CVector(*)[MAX_POINT_LIGHTS])0xC3F6E0;

public:
    static void  Init();
    static float GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity);
    static float GetLightMultiplier(const CVector* point);
    static void  RemoveLightsAffectingObject();
    static bool  ProcessVerticalLineUsingCache(CVector point, float* outZ);
    static void  AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType = 0, bool generateExtraShadows = false, CEntity* entityAffected = nullptr);
    static void  RenderFogEffect();

    static void ResetNumLights() { NumLights = 0; }
    static auto GetActiveLights() { return aLights | rng::views::take(NumLights); }
};
