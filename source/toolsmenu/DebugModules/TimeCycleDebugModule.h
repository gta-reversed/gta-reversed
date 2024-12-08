#pragma once

#include "DebugModule.h"

class TimeCycleDebugModule final : public DebugModule {
public:
    void RenderWindow() override final;
    void Render3D() override final;
    void RenderMenuEntry() override final;

    NOTSA_IMPLEMENT_DEBUG_MODULE_SERIALIZATION(TimeCycleDebugModule, m_IsOpen, m_ShowBoxes);

private:
    void SyncFromGame();
    void RenderBoxesGUI();

private:
    struct Color3 {
        float r, g, b;
    };

    struct Color4 : Color3 {
        float a;
    };

private:
     int    m_OldWeatherTypeId{};
     int    m_NewWeatherType{};
     int    m_Hours{};
     int    m_Minutes{};
     Color3 m_CurrentRGB1{};
     Color3 m_CurrentRGB2{};
     Color3 m_AmbRGB{};
     Color3 m_AmbObjRGB{};
     Color3 m_SkyTopRGB{};
     Color3 m_SkyBottomRGB{};
     Color3 m_SunCoreRGB{};
     Color3 m_SunCoronaRGB{};
     int32  m_SunSize{};
     Color4 m_BelowHorizonGrey{};
     int32  m_SpriteSize{};
     int32  m_SpriteBrightness{};
     int32  m_ShadowStrength{};
     int32  m_PoleShadowStrength{};
     int32  m_LightShadowStrength{};
     int32  m_FarClip{};
     int32  m_FogStart{};
     int32  m_LightsOnGroundBrightness{};
     Color3 m_LowCloudsRGB{};
     Color3 m_FluffyCloudsBottom{};
     Color4 m_WaterRGBA{};
     Color4 m_PostFx1RGBA{};
     Color4 m_PostFx2RGBA{};
     int32  m_CloudAlpha{};
     int32  m_HighLightMinIntensity{};
     int32  m_WaterFogAlpha{};
     int32  m_DirectionalMult{};
     Color3 m_BrightnessAddedToAmbient{};
     int32  m_TimeId{};
     int32  m_WeatherId{};
     bool   m_ShowBoxes{};
     bool   m_IsOpen{};
};
