/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PointLights.h"
#include "Clouds.h"

void CPointLights::InjectHooks() {
    RH_ScopedClass(CPointLights);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6FFB40);
    RH_ScopedInstall(GenerateLightsAffectingObject, 0x6FFBB0);
    RH_ScopedInstall(GetLightMultiplier, 0x6FFE70);
    RH_ScopedInstall(RemoveLightsAffectingObject, 0x6FFFE0);
    RH_ScopedInstall(ProcessVerticalLineUsingCache, 0x6FFFF0);
    RH_ScopedInstall(AddLight, 0x7000E0);
    RH_ScopedInstall(RenderFogEffect, 0x7002D0);
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
        const CVector delta  = light.m_vecPosn - *point;
        const float   radius = light.m_fRadius;
        if (!CRect{ CVector2D{ *point }, radius }.IsPointInside(light.m_vecPosn) || std::abs(delta.z) >= radius) {
            continue;
        }
        const float dist = delta.Magnitude();
        if (dist >= radius) {
            continue;
        }

        const float ratio = dist / radius;
        if (light.m_nType == PLTYPE_ANTILIGHT) {
            antilightMult *= ratio;
            continue;
        }

        if (totalLighting) {
            *totalLighting += (1.0f - ratio) * light.m_fColorRed   * (1.0f / 3.0f);
            *totalLighting += (1.0f - ratio) * light.m_fColorGreen * (1.0f / 3.0f);
            *totalLighting += (1.0f - ratio) * light.m_fColorBlue  * (1.0f / 3.0f);
        }

        float intensity = ratio >= 0.5f ? 1.0f - ((ratio - 0.5f) + (ratio - 0.5f)) : 1.0f;
        if (dist == 0.0f) {
            continue;
        }
        const CVector dir = delta * (1.0f / dist);
        if (light.m_nType == PLTYPE_DIRECTIONAL && light.m_pEntityToLight != entity) {
            const float dot = -DotProduct(dir, light.m_vecDirection) - 0.5f;
            intensity *= std::max(dot + dot, 0.0f);
        }
        if (intensity > 0.0f) {
            AddAnExtraDirectionalLight(Scene.m_pRpWorld, dir.x, dir.y, dir.z,
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
    float lightSum     = 0.0f;
    for (const auto& light : GetActiveLights()) {
        if (light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
            continue;
        }
        const CVector delta  = light.m_vecPosn - *point;
        const float   radius = light.m_fRadius;
        if (!CRect{ CVector2D{ *point }, radius }.IsPointInside(light.m_vecPosn) || std::abs(delta.z) >= radius) {
            continue;
        }
        const float dist = delta.Magnitude();
        if (dist >= radius) {
            continue;
        }
        const float ratio = dist / radius;
        if (light.m_nType == PLTYPE_ANTILIGHT) {
            antilightMult *= ratio;
        } else {
            lightSum += (1.0f - ratio)
                      * (light.m_fColorRed + light.m_fColorGreen + light.m_fColorBlue)
                      / 3.0f;
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
        if (cached == point) {
            *outZ = aCachedMapReadResults[i];
            return true;
        }
    }

    CColPoint colPoint;
    CEntity*  entity;
    if (!CWorld::ProcessVerticalLine(point, point.z - 20.0f, colPoint, entity, true, false, false, false, true, false, nullptr)) {
        return false;
    }

    aCachedMapReadResults[NextCachedValue] = colPoint.m_vecPoint.z;
    aCachedMapReads[NextCachedValue]       = point;
    NextCachedValue                       = (NextCachedValue + 1) % MAX_POINT_LIGHTS;

    *outZ = colPoint.m_vecPoint.z;
    return true;
}

// 0x7000E0
void CPointLights::AddLight(uint8 lightType, CVector point, CVector direction, float radius, float red, float green, float blue, uint8 fogType, bool generateExtraShadows, CEntity* entityAffected) {
    const float   maxDist = radius + 15.0f;
    const CVector camPos  = TheCamera.GetPosition();
    if (!CRect{ CVector2D{ point }, maxDist }.IsPointInside(CVector2D{ camPos })) {
        return;
    }
    if (NumLights >= MAX_POINT_LIGHTS) {
        return;
    }
    const float dist = (point - camPos).Magnitude();
    if (dist >= maxDist) {
        return;
    }

    // Fade color out starting at 75% of the max distance
    const float fade = dist < maxDist * 0.75f ? 1.0f : 1.0f - (dist / maxDist - 0.75f) * 4.0f;

    new (&aLights[NumLights++]) CPointLight{
        .m_vecPosn          = point,
        .m_vecDirection     = direction,
        .m_fRadius          = radius,
        .m_fColorRed        = red * fade,
        .m_fColorGreen      = green * fade,
        .m_fColorBlue       = blue * fade,
        .m_pEntityToLight   = entityAffected,
        .m_nType            = static_cast<ePointLightType>(lightType),
        .m_nFogType         = fogType,
        .m_bGenerateShadows = generateExtraShadows,
    };
}

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    // 0x8D5068 - Sprite scale per fog puff pattern index
    static constexpr float FogSizes[8] = { 1.3f, 2.0f, 1.7f, 2.0f, 1.4f, 2.1f, 1.5f, 2.3f };

    if (CCutsceneMgr::ms_running) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCloudTex)));
    CSprite::InitSpriteBuffer();

    const auto RenderFogSprite = [](const CPointLight& light, const CVector& pos, float intensity, float sizeMult, float aspect, float angle) {
        RwV3d screen;
        float w, h;
        if (!CSprite::CalcScreenCoors(pos, &screen, &w, &h, true, true)) {
            return;
        }
        CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect(
            screen.x, screen.y, screen.z,
            w * sizeMult, h * sizeMult * aspect,
            (uint8)(intensity * light.m_fColorRed),
            (uint8)(intensity * light.m_fColorGreen),
            (uint8)(intensity * light.m_fColorBlue),
            (int16)intensity,
            1.0f / screen.z,
            angle,
            255
        );
    };

    for (const auto& light : GetActiveLights()) {
        float fogAmount, fogSize;
        switch (light.m_nFogType) {
        case rwFOGTYPELINEAR:
            fogAmount = CWeather::Foggyness;
            fogSize   = 9.0f;
            break;
        case rwFOGTYPEEXPONENTIAL:
            fogAmount = 0.4f;
            fogSize   = 3.0f;
            break;
        default:
            continue;
        }
        if (fogAmount == 0.0f) {
            continue;
        }

        const auto& pos = light.m_vecPosn;
        const auto& dir = light.m_vecDirection;

        if (light.m_nType == PLTYPE_DIRECTIONAL) {
            // Fog cone along the light direction (12 units long, 5 units wide), sampled on a 4x4 grid
            constexpr float FOG_LENGTH = 12.0f;
            constexpr float FOG_RADIUS = 5.0f;

            const CVector2D end = CVector2D{ pos } + CVector2D{ dir } * FOG_LENGTH;
            const auto [minX, maxX] = std::minmax(pos.x, end.x);
            const auto [minY, maxY] = std::minmax(pos.y, end.y);

            // Truncate before snapping to the grid, including at negative coordinates.
            const int32 startX = static_cast<int32>(minX - FOG_RADIUS) / 4 * 4;
            const int32 startY = static_cast<int32>(minY - FOG_RADIUS) / 4 * 4;
            const int32 endX   = static_cast<int32>(maxX + FOG_RADIUS) + 4;
            const int32 endY   = static_cast<int32>(maxY + FOG_RADIUS) + 4;
            for (int32 x = startX; x <= endX; x += 4) {
                for (int32 y = startY; y <= endY; y += 4) {
                    const auto pattern = ((x >> 2) ^ (y >> 2)) & 0xF;
                    if (pattern % 2 == 0) {
                        continue;
                    }

                    const CVector2D delta2D = CVector2D{ (float)x, (float)y } - CVector2D{ pos };
                    const float     along2D = DotProduct2D(delta2D, dir);
                    if (along2D <= 0.0f || along2D >= FOG_LENGTH || delta2D.SquaredMagnitude() - sq(along2D) >= sq(FOG_RADIUS)) {
                        continue;
                    }

                    CColPoint colPoint;
                    CEntity*  entity;
                    if (!CWorld::ProcessVerticalLine({ (float)x, (float)y, pos.z + 10.0f }, pos.z - 10.0f, colPoint, entity, true, false, false, false, true, false, nullptr)) {
                        continue;
                    }

                    const CVector puffPos{ (float)x, (float)y, colPoint.m_vecPoint.z + 1.3f };
                    const CVector delta = puffPos - pos;
                    const float   along = DotProduct(delta, dir);
                    if (along <= 0.0f || along >= FOG_LENGTH) {
                        continue;
                    }
                    const float distSq = delta.SquaredMagnitude();
                    const float perpSq = distSq - sq(along);
                    if (perpSq >= sq(FOG_RADIUS)) {
                        continue;
                    }

                    const float intensity = along / std::sqrt(distSq) * fogAmount * 50.0f
                                          * (1.0f - sq(along / FOG_LENGTH))
                                          * (1.0f - sq(std::sqrt(perpSq) / FOG_RADIUS));
                    const auto puffIndex = pattern >> 1;
                    // 6.28, and not TWO_PI because the binary uses exactly 6.28
                    RenderFogSprite(light, puffPos, intensity, FogSizes[puffIndex], 1.0f,
                        6.28f * (float)(CTimer::GetTimeInMS() % 8192) / 8192.0f); // angle (0, 2pi)
                }
            }
        } else if (light.m_nType == PLTYPE_POINTLIGHT || light.m_nType == PLTYPE_ONLYFOGEFFECT_ALWAYS || light.m_nType == PLTYPE_ONLYFOGEFFECT) {
            // Fog disc around the light, sampled on a 2x2 grid
            float groundZ;
            if (!ProcessVerticalLineUsingCache(pos, &groundZ)) {
                continue;
            }

            // Truncate before snapping to the grid, including at negative coordinates.
            const int32 startX = static_cast<int32>(pos.x - fogSize) / 2 * 2;
            const int32 startY = static_cast<int32>(pos.y - fogSize) / 2 * 2;
            const int32 endX   = static_cast<int32>(pos.x + fogSize) + 2;
            const int32 endY   = static_cast<int32>(pos.y + fogSize) + 2;
            for (int32 x = startX; x <= endX; x += 2) {
                for (int32 y = startY; y <= endY; y += 2) {
                    // Cheap spatial hashing to get somewhat a random number for fog generation
                    // Imagine evaluating only a single round of some hashing algorithm (e.g. FNV)
                    const auto hash = ((x / 2) ^ (y / 2)) & 0xF;
                    if (!(hash & 1)) {
                        // is_odd(x/2) != is_odd(y/2)
                        // Filtering this case creates a checkerboard pattern, halving the no of render operations
                        continue;
                    }

                    const CVector2D puffPos2D{ (float)x, (float)y };
                    const float     distSq = (puffPos2D - CVector2D{ pos }).SquaredMagnitude();
                    if (distSq >= sq(fogSize)) {
                        continue;
                    }

                    const float camDist = (puffPos2D - CVector2D{ TheCamera.GetPosition() }).Magnitude();
                    if (camDist >= 15.0f) {
                        continue;
                    }
                    const float camFade = camDist < 7.5f ? 1.0f : 1.0f - (camDist - 7.5f) / 7.5f;

                    const float intensity = (1.0f - distSq / sq(fogSize)) * camFade * fogAmount * 37.0f;
                    const auto puffIndex = hash >> 1;

                    constexpr auto PhaseOffsetMs = 2300; // Intentionally non-power of two to appear asymmetric
                    // 6.28, and not TWO_PI because the binary uses exactly 6.28
                    RenderFogSprite(
                        light,
                        { puffPos2D.x, puffPos2D.y, groundZ + 1.6f },
                        intensity,
                        FogSizes[puffIndex],
                        0.7f,
                        6.28f * (float)((CTimer::GetTimeInMS() + puffIndex * PhaseOffsetMs) % 32768) / 32768.0f // angle (0,2pi)
                    );
                }
            }
        }
    }

    CSprite::FlushSpriteBuffer();
}
