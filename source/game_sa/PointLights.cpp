/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"

// 0x6FFB40
void CPointLights::Init() {
    rng::fill(aCachedMapReadResults, 0.0f);
    NextCachedValue = 0;
    rng::fill(aCachedMapReads, CVector{});
}

// 0x6FFBB0
float CPointLights::GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity) {
    return plugin::CallAndReturn<float, 0x6FFBB0, const CVector*, float*, CEntity*>(point, totalLighting, entity);
}

// 0x6FFE70
float CPointLights::GetLightMultiplier(const CVector* point) {
    return plugin::CallAndReturn<float, 0x6FFE70, const CVector*>(point);
}

// 0x6FFFE0
void CPointLights::RemoveLightsAffectingObject() {
    plugin::Call<0x6FFFE0>();
}

// 0x6FFFF0
bool CPointLights::ProcessVerticalLineUsingCache(CVector point, float* outZ) {
    return plugin::CallAndReturn<bool, 0x6FFFF0, CVector, float*>(point, outZ);
}

// 0x7000E0
void CPointLights::AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType, bool generateExtraShadows, CEntity* entityAffected) {
    const CVector delta   = point - TheCamera.GetPosition();
    const float   maxDist = radius + 15.0f;
    if (delta.x >= maxDist || -maxDist >= delta.x || delta.y >= maxDist || -maxDist >= delta.y) {
        return;
    }
    if (NumLights >= MAX_POINT_LIGHTS) {
        return;
    }
    const float dist = delta.Magnitude();
    if (dist >= maxDist) {
        return;
    }

    auto& light      = aLights[NumLights++];
    light.m_nType    = static_cast<ePointLightType>(lightType);
    light.m_nFogType = fogType;

    light.m_vecPosn      = point;
    light.m_vecDirection = direction;
    light.m_fRadius      = radius;

    light.m_bGenerateShadows = generateExtraShadows;
    light.m_pEntityToLight   = entityAffected;

    // Fade color out starting at 75% of the max distance
    if (const float fade = 1.0f - (dist / maxDist - 0.75f) * 4.0f; fade < 1.0f) {
        light.m_fColorRed   = red * fade;
        light.m_fColorGreen = green * fade;
        light.m_fColorBlue  = blue * fade;
    } else {
        light.m_fColorRed   = red;
        light.m_fColorGreen = green;
        light.m_fColorBlue  = blue;
    }
}

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    plugin::Call<0x7002D0>();
}
