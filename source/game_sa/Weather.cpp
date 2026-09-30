/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include <numbers>

#include "eWeatherType.h"
#include "PostEffects.h"
#include "game_sa/Data/Weather.def"

// 0x8CCF30
std::array<float, 16> CWeather::saTreeWindOffsets = { 1.0f, 0.5f, 0.2f, 0.7f, 0.4f, 1.0f, 0.5f, 0.3f, 0.2f, 0.1f, 0.7f, 0.6f, 0.3f, 1.0f, 0.5f, 0.2f };

/// 0x8CCF70
std::array<float, 32> CWeather::saBannerWindOffsets = { 0.0f, 0.3f, 0.6f, 0.85f, 0.99f, 0.97f, 0.65f, 0.15f, -0.1f, 0.0f, 0.35f, 0.57f, 0.55f, 0.35f, 0.45f, 0.67f, 0.73f, 0.45f, 0.25f, 0.35f, 0.35f, 0.11f, 0.13f, 0.21f, 0.28f, 0.28f, 0.22f, 0.1f, 0.0f, -0.1f, -0.17f, -0.12f };

constexpr std::array<float, NUM_WEATHERS> WindForWeatherType = { // 0x8D5E50
    0.0f, 0.25f, 0.0f, 0.2f, 0.7f, 0.25f, 0.0f, 0.7f, 1.0f, 0.0f, 0.2f, 0.0f, 0.4f, 0.0f, 0.3f, 0.7f, 1.0f, 0.0f, 0.3f, 1.5f, 0.0f, 0.0f, 0.0f
};

constexpr std::array<float, 16> WindDirScales = { // 0x8D5FF8
    1.0f, 0.5f, 1.0f, 0.2f, 0.4f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.7f, 1.0f, 1.0f
};

constexpr std::array<float, 16> WindDirOffsets = { // 0x8D6038
    0.5f, -0.3f, 0.8f, 0.0f, -0.4f, -0.8f, 0.3f, -0.1f, -0.9f, -0.5f, 0.7f, 0.7f, 0.3f, 0.7f, 0.0f, -0.5f
};

void CWeather::InjectHooks() {
    RH_ScopedClass(CWeather);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72A480);
    RH_ScopedInstall(AddRain, 0x72A9A0, { .reversed = false });
    RH_ScopedInstall(AddSandStormParticles, 0x72A820);
    RH_ScopedInstall(FindWeatherTypesList, 0x72A520);
    RH_ScopedInstall(ForceWeather, 0x72A4E0);
    RH_ScopedInstall(ForceWeatherNow, 0x72A4F0);
    RH_ScopedInstall(ForecastWeather, 0x72A590);
    RH_ScopedInstall(ReleaseWeather, 0x72A510);
    RH_ScopedInstall(RenderRainStreaks, 0x72AF70);
    RH_ScopedInstall(SetWeatherToAppropriateTypeNow, 0x72A790);
    RH_ScopedInstall(Update, 0x72B850);
    RH_ScopedInstall(UpdateInTunnelness, 0x72B630);
    //RH_ScopedInstall(UpdateWeatherRegion, 0x72A640, true, { .reversed = false }); // bad
    RH_ScopedInstall(IsRainy, 0x4ABF50);
}

// 0x72A480
void CWeather::Init() {
    ZoneScoped;

    NewWeatherType = WEATHER_EXTRASUNNY_LA;
    OldWeatherType = WEATHER_EXTRASUNNY_LA;
    WeatherRegion  = WEATHER_REGION_DEFAULT;

    InterpolationValue = 0.0f;
    WeatherTypeInList = 0;
    ForcedWeatherType = WEATHER_UNDEFINED;
    WhenToPlayLightningSound = 0;
    bScriptsForceRain = false;
    Rain = 0.0f;
    Sandstorm = 0.0f;
    CurrentRainParticleStrength = 0;
    InTunnelness = 0.0f;
    LightningStartX = 0;
    LightningStartY = 0;
    StreamAfterRainTimer = 0;
}

// 0x72A9A0
void CWeather::AddRain() {
    plugin::Call<0x72A9A0>();
}

// 0x72A820
void CWeather::AddSandStormParticles() {
    CVector position = TheCamera.GetPosition();
    position.x += TheCamera.m_mCameraMatrix.GetForward().x * 10.0f;
    position.y += TheCamera.m_mCameraMatrix.GetForward().y * 10.0f;

    position.x += CGeneral::GetRandomNumberInRange(0.0f, 40.0f) - 20.0f;
    position.y += CGeneral::GetRandomNumberInRange(0.0f, 40.0f) - 20.0f;
    position.z += CGeneral::GetRandomNumberInRange(0.0f, 7.00f) - 2.00f;

    g_fx.m_Sand2->AddParticle(position, CWeather::WindDir * 25.0f, 0.0f, FxPrtMult_c(0.67f, 0.65f, 0.55f, 0.25f, 1.0f, 0.0f, 0.2f));
}

// 0x72A520
const eWeatherType* CWeather::FindWeatherTypesList() {
    switch (WeatherRegion) {
    case WEATHER_REGION_LA:     return WeatherTypesListLA;
    case WEATHER_REGION_SF:     return WeatherTypesListSF;
    case WEATHER_REGION_LV:     return WeatherTypesListVegas;
    case WEATHER_REGION_DESERT: return WeatherTypesListDesert;
    default:                    return WeatherTypesListDefault;
    }
}

// 0x72A4E0
void CWeather::ForceWeather(eWeatherType weatherType) {
    ForcedWeatherType = weatherType;
}

// 0x72A4F0
void CWeather::ForceWeatherNow(eWeatherType weatherType) {
    ForcedWeatherType = weatherType;
    OldWeatherType = weatherType;
    NewWeatherType = weatherType;
}

// 0x72A590
bool CWeather::ForecastWeather(eWeatherType weatherType, int32 numSteps) {
    for (auto step = 0; step <= numSteps; step++) {
        if (FindWeatherTypesList()[(WeatherTypeInList + step) % 64] == weatherType) {
            return true;
        }
    }
    return false;
}

// 0x72A510
void CWeather::ReleaseWeather() {
    ForcedWeatherType = WEATHER_UNDEFINED;
}

// 0x72AF70
void CWeather::RenderRainStreaks() {
    if (CTimer::GetIsCodePaused())
        return;

    {
        const auto strength = (uint32)((64.0f - (float)CTimeCycle::m_FogReduction) * (Rain * 110.0f) / 64.0f);
        if (CurrentRainParticleStrength < strength) {
            if (CurrentRainParticleStrength + 1 <= strength) {
                CurrentRainParticleStrength++;
            }
        } else {
            if (CurrentRainParticleStrength > 0) {
                CurrentRainParticleStrength--;
            }
        }
    }

    if (!CurrentRainParticleStrength)
        return;

    if (CCullZones::CamNoRain() || CCullZones::PlayerNoRain())
        return;

    if (UnderWaterness > 0.0f)
        return;

    if (CGame::currArea)
        return;

    const CVector camPos = TheCamera.GetPosition();
    if (camPos.z > 900.0f)
        return;

    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;

    // (Pirulax) TODO... (refactor)
    constexpr auto RAIN_STREAK_COUNT{ 32u };

    // These are arrays of size `RAIN_STREAK_COUNT`
    static auto& streakPosX = StaticRef<int32*>(0xC81420);
    static auto& streakPosY = StaticRef<int32*>(0xC8141C);
    static auto& streakPosZ = StaticRef<int32*>(0xC81418);
    static auto& streakStrength = StaticRef<uint8*>(0xC81414);

    if (!streakPosX) {
        // This stuff isn't even freed anywhere..
        streakPosX     = new int32[RAIN_STREAK_COUNT];
        streakPosY     = new int32[RAIN_STREAK_COUNT];
        streakPosZ     = new int32[RAIN_STREAK_COUNT];
        streakStrength = new uint8[RAIN_STREAK_COUNT];

        for (unsigned i = 0; i < RAIN_STREAK_COUNT; i++) {
            streakPosX[i] = 0;
            streakPosY[i] = 0;
            streakPosZ[i] = 0;
            streakStrength[i] = (uint8)((float)CurrentRainParticleStrength * 0.6f);
        }
    }

    const auto GetStreakPosition = [&](unsigned i) {
        return CVector{ (float)(streakPosX[i]), (float)(streakPosY[i]), (float)(streakPosZ[i]) };
    };

    const auto UpdateStreak = [&](unsigned i) {
        const CVector posn = GetStreakPosition(i);
        if (!streakStrength[i] || posn.z <= 0.0f || (camPos - posn).Magnitude() > 8.0f) {
            const CVector newPosn = CVector::Random(0.0f, 5.0f) + TheCamera.GetForward() * 6.0f + camPos - CVector{2.5f, 2.5f, 2.5f};
            streakPosX[i] = (int32)(newPosn.x);
            streakPosY[i] = (int32)(newPosn.y);
            streakPosZ[i] = (int32)(newPosn.z);

            streakStrength[i] = (uint8)((float)CurrentRainParticleStrength * 0.6f);
        }
    };

    const auto GetRealVertexIndex = [](unsigned i) {
        return uiTempBufferVerticesStored + i;
    };

    for (unsigned s = 0; s < RAIN_STREAK_COUNT; s++) {
        UpdateStreak(s);

        CVector offsets[2]{};
        offsets[0] = CVector{
            CGeneral::GetRandomNumberInRange(-0.2f, 0.2f),
            CGeneral::GetRandomNumberInRange(-0.2f, 0.2f),
            CGeneral::GetRandomNumberInRange(-0.1f, 0.1f),
        };

        const float posMul = (s % 2) ? Wind * 0.1f : Wind * Rain * 0.1f;
        offsets[1] = offsets[0] - WindDir * posMul + CVector{ 0.0f, 0.0f, CGeneral::GetRandomNumberInRange(0.1f, 0.5f) };

        const uint8 alphas[]{ streakStrength[s], static_cast<uint8>(streakStrength[s] / 2u) };
        for (auto v = 0u; v < std::size(alphas); v++) {
            RxObjSpace3DVertex* vertex = &TempBufferVertices.m_3d[GetRealVertexIndex(v)];

            const RwRGBA color{ 210, 210, 230, alphas[v] };
            // const RwRGBA color{ 255, 0, 0, 255 }; // For debug (makes it more visible)
            RxObjSpace3DVertexSetPreLitColor(vertex, &color);

            const CVector vertPosn = GetStreakPosition(s) + offsets[v];
            RxObjSpace3DVertexSetPos(vertex, &vertPosn);

            aTempBufferIndices[uiTempBufferIndicesStored + v] = GetRealVertexIndex(v);
        }

        streakPosZ[s] -= (int32)CGeneral::GetRandomNumberInRange(0.01f, 0.1f);
        streakStrength[s] = (uint8)std::max(0, (int32)streakStrength[s] - CGeneral::GetRandomNumberInRange(2, 5));

        uiTempBufferVerticesStored += 2;
        uiTempBufferIndicesStored += 2;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGTYPE,           RWRSTATE(rwFOGTYPELINEAR));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aTempBufferIndices, uiTempBufferIndicesStored);
        RwIm3DEnd();
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
}

// 0x72A790
void CWeather::SetWeatherToAppropriateTypeNow() {
    CVector playerCoors = FindPlayerCoors();
    UpdateWeatherRegion(&playerCoors);

    auto weatherType = FindWeatherTypesList()[0];
    ForcedWeatherType = WEATHER_UNDEFINED;
    OldWeatherType = weatherType;
    NewWeatherType = weatherType;
}

// 0x72B850
void CWeather::Update() {
    ZoneScoped;

    const auto IsSunny = [](eWeatherType wt) {
        return notsa::contains({ WEATHER_SUNNY_LA, WEATHER_SUNNY_SMOG_LA, WEATHER_SUNNY_COUNTRYSIDE, WEATHER_SUNNY_SF, WEATHER_SUNNY_VEGAS, WEATHER_SUNNY_DESERT }, wt);
    };
    const auto IsCloudy = [](eWeatherType wt) {
        return notsa::contains({ WEATHER_CLOUDY_LA, WEATHER_CLOUDY_COUNTRYSIDE, WEATHER_CLOUDY_VEGAS, WEATHER_CLOUDY_SF }, wt);
    };
    const auto IsRainy = [](eWeatherType wt) {
        return wt == WEATHER_RAINY_COUNTRYSIDE || wt == WEATHER_RAINY_SF;
    };
    const auto IsFoggy = [](eWeatherType wt) {
        return wt == WEATHER_FOGGY_SF || wt == WEATHER_SANDSTORM_DESERT;
    };
    const auto IsHeatHazy = [](eWeatherType wt) {
        return notsa::contains({ WEATHER_SUNNY_DESERT, WEATHER_EXTRASUNNY_DESERT, WEATHER_EXTRASUNNY_VEGAS, WEATHER_EXTRASUNNY_LA }, wt);
    };

    if (CTimer::GetFrameCounter() % 16 == 0) {
        UpdateWeatherRegion(nullptr);
    }

    // 0x72B866
    if (CReplay::Mode != MODE_PLAYBACK) {
        const auto interpolation = ((float)(uint8)CClock::ms_nGameClockSeconds * (1.0f / 60.0f) + (float)CClock::ms_nGameClockMinutes) * (1.0f / 60.0f);
        if (interpolation < InterpolationValue) {
            UpdateWeatherRegion(nullptr);
            OldWeatherType = NewWeatherType;
            if (ForcedWeatherType >= 0) {
                NewWeatherType = ForcedWeatherType;
            } else if (TheCamera.GetPosition().z < 950.0f) {
                WeatherTypeInList = (WeatherTypeInList + 1) % 64;
                NewWeatherType = FindWeatherTypesList()[WeatherTypeInList];
            }
        }
        InterpolationValue = interpolation;
    }

    // 0x72B96E
    if (IsRainy(NewWeatherType) && IsRainy(OldWeatherType) && !CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() && UnderWaterness <= 0.0f && !CGame::currArea) {
        if (LightningBurst) {
            if ((CGeneral::GetRandomNumber() & 0xFF) < 24) {
                LightningBurst = false;
                LightningDuration = std::min(CTimer::GetFrameCounter() - LightningStart, 20u);
                WhenToPlayLightningSound = (int32)((20 - LightningDuration) * 150 + CTimer::GetTimeInMS());
                LightningFlash = false;
            } else if (CTimer::GetTimeInMS() - LightningFlashLastChange > 50) {
                const auto wasFlashing = LightningFlash;
                LightningFlash = CGeneral::GetRandomNumber() & 1;
                if (LightningFlash != wasFlashing) {
                    LightningFlashLastChange = CTimer::GetTimeInMS();
                }
            }
        } else if (CGeneral::GetRandomNumber() < 200) {
            LightningStart = CTimer::GetFrameCounter();
            LightningFlashLastChange = CTimer::GetTimeInMS();
            LightningBurst = true;
            LightningFlash = true;
        } else {
            LightningFlash = false;
        }
    } else {
        LightningFlash = false;
        LightningBurst = false;
    }

    // 0x72BB25
    if (WhenToPlayLightningSound && CTimer::GetTimeInMS() > (uint32)WhenToPlayLightningSound) {
        m_WeatherAudioEntity.AddAudioEvent(AE_THUNDER);
        CPad::GetPad(0)->StartShake((int16)(40 * LightningDuration + 100), (uint8)((LightningDuration + 40) * 2), 0);
        WhenToPlayLightningSound = 0;
    }

    // 0x72BB87
    if (IsRainy(OldWeatherType)) {
        WetRoads = IsRainy(NewWeatherType) ? 1.0f : 1.0f - InterpolationValue;
    } else {
        WetRoads = IsRainy(NewWeatherType) ? InterpolationValue : 0.0f;
    }

    // 0x72BBE1
    const auto StepTo = [step = CTimer::GetTimeStep() * 0.005f](float value, float target) {
        const auto delta = target - value;
        if (std::abs(delta) < step) {
            return target;
        }
        return delta > 0.0f ? value + step : value - step;
    };
    const auto intensity = (float)((CTimer::GetTimeInMS() >> 13) % 4) * 0.1f + 0.7f;

    auto rain = IsRainy(NewWeatherType) ? InterpolationValue : 0.0f;
    if (IsRainy(OldWeatherType)) {
        rain += 1.0f - InterpolationValue;
    }
    Rain = StepTo(Rain, intensity * rain);

    // 0x72BC98
    auto sandstorm = NewWeatherType == WEATHER_SANDSTORM_DESERT ? InterpolationValue : 0.0f;
    if (OldWeatherType == WEATHER_SANDSTORM_DESERT) {
        sandstorm += 1.0f - InterpolationValue;
    }
    Sandstorm = StepTo(Sandstorm, intensity * sandstorm);

    // 0x72BD11
    CloudCoverage = (IsSunny(OldWeatherType) || IsExtraSunny(OldWeatherType)) ? 0.0f : 1.0f - InterpolationValue;
    if (!IsSunny(NewWeatherType) && !IsExtraSunny(NewWeatherType)) {
        CloudCoverage += InterpolationValue;
    }

    // 0x72BDD1
    Foggyness = IsFoggy(OldWeatherType) ? 1.0f - InterpolationValue : 0.0f;
    if (IsFoggy(NewWeatherType)) {
        Foggyness += InterpolationValue;
    }

    // 0x72BE19
    Foggyness_SF = OldWeatherType == WEATHER_FOGGY_SF ? 1.0f - InterpolationValue : 0.0f;
    if (NewWeatherType == WEATHER_FOGGY_SF) {
        Foggyness_SF += InterpolationValue;
    }

    // 0x72BE55
    ExtraSunnyness = IsExtraSunny(OldWeatherType) ? 1.0f - InterpolationValue : 0.0f;
    if (IsExtraSunny(NewWeatherType)) {
        ExtraSunnyness += InterpolationValue;
    }

    // 0x72BECB
    if (IsCloudy(OldWeatherType) && IsSunny(NewWeatherType) && InterpolationValue < 0.5f && CClock::ms_nGameClockHours > 6 && CClock::ms_nGameClockHours < 21) {
        Rainbow = 1.0f - std::abs(InterpolationValue - 0.25f) * 4.0f;
    } else {
        Rainbow = 0.0f;
    }

    // 0x72BF63
    SunGlare = (IsExtraSunny(OldWeatherType) || IsSunny(OldWeatherType)) ? 1.0f - InterpolationValue : 0.0f;
    if (IsExtraSunny(NewWeatherType) || IsSunny(NewWeatherType)) {
        SunGlare += InterpolationValue;
    }

    // 0x72C021
    if (SunGlare > 0.0f) {
        SunGlare *= std::min(CTimeCycle::GetVectorToSun().z * 7.0f, 1.0f);
        SunGlare = std::clamp(SunGlare, 0.0f, 1.0f);
        if (!CSpecialFX::bSnapShotActive) {
            SunGlare *= 1.0f - (float)(CGeneral::GetRandomNumber() % 32) * 0.007f;
        }
    }

    // 0x72C0FB
    HeatHaze = IsHeatHazy(OldWeatherType) ? 1.0f - InterpolationValue : 0.0f;
    if (IsHeatHazy(NewWeatherType)) {
        HeatHaze += InterpolationValue;
    }

    // 0x72C157
    if (HeatHaze > 0.0f) {
        const auto minutes       = CClock::ms_nGameClockMinutes;
        const auto hours         = CClock::ms_nGameClockHours;
        const auto minuteChanged = minutes != HeatHazeFXLastMinute;

        HeatHazeFXControl = 0.0f;
        auto fadeSpeed = CPostEffects::m_fHeatHazeFXFadeSpeed;

        const auto fadeIn  = hours >= CPostEffects::m_HeatHazeFXHourOfDayStart && hours < CPostEffects::m_HeatHazeFXHourOfDayEnd;
        const auto fadeOut = hours >= CPostEffects::m_HeatHazeFXHourOfDayEnd;

        // 0x72C1C8
        const auto isOutside = !CGame::currArea
            && FindPlayerPed()->GetAreaCode() == AREA_CODE_NORMAL_WORLD
            && !CCullZones::CamNoRain()
            && !CCullZones::PlayerNoRain();
        if (!isOutside) {
            fadeSpeed = CPostEffects::m_fHeatHazeFXInsideBuildingFadeSpeed;
        }

        // 0x72C20E
        if (isOutside && fadeIn) {
            if (minuteChanged) {
                HeatHazeFXFade += fadeSpeed;
            }
            HeatHazeFXFade = std::min(HeatHazeFXFade, 1.0f);
            HeatHazeFXControl = HeatHazeFXFade;
        }

        // 0x72C26D
        if (!isOutside || fadeOut) {
            if (minuteChanged) {
                HeatHazeFXFade -= fadeSpeed;
            }
            HeatHazeFXFade = std::max(HeatHazeFXFade, 0.0f);
            HeatHazeFXControl = HeatHazeFXFade;
        }

        HeatHazeFXControl *= HeatHaze;
        HeatHazeFXLastMinute = minutes;
    }

    // 0x72C2C9
    const auto waterFogAlpha = CTimeCycle::m_CurrentColours.m_nWaterFogAlpha;
    const auto waterFog      = (float)waterFogAlpha * 0.01f;
    if (!waterFogAlpha) {
        WaterFogFXFade = 0.0f;
    } else if (waterFogAlpha >= 95) {
        WaterFogFXFadingOut = true;
    }
    if (WaterFogFXFadingOut) {
        WaterFogFXFade = std::min(WaterFogFXFade, waterFog);
        if (WaterFogFXFade <= 0.0f) {
            WaterFogFXFadingOut = false;
        }
    } else {
        WaterFogFXFade = std::max(WaterFogFXFade, waterFog);
    }
    WaterFogFXControl = std::clamp(WaterFogFXFade * 1.4f, 0.0f, 1.0f);

    // 0x72C398
    Wind = WindForWeatherType[OldWeatherType] * (1.0f - InterpolationValue) + WindForWeatherType[NewWeatherType] * InterpolationValue;
    WindClipped = std::min(Wind, 1.0f);

    // 0x72C3F3
    WindDir.x = WindClipped * 0.7f;
    WindDir.y = WindClipped * 0.7f;

    const auto timeMs = CTimer::GetTimeInMS();

    const auto slowIdx = (timeMs >> 10) % 16;
    const auto slowT   = 0.5f - std::cos((float)(timeMs % 1024) / 1024.0f * std::numbers::pi_v<float>) * 0.5f;
    const auto slowTInv = 1.0f - slowT;
    auto windX = (slowT * WindDirOffsets[(slowIdx + 1) % 16] + slowTInv * WindDirOffsets[slowIdx]) * WindClipped * 0.4f + WindDir.x;
    auto windY = (slowTInv * WindDirOffsets[(slowIdx + 3) % 16] + slowT * WindDirOffsets[(slowIdx + 4) % 16]) * WindClipped * 0.4f + WindDir.y;
    WindDir.z  = (slowTInv * WindDirOffsets[(slowIdx + 6) % 16] + slowT * WindDirOffsets[(slowIdx + 7) % 16]) * WindClipped * 0.2f;

    // 0x72C4F5
    if (const auto gust = (WindClipped - 0.5f) * 0.4f; gust > 0.0f) {
        const auto fastIdx  = (timeMs >> 8) % 16;
        const auto fastT    = (float)(timeMs % 256) / 256.0f;
        const auto fastTInv = 1.0f - fastT;
        windX     += (fastT * WindDirOffsets[(fastIdx + 1) % 16] + fastTInv * WindDirOffsets[fastIdx]) * gust;
        windY     += (fastTInv * WindDirOffsets[(fastIdx + 3) % 16] + fastT * WindDirOffsets[(fastIdx + 4) % 16]) * gust;
        WindDir.z += (fastTInv * WindDirOffsets[(fastIdx + 6) % 16] + fastT * WindDirOffsets[(fastIdx + 7) % 16]) * gust;
    }

    // 0x72C5B6
    const auto scaleIdx = (timeMs >> 11) % 16;
    const auto scaleT   = 0.5f - std::cos((float)(timeMs % 2048) / 2048.0f * std::numbers::pi_v<float>) * 0.5f;
    const auto scale    = (1.0f - scaleT) * WindDirScales[scaleIdx] + scaleT * WindDirScales[(scaleIdx + 1) % 16];
    WindDir.x  = scale * windX;
    WindDir.y  = scale * windY;
    WindDir.z *= scale;

    // 0x72C63C
    Wavyness = std::min(WindClipped + 0.3f, 1.0f);
    Rain     = std::min(Rain, 1.0f - UnderWaterness);

    // 0x72C690
    if (CClock::ms_nGameClockHours > 20) {
        TrafficLightsBrightness = 1.0f;
    } else if (CClock::ms_nGameClockHours > 19) {
        TrafficLightsBrightness = (float)CClock::ms_nGameClockMinutes * (1.0f / 60.0f);
    } else if (CClock::ms_nGameClockHours > 6) {
        TrafficLightsBrightness = 0.0f;
    } else if (CClock::ms_nGameClockHours > 5) {
        TrafficLightsBrightness = 1.0f - (float)CClock::ms_nGameClockMinutes * (1.0f / 60.0f);
    } else {
        TrafficLightsBrightness = 1.0f;
    }

    // 0x72C6FA
    HeadLightsSpectrum      = std::min(std::max(Rain, Foggyness), 1.0f);
    TrafficLightsBrightness = std::max({ TrafficLightsBrightness, WetRoads, Foggyness, Rain });

    AddRain();

    // 0x72C7C5
    const auto* const task = FindPlayerPed()->GetTaskManager().GetSimplestActiveTask();
    const auto isClimbing = task && task->GetTaskType() == TASK_SIMPLE_CLIMB;
    if ((IsSunny(NewWeatherType) || IsExtraSunny(NewWeatherType)) && !CGame::currArea && !CCutsceneMgr::IsRunning() && CTimer::GetFrameCounter() % 8 == 0 && isClimbing) {
        FindPlayerPed(); // Body is empty in the original apart from this call
    }

    UpdateInTunnelness();
    m_WeatherAudioEntity.Service();
}

// 0x72B630
void CWeather::UpdateInTunnelness() {
    ZoneScoped;

    static const CVector s_TunnelPoint1{ 85.0f, -1020.0f, 0.0f }; // 0xC81430
    static const CVector s_TunnelPoint2{ 1683.0f, -1956.0f, 0.0f }; // 0xC81424

    float target = 0.0f;
    if (CCullZones::CurrentFlags_Camera & 0x2000) { // TODO: Unnamed tunnel-related eZoneAttributes flag (bit 0x2000)
        const CVector from{ CVector2D{ TheCamera.GetPosition() } };
        const CVector to = from + CVector{ CVector2D{ TheCamera.GetForwardVector() }.Normalized() } * 100.0f;
        const auto dist = std::min({
            CCollision::DistToLine(from, to, s_TunnelPoint1),
            CCollision::DistToLine(from, to, s_TunnelPoint2),
            100.0f,
        });
        target = std::min(1.0f, dist / 100.0f);
    }

    InTunnelness = notsa::step_to(InTunnelness, target, CTimer::GetTimeStep() * 0.01f);
}

// Based on 0x72A640
eWeatherRegion CWeather::FindWeatherRegion(CVector2D pos) {
    if (pos.x > 1000.0f && pos.y > 910.0f) {
        return WEATHER_REGION_LV;
    }
    if (pos.x > -850.0f && pos.x < 1000.0f && pos.y > 1280.0f) {
        return WEATHER_REGION_DESERT;
    }
    if (pos.x < -1430.0f && pos.y > -580.0f && pos.y < 1430.0f) {
        return WEATHER_REGION_SF;
    }
    if (pos.x > 250.0f && pos.x < 3000.0f && pos.y > -3000.0f && pos.y < -850.0f) {
        return WEATHER_REGION_LA;
    }
    return WEATHER_REGION_DEFAULT;
}

// 0x72A640
void CWeather::UpdateWeatherRegion(CVector* posn) {
    WeatherRegion = FindWeatherRegion(posn ? *posn : TheCamera.GetPosition());
}

// 0x4ABF50
bool CWeather::IsRainy() {
    return Rain >= 0.2f;
}
