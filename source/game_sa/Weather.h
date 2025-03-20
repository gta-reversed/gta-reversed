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
    static float &TrafficLightsBrightness;
    static bool &bScriptsForceRain;
    static float &Earthquake;
    static uint32 &CurrentRainParticleStrength;
    static uint32 &LightningStartY; // only initialized (0), not used
    static uint32 &LightningStartX; // only initialized (0), not used
    static int32 &LightningFlashLastChange;
    static int32 &WhenToPlayLightningSound;
    static uint32 &LightningDuration;
    static uint32 &LightningStart; // frame number
    static bool &LightningFlash;
    static bool &LightningBurst;
    static float &HeadLightsSpectrum;
    static float &WaterFogFXControl;
    static float &HeatHazeFXControl;
    static float &HeatHaze;
    static float &SunGlare;
    static float &Rainbow;
    static float &Wavyness;
    static float &WindClipped;
    static CVector &WindDir;
    static float &Wind;
    static float &Sandstorm;
    static float &Rain;
    static float &InTunnelness;
    static float &WaterDepth;
    static float &UnderWaterness;
    static float &ExtraSunnyness;
    static float &Foggyness_SF;
    static float &Foggyness;
    static float &CloudCoverage;
    static float &WetRoads;
    static float &InterpolationValue;
    static uint32 &WeatherTypeInList;
    static eWeatherRegion &WeatherRegion;
    static eWeatherType &ForcedWeatherType;
    static eWeatherType &NewWeatherType;
    static eWeatherType &OldWeatherType;
    static CAEWeatherAudioEntity &m_WeatherAudioEntity;
    static bool& StreamAfterRainTimer;

    static float(&saTreeWindOffsets)[16];
    static float(&saBannerWindOffsets)[32];

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
