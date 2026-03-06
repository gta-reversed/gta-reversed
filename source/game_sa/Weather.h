/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include "AEWeatherAudioEntity.h"
#include "eWeatherType.h"

enum eWeatherRegion : int16 {
    WEATHER_REGION_DEFAULT = 0,
    WEATHER_REGION_LA = 1,
    WEATHER_REGION_SF = 2,
    WEATHER_REGION_LV = 3,
    WEATHER_REGION_DESERT = 4
};

class CWeather {
public:
    static inline auto& TrafficLightsBrightness = StaticRef<float>(0xC812A8);
    static inline auto& bScriptsForceRain = StaticRef<bool>(0xC812AC);
    static inline auto& Earthquake = StaticRef<float>(0xC81340);
    static inline auto& CurrentRainParticleStrength = StaticRef<uint32>(0xC812B0);
    static inline auto& LightningStartY = StaticRef<uint32>(0xC812B4); // only initialized (0), not used
    static inline auto& LightningStartX = StaticRef<uint32>(0xC812B8); // only initialized (0), not used
    static inline auto& LightningFlashLastChange = StaticRef<int32>(0xC812BC);
    static inline auto& WhenToPlayLightningSound = StaticRef<int32>(0xC812C0);
    static inline auto& LightningDuration = StaticRef<uint32>(0xC812C4);
    static inline auto& LightningStart = StaticRef<uint32>(0xC812C8); // frame number
    static inline auto& LightningFlash = StaticRef<bool>(0xC812CC);
    static inline auto& LightningBurst = StaticRef<bool>(0xC812CD);
    static inline auto& HeadLightsSpectrum = StaticRef<float>(0xC812D0);
    static inline auto& WaterFogFXControl = StaticRef<float>(0xC81338);
    static inline auto& HeatHazeFXControl = StaticRef<float>(0xC812D8);
    static inline auto& HeatHaze = StaticRef<float>(0xC812DC);
    static inline auto& SunGlare = StaticRef<float>(0xC812E0);
    static inline auto& Rainbow = StaticRef<float>(0xC812E4);
    static inline auto& Wavyness = StaticRef<float>(0xC812E8);
    static inline auto& WindClipped = StaticRef<float>(0xC812EC);
    static inline auto& WindDir = StaticRef<CVector>(0xC813E0);
    static inline auto& Wind = StaticRef<float>(0xC812F0);
    static inline auto& Sandstorm = StaticRef<float>(0xC812F4);
    static inline auto& Rain = StaticRef<float>(0xC81324);
    static inline auto& InTunnelness = StaticRef<float>(0xC81334);
    static inline auto& WaterDepth = StaticRef<float>(0xC81330);
    static inline auto& UnderWaterness = StaticRef<float>(0xC8132C);
    static inline auto& ExtraSunnyness = StaticRef<float>(0xC812F8);
    static inline auto& Foggyness_SF = StaticRef<float>(0xC812FC);
    static inline auto& Foggyness = StaticRef<float>(0xC81300);
    static inline auto& CloudCoverage = StaticRef<float>(0xC81304);
    static inline auto& WetRoads = StaticRef<float>(0xC81308);
    static inline auto& InterpolationValue = StaticRef<float>(0xC8130C);
    static inline auto& WeatherTypeInList = StaticRef<uint32>(0xC81310);
    static inline auto& WeatherRegion = StaticRef<eWeatherRegion>(0xC81314);
    static inline auto& ForcedWeatherType = StaticRef<eWeatherType>(0xC81318);
    static inline auto& NewWeatherType = StaticRef<eWeatherType>(0xC8131C);
    static inline auto& OldWeatherType = StaticRef<eWeatherType>(0xC81320);
    static inline auto& m_WeatherAudioEntity = StaticRef<CAEWeatherAudioEntity>(0xC81360);
    static inline auto& StreamAfterRainTimer = StaticRef<bool>(0x8D5EAC);

    // in entity.cpp:

    static std::array<float, 16> saTreeWindOffsets; // orig WindTabel
    static std::array<float, 32> saBannerWindOffsets; // orig BannerWindTabel

public:
    static void InjectHooks();

    static void Init();
    static void AddRain();
    static void AddSandStormParticles();
    static const eWeatherType* FindWeatherTypesList();
    static void ForceWeather(eWeatherType weatherType);
    static void ForceWeatherNow(eWeatherType weatherType);
    static bool ForecastWeather(eWeatherType weatherType, int32 numSteps);
    static void ReleaseWeather();
    static void RenderRainStreaks();
    static void SetWeatherToAppropriateTypeNow();
    static void Update();
    static void UpdateInTunnelness();
    /*!
    * @notsa
    * @detail Based on code @ `0x72A640`
    * @return The corresponding weather region at a given 2D position, or `WEATHER_REGION_DEFAULT` if no specific region was found
    */
    static eWeatherRegion FindWeatherRegion(CVector2D pos);
    static void UpdateWeatherRegion(CVector* posn);
    static bool IsRainy();

    static bool IsUnderWater() { return UnderWaterness <= 0.0f; } // NOTSA
};
