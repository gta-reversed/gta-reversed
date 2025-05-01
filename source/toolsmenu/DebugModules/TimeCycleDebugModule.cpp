#include "StdInc.h"

#include "TimeCycleDebugModule.h"

#include "CTeleportDebugModule.h"
#include "Lines.h"
#include "PedPlacement.h"

uint8 ColorU8(float color) {
    return (uint8)(color * 255.0f);
}

float ColorF32(uint8 color) {
    return float(color) / 255.0f;
}

float ColorF32(float color) {
    return color / 255.0f;
}

constexpr const char* WEATHER_NAMES[] = {
    "EXTRASUNNY_LA",     "SUNNY_LA",           "EXTRASUNNY_SMOG_LA", "SUNNY_SMOG_LA",     "CLOUDY_LA",        "SUNNY_SF",         "EXTRASUNNY_SF",
    "CLOUDY_SF",         "RAINY_SF",           "FOGGY_SF",           "SUNNY_VEGAS",       "EXTRASUNNY_VEGAS", "CLOUDY_VEGAS",     "EXTRASUNNY_COUNTRYSIDE",
    "SUNNY_COUNTRYSIDE", "CLOUDY_COUNTRYSIDE", "RAINY_COUNTRYSIDE",  "EXTRASUNNY_DESERT", "SUNNY_DESERT",     "SANDSTORM_DESERT", "UNDERWATER",
    "EXTRACOLOURS_1",    "EXTRACOLOURS_2"
};

void TimeCycleDebugModule::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] {
        ImGui::MenuItem("TimeCyc", nullptr, &m_IsOpen);
    });

    notsa::ui::DoNestedMenuIL({ "Visualization", "TimeCyc" }, [&] {
        ImGui::Checkbox("Show Boxes", &m_ShowBoxes);
    });
}

void TimeCycleDebugModule::RenderWindow() {
    const notsa::ui::ScopedWindow wnd{ "TimeCyc", {500.f, 700.f}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }

    SyncFromGame();

    ImGui::Checkbox("Stop Time", &CClock::gbFreezeTime);
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        CMessages::AddBigMessage("Saved"_gxt, 3000, STYLE_MIDDLE);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        CTimeCycle::Initialise();
    }
    ImGui::Checkbox("Extra Colour On", reinterpret_cast<bool*>(&CTimeCycle::m_bExtraColourOn));
    ImGui::Checkbox("Show boxes", &m_ShowBoxes);
    if (m_ShowBoxes) {
        RenderBoxesGUI();
        ImGui::NewLine();
    }

    ImGui::SliderInt("Selected Hour", &m_TimeId, 0, CTimeCycle::NUM_HOURS); // todo: Calc this from the current time
    if (ImGui::Combo("Current Weather", &m_OldWeatherTypeId, WEATHER_NAMES, std::size(WEATHER_NAMES))) {
        CWeather::OldWeatherType = static_cast<eWeatherType>(m_OldWeatherTypeId);
    }
    if (ImGui::Combo("Next Weather", &m_NewWeatherType, WEATHER_NAMES, std::size(WEATHER_NAMES))) {
        CWeather::NewWeatherType = static_cast<eWeatherType>(m_NewWeatherType);
    }

    if (ImGui::SliderInt("Hours", &m_Hours, 0, 23, "%d")) {
        CClock::ms_nGameClockHours = static_cast<uint8>(m_Hours);
    }
    if (ImGui::SliderInt("Minutes", &m_Minutes, 0, 59, "%d")) {
        CClock::ms_nGameClockMinutes = static_cast<uint8>(m_Minutes);
    }

    ImGui::NewLine();
    if (ImGui::ColorEdit3("Current RGB 1", (float*)&m_CurrentRGB1)) {
        CTimeCycle::m_fCurrentRGB1Red   = m_CurrentRGB1.r * 255.0f;
        CTimeCycle::m_fCurrentRGB1Green = m_CurrentRGB1.g * 255.0f;
        CTimeCycle::m_fCurrentRGB1Blue  = m_CurrentRGB1.b * 255.0f;
    }

    if (ImGui::ColorEdit3("Current RGB 2", (float*)&m_CurrentRGB2)) {
        CTimeCycle::m_fCurrentRGB2Red   = m_CurrentRGB2.r * 255.0f;
        CTimeCycle::m_fCurrentRGB2Green = m_CurrentRGB2.r * 255.0f;
        CTimeCycle::m_fCurrentRGB2Blue  = m_CurrentRGB2.r * 255.0f;
    }

    ImGui::NewLine();
    if (ImGui::ColorEdit3("Ambient", (float*)&m_AmbRGB)) {
        CTimeCycle::m_nAmbientRed[m_TimeId][m_WeatherId]   = ColorU8(m_AmbRGB.r);
        CTimeCycle::m_nAmbientGreen[m_TimeId][m_WeatherId] = ColorU8(m_AmbRGB.g);
        CTimeCycle::m_nAmbientBlue[m_TimeId][m_WeatherId]  = ColorU8(m_AmbRGB.b);
    }

    if (ImGui::ColorEdit3("Ambient Obj", (float*)&m_AmbObjRGB)) {
        CTimeCycle::m_nAmbientRed_Obj[m_TimeId][m_WeatherId]   = ColorU8(m_AmbObjRGB.r);
        CTimeCycle::m_nAmbientGreen_Obj[m_TimeId][m_WeatherId] = ColorU8(m_AmbObjRGB.g);
        CTimeCycle::m_nAmbientBlue_Obj[m_TimeId][m_WeatherId]  = ColorU8(m_AmbObjRGB.b);
    }

    ImGui::NewLine();
    if (ImGui::ColorEdit3("Sky Top", (float*)&m_SkyTopRGB)) {
        CTimeCycle::m_nSkyTopRed[m_TimeId][m_WeatherId]   = ColorU8(m_SkyTopRGB.r);
        CTimeCycle::m_nSkyTopGreen[m_TimeId][m_WeatherId] = ColorU8(m_SkyTopRGB.g);
        CTimeCycle::m_nSkyTopBlue[m_TimeId][m_WeatherId]  = ColorU8(m_SkyTopRGB.b);
    }

    if (ImGui::ColorEdit3("Sky Bottom", (float*)&m_SkyBottomRGB)) {
        CTimeCycle::m_nSkyBottomRed[m_TimeId][m_WeatherId]   = ColorU8(m_SkyBottomRGB.r);
        CTimeCycle::m_nSkyBottomGreen[m_TimeId][m_WeatherId] = ColorU8(m_SkyBottomRGB.g);
        CTimeCycle::m_nSkyBottomBlue[m_TimeId][m_WeatherId]  = ColorU8(m_SkyBottomRGB.b);
    }

    ImGui::NewLine();

    auto& vec = CTimeCycle::m_VectorToSun[0];
    ImGui::Text("Vector To Sun XYZ: %.3f %.3f %.3f", vec.x, vec.y, vec.z);

    if (ImGui::ColorEdit3("Sun Core", (float*)&m_SunCoreRGB)) {
        CTimeCycle::m_nSunCoreRed[m_TimeId][m_WeatherId]   = ColorU8(m_SunCoreRGB.r);
        CTimeCycle::m_nSunCoreGreen[m_TimeId][m_WeatherId] = ColorU8(m_SunCoreRGB.g);
        CTimeCycle::m_nSunCoreBlue[m_TimeId][m_WeatherId]  = ColorU8(m_SunCoreRGB.b);
    }

    if (ImGui::ColorEdit3("Sun Corona", (float*)&m_SunCoronaRGB)) {
        CTimeCycle::m_nSunCoronaRed[m_TimeId][m_WeatherId]   = ColorU8(m_SunCoronaRGB.r);
        CTimeCycle::m_nSunCoronaGreen[m_TimeId][m_WeatherId] = ColorU8(m_SunCoronaRGB.g);
        CTimeCycle::m_nSunCoronaBlue[m_TimeId][m_WeatherId]  = ColorU8(m_SunCoronaRGB.b);
    }

    if (ImGui::SliderInt("Sun Size", &m_SunSize, 0, 255)) {
        CTimeCycle::m_fSunSize[m_TimeId][m_WeatherId] = m_SunSize;
    }

    ImGui::SliderFloat("Light Direction To Sun X", &CTimeCycle::m_vecDirnLightToSun.x, 0, 255);
    ImGui::SliderFloat("Light Direction To Sun Y", &CTimeCycle::m_vecDirnLightToSun.z, 0, 255);
    ImGui::SliderFloat("Light Direction To Sun Z", &CTimeCycle::m_vecDirnLightToSun.y, 0, 255);

    if (ImGui::ColorEdit4("Below Horizon Grey", (float*)&m_BelowHorizonGrey)) {
        CTimeCycle::m_BelowHorizonGrey.red   = ColorU8(m_BelowHorizonGrey.r);
        CTimeCycle::m_BelowHorizonGrey.green = ColorU8(m_BelowHorizonGrey.g);
        CTimeCycle::m_BelowHorizonGrey.blue  = ColorU8(m_BelowHorizonGrey.b);
        CTimeCycle::m_BelowHorizonGrey.alpha = ColorU8(m_BelowHorizonGrey.a);
    }

    ImGui::NewLine();
    if (ImGui::SliderInt("Sprite Size", &m_SpriteSize, 0, 255)) {
        CTimeCycle::m_fSpriteSize[m_TimeId][m_WeatherId] = m_SpriteSize;
    }

    if (ImGui::SliderInt("Sprite Brightness", &m_SpriteBrightness, 1, 10)) {
        CTimeCycle::m_fSpriteBrightness[m_TimeId][m_WeatherId] = m_SpriteBrightness;
    }

    if (ImGui::SliderInt("Shadows Strength", &m_ShadowStrength, 0, 255)) {
        CTimeCycle::m_nShadowStrength[m_TimeId][m_WeatherId] = m_ShadowStrength;
    }

    if (ImGui::SliderInt("Light Shadows Strength", &m_LightShadowStrength, 0, 255)) {
        CTimeCycle::m_nLightShadowStrength[m_TimeId][m_WeatherId] = m_LightShadowStrength;
    }

    if (ImGui::SliderInt("Pole Shadows Strength", &m_PoleShadowStrength, 0, 255)) {
        CTimeCycle::m_nPoleShadowStrength[m_TimeId][m_WeatherId] = m_PoleShadowStrength;
    }

    if (ImGui::SliderInt("Light On Ground Brightness", &m_LightsOnGroundBrightness, 0, 255)) {
        CTimeCycle::m_fLightsOnGroundBrightness[m_TimeId][m_WeatherId] = m_LightsOnGroundBrightness;
    }

    if (ImGui::SliderInt("Far Clip", &m_FarClip, INT16_MIN, INT16_MAX)) {
        CTimeCycle::m_fFarClip[m_TimeId][m_WeatherId] = static_cast<uint16>(m_FarClip);
    }

    if (ImGui::SliderInt("Fog Start", &m_FogStart, INT16_MIN, INT16_MAX)) {
        CTimeCycle::m_fFogStart[m_TimeId][m_WeatherId] = static_cast<uint16>(m_FogStart);
    }

    ImGui::NewLine();
    if (ImGui::ColorEdit3("Low Clouds", (float*)&m_LowCloudsRGB)) {
        CTimeCycle::m_nLowCloudsRed[m_TimeId][m_WeatherId]   = ColorU8(m_LowCloudsRGB.r);
        CTimeCycle::m_nLowCloudsGreen[m_TimeId][m_WeatherId] = ColorU8(m_LowCloudsRGB.g);
        CTimeCycle::m_nLowCloudsBlue[m_TimeId][m_WeatherId]  = ColorU8(m_LowCloudsRGB.b);
    }

    if (ImGui::ColorEdit3("Fluffy Clouds", (float*)&m_FluffyCloudsBottom)) {
        CTimeCycle::m_nFluffyCloudsBottomRed[m_TimeId][m_WeatherId]   = ColorU8(m_FluffyCloudsBottom.r);
        CTimeCycle::m_nFluffyCloudsBottomGreen[m_TimeId][m_WeatherId] = ColorU8(m_FluffyCloudsBottom.g);
        CTimeCycle::m_nFluffyCloudsBottomBlue[m_TimeId][m_WeatherId]  = ColorU8(m_FluffyCloudsBottom.b);
    }

    if (ImGui::ColorEdit4("Water", (float*)&m_WaterRGBA)) {
        CTimeCycle::m_fWaterRed[m_TimeId][m_WeatherId]   = ColorU8(m_WaterRGBA.r);
        CTimeCycle::m_fWaterGreen[m_TimeId][m_WeatherId] = ColorU8(m_WaterRGBA.g);
        CTimeCycle::m_fWaterBlue[m_TimeId][m_WeatherId]  = ColorU8(m_WaterRGBA.b);
        CTimeCycle::m_fWaterAlpha[m_TimeId][m_WeatherId] = ColorU8(m_WaterRGBA.a);
    }

    if (ImGui::ColorEdit4("Post Fx 1", (float*)&m_PostFx1RGBA)) {
        CTimeCycle::m_fPostFx1Red[m_TimeId][m_WeatherId]   = ColorU8(m_PostFx1RGBA.r);
        CTimeCycle::m_fPostFx1Green[m_TimeId][m_WeatherId] = ColorU8(m_PostFx1RGBA.g);
        CTimeCycle::m_fPostFx1Blue[m_TimeId][m_WeatherId]  = ColorU8(m_PostFx1RGBA.b);
        CTimeCycle::m_fPostFx1Alpha[m_TimeId][m_WeatherId] = ColorU8(m_PostFx1RGBA.a);
    }

    if (ImGui::ColorEdit4("Post Fx 2", (float*)&m_PostFx2RGBA)) {
        CTimeCycle::m_fPostFx2Red[m_TimeId][m_WeatherId]   = ColorU8(m_PostFx2RGBA.r);
        CTimeCycle::m_fPostFx2Green[m_TimeId][m_WeatherId] = ColorU8(m_PostFx2RGBA.g);
        CTimeCycle::m_fPostFx2Blue[m_TimeId][m_WeatherId]  = ColorU8(m_PostFx2RGBA.b);
        CTimeCycle::m_fPostFx2Alpha[m_TimeId][m_WeatherId] = ColorU8(m_PostFx2RGBA.a);
    }

    if (ImGui::ColorEdit3("Brightness Added To Ambient", (float*)&m_BrightnessAddedToAmbient)) {
        CTimeCycle::m_BrightnessAddedToAmbientRed   = m_BrightnessAddedToAmbient.r;
        CTimeCycle::m_BrightnessAddedToAmbientGreen = m_BrightnessAddedToAmbient.g;
        CTimeCycle::m_BrightnessAddedToAmbientBlue  = m_BrightnessAddedToAmbient.b;
    }

    if (ImGui::SliderInt("Cloud Alpha", &m_CloudAlpha, 0, 255)) {
        CTimeCycle::m_fCloudAlpha[m_TimeId][m_WeatherId] = m_CloudAlpha;
    }

    if (ImGui::SliderInt("High Light Min Intensity", &m_HighLightMinIntensity, 0, 255)) {
        CTimeCycle::m_nHighLightMinIntensity[m_TimeId][m_WeatherId] = m_HighLightMinIntensity;
    }

    if (ImGui::SliderInt("Water Fog Alpha", &m_WaterFogAlpha, 0, 255)) {
        CTimeCycle::m_nWaterFogAlpha[m_TimeId][m_WeatherId] = m_WaterFogAlpha;
    }

    if (ImGui::SliderInt("Directional Mult", &m_DirectionalMult, 0, 255)) {
        CTimeCycle::m_nDirectionalMult[m_TimeId][m_WeatherId] = m_DirectionalMult;
    }

    ImGui::NewLine();
    ImGui::SliderInt("Current Stored Value", &CTimeCycle::m_CurrentStoredValue, 0, 16);
    ImGui::SliderFloat("Shadow Front X", &CTimeCycle::m_fShadowFrontX[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Front Y", &CTimeCycle::m_fShadowFrontY[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Side X", &CTimeCycle::m_fShadowSideX[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Side Y", &CTimeCycle::m_fShadowSideY[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Displacement X", &CTimeCycle::m_fShadowDisplacementX[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shadow Displacement Y", &CTimeCycle::m_fShadowDisplacementY[0], 0.0f, 1.0f);

    ImGui::NewLine();
    ImGui::SliderInt("Fog Reduction", &CTimeCycle::m_FogReduction, 0, 64);
    ImGui::SliderInt("Extra Colour", &CTimeCycle::m_ExtraColour, 0, 255);
    ImGui::SliderInt("Extra Colour Weather Type", &CTimeCycle::m_ExtraColourWeatherType, 0, 255);
    ImGui::SliderFloat("Extra Colour Inter", &CTimeCycle::m_ExtraColourInter, 0.0f, 1.0f);
}

void TimeCycleDebugModule::RenderBoxesGUI() {
    if (!ImGui::BeginTable("Boxes", 7)) {
        return;
    }

    ImGui::TableSetupColumn("Posistion");
    ImGui::TableSetupColumn("Far Clip");
    ImGui::TableSetupColumn("Lod Dis tMult");
    ImGui::TableSetupColumn("Extra Color");
    ImGui::TableSetupColumn("Strength");
    ImGui::TableSetupColumn("Fall Off");
    ImGui::TableSetupColumn("Teleport");
    ImGui::TableHeadersRow();

    for (auto& box : CTimeCycle::GetBoxes()) {
        ImGui::PushID(&box);

        const auto& boxCenter = box.m_Box.GetCenter();

        ImGui::TableNextColumn();
        ImGui::Text("%s", CTheZones::FindSmallestZoneForPosition(boxCenter, false)->GetTranslatedName());

        ImGui::TableNextColumn();
        ImGui::Text("%d", box.m_FarClip);

        ImGui::TableNextColumn();
        ImGui::Text("%d", box.m_LodDistMult);

        ImGui::TableNextColumn();
        ImGui::Text("%d", box.m_ExtraColor);

        ImGui::TableNextColumn();
        ImGui::Text("%f", box.m_Strength);

        ImGui::TableNextColumn();
        ImGui::Text("%f", box.m_Falloff);

        ImGui::TableNextColumn();
        if (bool selected = false; ImGui::Selectable("Teleport", &selected, ImGuiSelectableFlags_SpanAllColumns)) {
            TeleportDebugModule::TeleportTo(boxCenter);
            (void)CPedPlacement::FindZCoorForPed(FindPlayerPed()->GetPosition()); // Sets ped at the correct Z
        }

        ImGui::PopID();
    }

    ImGui::EndTable();
}

void TimeCycleDebugModule::Render3D() {
    if (!m_ShowBoxes) {
        return;
    }

    for (auto& box : CTimeCycle::GetBoxes()) {
        box.m_Box.DrawWireFrame({ 255, 0, 0, 255 }, CMatrix::Unity());
    }
}

void TimeCycleDebugModule::SyncFromGame() {
    m_WeatherId                  = CWeather::OldWeatherType;

    m_OldWeatherTypeId           = CWeather::OldWeatherType;
    m_NewWeatherType             = CWeather::NewWeatherType;
    m_Hours                      = CClock::ms_nGameClockHours;
    m_Minutes                    = CClock::ms_nGameClockMinutes;

    m_CurrentRGB1.r              = ColorF32(CTimeCycle::m_fCurrentRGB1Red);
    m_CurrentRGB1.g              = ColorF32(CTimeCycle::m_fCurrentRGB1Green);
    m_CurrentRGB1.b              = ColorF32(CTimeCycle::m_fCurrentRGB1Blue);

    m_CurrentRGB2.r              = ColorF32(CTimeCycle::m_fCurrentRGB2Red);
    m_CurrentRGB2.g              = ColorF32(CTimeCycle::m_fCurrentRGB2Green);
    m_CurrentRGB2.b              = ColorF32(CTimeCycle::m_fCurrentRGB2Blue);

    m_AmbRGB.r                   = ColorF32(CTimeCycle::m_nAmbientRed[m_TimeId][m_WeatherId]);
    m_AmbRGB.g                   = ColorF32(CTimeCycle::m_nAmbientGreen[m_TimeId][m_WeatherId]);
    m_AmbRGB.b                   = ColorF32(CTimeCycle::m_nAmbientBlue[m_TimeId][m_WeatherId]);

    m_AmbObjRGB.r                = ColorF32(CTimeCycle::m_nAmbientRed_Obj[m_TimeId][m_WeatherId]);
    m_AmbObjRGB.g                = ColorF32(CTimeCycle::m_nAmbientGreen_Obj[m_TimeId][m_WeatherId]);
    m_AmbObjRGB.b                = ColorF32(CTimeCycle::m_nAmbientBlue_Obj[m_TimeId][m_WeatherId]);

    m_SkyTopRGB.r                = ColorF32(CTimeCycle::m_nSkyTopRed[m_TimeId][m_WeatherId]);
    m_SkyTopRGB.g                = ColorF32(CTimeCycle::m_nSkyTopGreen[m_TimeId][m_WeatherId]);
    m_SkyTopRGB.b                = ColorF32(CTimeCycle::m_nSkyTopBlue[m_TimeId][m_WeatherId]);

    m_SkyBottomRGB.r             = ColorF32(CTimeCycle::m_nSkyBottomRed[m_TimeId][m_WeatherId]);
    m_SkyBottomRGB.g             = ColorF32(CTimeCycle::m_nSkyBottomGreen[m_TimeId][m_WeatherId]);
    m_SkyBottomRGB.b             = ColorF32(CTimeCycle::m_nSkyBottomBlue[m_TimeId][m_WeatherId]);

    m_SunCoreRGB.r               = ColorF32(CTimeCycle::m_nSunCoreRed[m_TimeId][m_WeatherId]);
    m_SunCoreRGB.g               = ColorF32(CTimeCycle::m_nSunCoreGreen[m_TimeId][m_WeatherId]);
    m_SunCoreRGB.b               = ColorF32(CTimeCycle::m_nSunCoreBlue[m_TimeId][m_WeatherId]);

    m_SunCoronaRGB.r             = ColorF32(CTimeCycle::m_nSunCoronaRed[m_TimeId][m_WeatherId]);
    m_SunCoronaRGB.g             = ColorF32(CTimeCycle::m_nSunCoronaGreen[m_TimeId][m_WeatherId]);
    m_SunCoronaRGB.b             = ColorF32(CTimeCycle::m_nSunCoronaBlue[m_TimeId][m_WeatherId]);

    m_SunSize                    = CTimeCycle::m_fSunSize[m_TimeId][m_WeatherId];

    m_BelowHorizonGrey.r         = ColorF32(CTimeCycle::m_BelowHorizonGrey.red);
    m_BelowHorizonGrey.g         = ColorF32(CTimeCycle::m_BelowHorizonGrey.green);
    m_BelowHorizonGrey.b         = ColorF32(CTimeCycle::m_BelowHorizonGrey.blue);
    m_BelowHorizonGrey.a         = ColorF32(CTimeCycle::m_BelowHorizonGrey.alpha);

    m_SpriteSize                 = CTimeCycle::m_fSpriteSize[m_TimeId][m_WeatherId];
    m_SpriteBrightness           = CTimeCycle::m_fSpriteBrightness[m_TimeId][m_WeatherId];
    m_ShadowStrength             = CTimeCycle::m_nShadowStrength[m_TimeId][m_WeatherId];
    m_LightShadowStrength        = CTimeCycle::m_nLightShadowStrength[m_TimeId][m_WeatherId];
    m_PoleShadowStrength         = CTimeCycle::m_nPoleShadowStrength[m_TimeId][m_WeatherId];
    m_FarClip                    = CTimeCycle::m_fFarClip[m_TimeId][m_WeatherId];
    m_FogStart                   = CTimeCycle::m_fFogStart[m_TimeId][m_WeatherId];
    m_LightsOnGroundBrightness   = CTimeCycle::m_fLightsOnGroundBrightness[m_TimeId][m_WeatherId];

    m_LowCloudsRGB.r             = ColorF32(CTimeCycle::m_nLowCloudsRed[m_TimeId][m_WeatherId]);
    m_LowCloudsRGB.g             = ColorF32(CTimeCycle::m_nLowCloudsGreen[m_TimeId][m_WeatherId]);
    m_LowCloudsRGB.b             = ColorF32(CTimeCycle::m_nLowCloudsBlue[m_TimeId][m_WeatherId]);

    m_FluffyCloudsBottom.r       = ColorF32(CTimeCycle::m_nFluffyCloudsBottomRed[m_TimeId][m_WeatherId]);
    m_FluffyCloudsBottom.g       = ColorF32(CTimeCycle::m_nFluffyCloudsBottomGreen[m_TimeId][m_WeatherId]);
    m_FluffyCloudsBottom.b       = ColorF32(CTimeCycle::m_nFluffyCloudsBottomBlue[m_TimeId][m_WeatherId]);

    m_WaterRGBA.r                = ColorF32(CTimeCycle::m_fWaterRed[m_TimeId][m_WeatherId]);
    m_WaterRGBA.g                = ColorF32(CTimeCycle::m_fWaterGreen[m_TimeId][m_WeatherId]);
    m_WaterRGBA.b                = ColorF32(CTimeCycle::m_fWaterBlue[m_TimeId][m_WeatherId]);
    m_WaterRGBA.a                = ColorF32(CTimeCycle::m_fWaterAlpha[m_TimeId][m_WeatherId]);

    m_PostFx1RGBA.r              = ColorF32(CTimeCycle::m_fPostFx1Red[m_TimeId][m_WeatherId]);
    m_PostFx1RGBA.g              = ColorF32(CTimeCycle::m_fPostFx1Green[m_TimeId][m_WeatherId]);
    m_PostFx1RGBA.b              = ColorF32(CTimeCycle::m_fPostFx1Blue[m_TimeId][m_WeatherId]);
    m_PostFx1RGBA.a              = ColorF32(CTimeCycle::m_fPostFx1Alpha[m_TimeId][m_WeatherId]);

    m_PostFx2RGBA.r              = ColorF32(CTimeCycle::m_fPostFx2Red[m_TimeId][m_WeatherId]);
    m_PostFx2RGBA.g              = ColorF32(CTimeCycle::m_fPostFx2Green[m_TimeId][m_WeatherId]);
    m_PostFx2RGBA.b              = ColorF32(CTimeCycle::m_fPostFx2Blue[m_TimeId][m_WeatherId]);
    m_PostFx2RGBA.a              = ColorF32(CTimeCycle::m_fPostFx2Alpha[m_TimeId][m_WeatherId]);

    m_BrightnessAddedToAmbient.r = CTimeCycle::m_BrightnessAddedToAmbientRed;
    m_BrightnessAddedToAmbient.g = CTimeCycle::m_BrightnessAddedToAmbientGreen;
    m_BrightnessAddedToAmbient.b = CTimeCycle::m_BrightnessAddedToAmbientBlue;

    m_CloudAlpha                 = CTimeCycle::m_fCloudAlpha[m_TimeId][m_WeatherId];
    m_HighLightMinIntensity      = CTimeCycle::m_nHighLightMinIntensity[m_TimeId][m_WeatherId];
    m_WaterFogAlpha              = CTimeCycle::m_nWaterFogAlpha[m_TimeId][m_WeatherId];
    m_DirectionalMult            = CTimeCycle::m_nDirectionalMult[m_TimeId][m_WeatherId];
}
