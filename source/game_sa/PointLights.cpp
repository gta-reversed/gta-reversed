/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"

// Extra directional lights appended to the world this frame (up to 4 outdoors, 6 in interiors)
static inline auto& s_ExtraDirectionalLights         = StaticRef<std::array<RpLight*, 6>>(0xC886F0);
static inline auto& s_ExtraDirectionalLightStrengths = StaticRef<std::array<float, 6>>(0xC8867C);
static inline auto& s_NumExtraDirLightsInWorld       = StaticRef<int32>(0xC88708);

// 0x735840
static void AddAnExtraDirectionalLight(RpWorld* world, float dirx, float diry, float dirz, float red, float green, float blue) {
    const float strength = std::max({ red, green, blue });
    const int32 nLights  = CGame::currArea != AREA_CODE_NORMAL_WORLD ? 6 : 4;

    int32 slot;
    if (s_NumExtraDirLightsInWorld < nLights) {
        slot = s_NumExtraDirLightsInWorld;
    } else {
        slot  = -1;
        float weakest = strength;
        for (int32 i = 0; i < nLights; i++) {
            if (s_ExtraDirectionalLightStrengths[i] < weakest) {
                weakest = s_ExtraDirectionalLightStrengths[i];
                slot    = i;
            }
        }
    }
    if (slot < 0) {
        return;
    }

    const RwRGBAReal color{ red, green, blue };
    auto*            light = s_ExtraDirectionalLights[slot];
    RpLightSetColor(light, &color);

    const auto frame                           = RpLightGetFrame(light);
    RwMatrixGetAt(RwFrameGetMatrix(frame))     = { -dirx, -diry, -dirz };
    RwMatrixUpdate(RwFrameGetMatrix(frame));
    RwFrameUpdateObjects(frame);
    RpLightSetFlags(light, rpLIGHTLIGHTATOMICS);

    s_ExtraDirectionalLightStrengths[slot] = strength;
    s_NumExtraDirLightsInWorld             = std::min(s_NumExtraDirLightsInWorld + 1, nLights);
}

// 0x7359E0
static void RemoveExtraDirectionalLights(RpWorld* world) {
    for (const auto light : s_ExtraDirectionalLights) {
        RpLightSetFlags(light, 0);
    }
    s_NumExtraDirLightsInWorld = 0;
}

// 0x6FFB40
void CPointLights::Init() {
    rng::fill(aCachedMapReadResults, 0.0f);
    NextCachedValue = 0;
    rng::fill(aCachedMapReads, CVector{});
}

// 0x6FFBB0
float CPointLights::GenerateLightsAffectingObject(const CVector* point, float* totalLighting, CEntity* entity) {
    float antilightMult = 1.0f;
    for (const auto& light : GetActiveLights()) {
        if (light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
            continue;
        }
        const CVector delta = light.m_vecPosn - *point;
        const float   rad   = light.m_fRadius;
        if (-rad >= delta.x || delta.x >= rad || -rad >= delta.y || delta.y >= rad || -rad >= delta.z || delta.z >= rad) {
            continue;
        }
        const float dist = delta.Magnitude();
        if (dist >= rad) {
            continue;
        }

        const float ratio = dist / rad;
        if (light.m_nType == PLTYPE_ANTILIGHT) {
            antilightMult *= ratio;
            continue;
        }

        if (totalLighting) {
            const float f = (1.0f - ratio) * (1.0f / 3.0f);
            *totalLighting += f * light.m_fColorRed;
            *totalLighting += f * light.m_fColorGreen;
            *totalLighting += f * light.m_fColorBlue;
        }

        float intensity = ratio >= 0.5f ? 1.0f - ((ratio - 0.5f) + (ratio - 0.5f)) : 1.0f;
        if (dist == 0.0f) {
            continue;
        }
        const float invDist = 1.0f / dist;
        if (light.m_nType == PLTYPE_DIRECTIONAL && light.m_pEntityToLight != entity) {
            const float dot = -(delta.x * invDist * light.m_vecDirection.x
                              + delta.y * invDist * light.m_vecDirection.y
                              + invDist * delta.z * light.m_vecDirection.z) - 0.5f;
            intensity *= std::max(dot + dot, 0.0f);
        }
        if (intensity > 0.0f) {
            AddAnExtraDirectionalLight(Scene.m_pRpWorld, delta.x * invDist, delta.y * invDist, invDist * delta.z,
                intensity * light.m_fColorRed,
                intensity * light.m_fColorGreen,
                intensity * light.m_fColorBlue);
        }
    }
    return antilightMult;
}

// 0x6FFE70
float CPointLights::GetLightMultiplier(const CVector* point) {
    float antilightMult = 1.0f;
    float lightSum      = 0.0f;
    for (const auto& light : GetActiveLights()) {
        if (light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
            continue;
        }
        const CVector delta = light.m_vecPosn - *point;
        const float   rad   = light.m_fRadius;
        if (-rad >= delta.x || delta.x >= rad || -rad >= delta.y || delta.y >= rad || -rad >= delta.z || delta.z >= rad) {
            continue;
        }
        const float dist = delta.Magnitude();
        if (dist >= rad) {
            continue;
        }
        const float ratio = dist / rad;
        if (light.m_nType == PLTYPE_ANTILIGHT) {
            antilightMult *= ratio;
        } else {
            lightSum += (1.0f - ratio) * light.m_fColorRed   * (1.0f / 3.0f)
                      + (1.0f - ratio) * light.m_fColorGreen * (1.0f / 3.0f)
                      + (1.0f - ratio) * light.m_fColorBlue  * (1.0f / 3.0f);
        }
    }
    return antilightMult + lightSum;
}

// 0x6FFFE0
void CPointLights::RemoveLightsAffectingObject() {
    RemoveExtraDirectionalLights(Scene.m_pRpWorld);
}

// 0x6FFFF0
bool CPointLights::ProcessVerticalLineUsingCache(CVector point, float* outZ) {
    for (auto&& [i, cached] : rngv::enumerate(aCachedMapReads)) {
        if (cached.x == point.x && cached.y == point.y && cached.z == point.z) {
            *outZ = aCachedMapReadResults[i];
            return true;
        }
    }

    CColPoint colPoint;
    CEntity*  entity;
    if (!CWorld::ProcessVerticalLine(point, point.z - 5.0f, colPoint, entity, true, false, false, false, true, false, nullptr)) {
        return false;
    }

    aCachedMapReadResults[NextCachedValue] = colPoint.m_vecPoint.z;
    aCachedMapReads[NextCachedValue]       = point;
    NextCachedValue                        = (NextCachedValue + 1) % MAX_POINT_LIGHTS;

    *outZ = colPoint.m_vecPoint.z;
    return true;
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
