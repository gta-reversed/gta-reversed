#include "StdInc.h"

#include "PlaneTrails.h"

void CPlaneTrails::InjectHooks() {
    RH_ScopedClass(CPlaneTrails);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717370);
    RH_ScopedInstall(Render, 0x7173A0);
    RH_ScopedInstall(RegisterPoint, 0x7174C0);
    RH_ScopedInstall(Update, 0x7174F0);
}

// 0x717370
void CPlaneTrails::Init() {
    for (auto& pt : aArray) {
        pt.Init();
    }
}

// 0x7173A0
void CPlaneTrails::Render() {
    const CColourSet& cc            = CTimeCycle::m_CurrentColours;
    const float maxColorComponent   = (float)std::max({ cc.m_nSkyBottomBlue, cc.m_nSkyBottomGreen, cc.m_nSkyBottomRed }) * (1.0f / 255.0f);
    const float visibilityIntensity = std::min({ 1.0f - CWeather::CloudCoverage, 1.0f - CWeather::Foggyness, 1.0f - CWeather::Rain });
    const float finalIntensity      = std::min(maxColorComponent, visibilityIntensity);

    if (finalIntensity > 0.0001f) {
        for (auto& pt : aArray) {
            pt.Render(finalIntensity);
        }
    }
}

// 0x7174C0
void CPlaneTrails::RegisterPoint(CVector point, uint32 trailIndex) {
    aArray[trailIndex].RegisterPoint(point);
}

// 0x7174F0
void CPlaneTrails::Update() {
    const uint32 coronaBaseId = 100; // TODO: Magic number

    aArray[0].Update(CVector{ 2590.0f, +2200.0f, 550.0f }, CRGBA{ 255,   0,   0, 255 }, coronaBaseId + 1,   0, 22, 7);
    aArray[1].Update(CVector{ 2000.0f, -2600.0f, 500.0f }, CRGBA{ 255, 255, 128, 255 }, coronaBaseId + 2, 350, 21, 7);
    aArray[2].Update(CVector{ 2100.0f, +1300.0f, 600.0f }, CRGBA{ 255, 255, 255, 255 }, coronaBaseId + 3, 200, 20, 6);
}
