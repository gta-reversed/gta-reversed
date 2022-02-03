/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "eWeatherType.h"
#include "game_sa/Data/Weather.def"

float& CWeather::TrafficLightsBrightness = *(float*)0xC812A8;
bool& CWeather::bScriptsForceRain = *(bool*)0xC812AC;
float& CWeather::Earthquake = *(float*)0xC81340;
uint32& CWeather::CurrentRainParticleStrength = *(uint32*)0xC812B0;
uint32& CWeather::LightningStartY = *(uint32*)0xC812B4;
uint32& CWeather::LightningStartX = *(uint32*)0xC812B8;
int32& CWeather::LightningFlashLastChange = *(int32*)0xC812BC;
int32& CWeather::WhenToPlayLightningSound = *(int32*)0xC812C0;
uint32& CWeather::LightningDuration = *(uint32*)0xC812C4;
uint32& CWeather::LightningStart = *(uint32*)0xC812C8;
bool& CWeather::LightningFlash = *(bool*)0xC812CC;
bool& CWeather::LightningBurst = *(bool*)0xC812CD;
float& CWeather::HeadLightsSpectrum = *(float*)0xC812D0;
float& CWeather::WaterFogFXControl = *(float*)0xC81338;
float& CWeather::HeatHazeFXControl = *(float*)0xC812D8;
float& CWeather::HeatHaze = *(float*)0xC812DC;
float& CWeather::SunGlare = *(float*)0xC812E0;
float& CWeather::Rainbow = *(float*)0xC812E4;
float& CWeather::Wavyness = *(float*)0xC812E8;
float& CWeather::WindClipped = *(float*)0xC812EC;
CVector& CWeather::WindDir = *(CVector*)0xC813E0;
float& CWeather::Wind = *(float*)0xC812F0;
float& CWeather::Sandstorm = *(float*)0xC812F4;
float& CWeather::Rain = *(float*)0xC81324;
float& CWeather::InTunnelness = *(float*)0xC81334;
float& CWeather::WaterDepth = *(float*)0xC81330;
float& CWeather::UnderWaterness = *(float*)0xC8132C;
float& CWeather::ExtraSunnyness = *(float*)0xC812F8;
float& CWeather::Foggyness_SF = *(float*)0xC812FC;
float& CWeather::Foggyness = *(float*)0xC81300;
float& CWeather::CloudCoverage = *(float*)0xC81304;
float& CWeather::WetRoads = *(float*)0xC81308;
float& CWeather::InterpolationValue = *(float*)0xC8130C;
uint32& CWeather::WeatherTypeInList = *(uint32*)0xC81310;
eWeatherRegion& CWeather::WeatherRegion = *(eWeatherRegion*)0xC81314;
eWeatherType& CWeather::ForcedWeatherType = *(eWeatherType*)0xC81318;
eWeatherType& CWeather::NewWeatherType = *(eWeatherType*)0xC8131C;
eWeatherType& CWeather::OldWeatherType = *(eWeatherType*)0xC81320;
CAEWeatherAudioEntity& CWeather::m_WeatherAudioEntity = *(CAEWeatherAudioEntity*)0xC81360;
bool& CWeather::StreamAfterRainTimer = *(bool*)0x8D5EAC;

float(&CWeather::saTreeWindOffsets)[16] = *(float(*)[16])0x8CCF30;
float(&CWeather::saBannerWindOffsets)[32] = *(float(*)[32])0x8CCF70;


void CWeather::InjectHooks() {
    RH_ScopedClass(CWeather);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x72A480);
    // RH_ScopedInstall(AddRain, 0x72A9A0);
    // RH_ScopedInstall(AddSandStormParticles, 0x72A820);
    // RH_ScopedInstall(FindWeatherTypesList, 0x72A520, true); // bad
    RH_ScopedInstall(ForceWeather, 0x72A4E0);
    RH_ScopedInstall(ForceWeatherNow, 0x72A4F0);
    // RH_ScopedInstall(ForecastWeather, 0x72A590);
    RH_ScopedInstall(ReleaseWeather, 0x72A510);
    RH_ScopedInstall(RenderRainStreaks, 0x72AF70);
    RH_ScopedInstall(SetWeatherToAppropriateTypeNow, 0x72A790);
    // RH_ScopedInstall(Update, 0x72B850);
    // RH_ScopedInstall(UpdateInTunnelness, 0x72B630);
    // RH_ScopedInstall(UpdateWeatherRegion, 0x72A640, true); // bad
    RH_ScopedInstall(IsRainy, 0x4ABF50);
}

// 0x72A480
void CWeather::Init() {
    NewWeatherType = WEATHER_EXTRASUNNY_LA;
    OldWeatherType = WEATHER_EXTRASUNNY_LA;
    WeatherRegion = WEATHER_REGION_DEFAULT;
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
    StreamAfterRainTimer = false;
}

// 0x72A9A0
void CWeather::AddRain() {
    plugin::Call<0x72A9A0>();
}

// 0x72A820
void CWeather::AddSandStormParticles() {
    plugin::Call<0x72A820>();
}

// todo: fixme
// 0x72A520
const eWeatherType* CWeather::FindWeatherTypesList() {
    return plugin::CallAndReturn<const eWeatherType*, 0x72A520>();

    switch (WeatherRegion) {
    case WEATHER_REGION_LA:
        return WeatherTypesListLA;

    case WEATHER_REGION_SF:
        return WeatherTypesListSF;

    case WEATHER_REGION_LV:
        return WeatherTypesListVegas;

    case WEATHER_REGION_DESERT:
        return WeatherTypesListDesert;

    default:
        return WeatherTypesListDefault;
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
    return plugin::CallAndReturn<bool, 0x72A590, int32, int32>(weatherType, numSteps);
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
        const auto strength = (uint32)(64.0f - CTimeCycle::m_FogReduction) * (Rain * 110.0f) / 64.0f;
        if (CurrentRainParticleStrength < strength) {
            if (CurrentRainParticleStrength + 1 <= strength)
                CurrentRainParticleStrength++;
        } else {
            if (CurrentRainParticleStrength > 0)
                CurrentRainParticleStrength--;
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
    static int32*& streakPosX = *(int32**)0xC81420; // TODO | STATICREF
    static int32*& streakPosY = *(int32**)0xC8141C; // TODO | STATICREF
    static int32*& streakPosZ = *(int32**)0xC81418; // TODO | STATICREF
    static uint8*& streakStrength = *(uint8**)0xC81414; // TODO | STATICREF

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
            streakStrength[i] = (uint8)(CurrentRainParticleStrength * 0.6f);
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

            streakStrength[i] = (uint8)(CurrentRainParticleStrength * 0.6f);
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

        const uint8 alphas[]{ streakStrength[s], streakStrength[s] / 2u }; // todo: Non-constant-expression cannot be narrowed from type 'unsigned int' to 'uint8' (aka 'unsigned char') in initializer list
        for (unsigned v = 0; v < 2; v++) {
            RxObjSpace3DVertex* vertex = &aTempBufferVertices[GetRealVertexIndex(v)];

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

    if (RwIm3DTransform(aTempBufferVertices, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ)) {
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
    CVector playerCoors = FindPlayerCoors(-1);
    UpdateWeatherRegion(&playerCoors);

    auto weatherType = static_cast<eWeatherType>(*FindWeatherTypesList());
    ForcedWeatherType = WEATHER_UNDEFINED;
    OldWeatherType = weatherType;
    NewWeatherType = weatherType;
}

// 0x72B850
void CWeather::Update() {
    plugin::Call<0x72B850>();
}

// 0x72B630
void CWeather::UpdateInTunnelness() {
    plugin::Call<0x72B630>();
}

// todo: fixme
// 0x72A640
void CWeather::UpdateWeatherRegion(CVector* posn) {
    CVector vecPos = TheCamera.GetPosition();
    if (posn) {
        vecPos = *posn;
    }
    if (vecPos.x > 1000.0f && vecPos.y > 910.0f) {
        WeatherRegion = WEATHER_REGION_LV;
        return;
    }
    if (vecPos.x > -850.0f && vecPos.x < 1000.0f && vecPos.y > 1280.0f) {
        WeatherRegion = WEATHER_REGION_DESERT;
        return;
    }
    if (vecPos.x < -1430.0f && vecPos.y > 580.0f && vecPos.y < 1430.0f) {
        WeatherRegion = WEATHER_REGION_SF;
        return;
    }
    if (vecPos.x > 250.0f && vecPos.x < 3000.0f && vecPos.y > -3000.0f && vecPos.y < -850.0f) {
        WeatherRegion = WEATHER_REGION_LA;
        return;
    }
    WeatherRegion = WEATHER_REGION_DEFAULT;
}

// 0x4ABF50
bool CWeather::IsRainy() {
    return Rain >= 0.2f;
}
