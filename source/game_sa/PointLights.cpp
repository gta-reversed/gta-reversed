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
    plugin::Call<0x6FFB40>();
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
    plugin::Call<0x7000E0, uint8, CVector, CVector, float, float, float, float, uint8, bool, CEntity*>(lightType, point, direction, radius, red, green, blue, fogType, generateExtraShadows, entityAffected);
}

// 0x7002D0
void CPointLights::RenderFogEffect() {
    ZoneScoped;

    plugin::Call<0x7002D0>();
}
