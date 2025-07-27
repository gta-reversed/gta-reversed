#include "StdInc.h"

#include "TimeCycle.h"
#include "PostEffects.h"
#include "Shadows.h"

int &TunnelWeather = *(int*)0x8CDEE0; // 9 = WEATHER_FOGGY_SF, unchanged

void CTimeCycle::InjectHooks() {
    RH_ScopedClass(CTimeCycle);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BBAC0);
    RH_ScopedInstall(InitForRestart, 0x5601F0);
    RH_ScopedInstall(Shutdown, 0x5601E0);
    RH_ScopedInstall(Update, 0x561760);
    RH_ScopedInstall(StartExtraColour, 0x55FEC0);
    RH_ScopedInstall(StopExtraColour, 0x55FF20);
    RH_ScopedInstall(AddOne, 0x55FF40);
    RH_ScopedInstall(CalcColoursForPoint, 0x5603D0);
    RH_ScopedInstall(FindFarClipForCoors, 0x5616E0);
    RH_ScopedInstall(FindTimeCycleBox, 0x55FFD0);
    RH_ScopedInstall(SetConstantParametersForPostFX, 0x560210);
    RH_ScopedInstall(GetAmbientRed, 0x560330);
    RH_ScopedInstall(GetAmbientGreen, 0x560340);
    RH_ScopedInstall(GetAmbientBlue, 0x560350);
    RH_ScopedInstall(GetAmbientRed_BeforeBrightness, 0x560390);
    RH_ScopedInstall(GetAmbientGreen_BeforeBrightness, 0x5603A0);
    RH_ScopedInstall(GetAmbientBlue_BeforeBrightness, 0x5603B0);
    RH_ScopedInstall(GetAmbientRed_Obj, 0x560360);
    RH_ScopedInstall(GetAmbientGreen_Obj, 0x560370);
    RH_ScopedInstall(GetAmbientBlue_Obj, 0x560380);
}

// 0x5BBAC0
void CTimeCycle::Initialise() {
    CFileMgr::SetDir("DATA");
    auto file = CFileMgr::OpenFile("TIMECYC.DAT", "rb");
    CFileMgr::SetDir("");

    if (!file) { // NOTSA
        NOTSA_LOG_WARN("[CTimeCycle] Failed to open TIMECYC.DAT");
        CFileMgr::CloseFile(file);
        return;
    }

    int32 ambR, ambG, ambB;
    int32 ambObjR, ambObjG, ambObjB;
    int32 dirR, dirG, dirB;
    int32 skyTopR, skyTopG, skyTopB;
    int32 skyBotR, skyBotG, skyBotB;
    int32 sunCoreR, sunCoreG, sunCoreB;
    int32 sunCoronaR, sunCoronaG, sunCoronaB;
    float sunSize, spriteSize, spriteBrightness;
    int32 shadowStrength, lightShadowStrength, poleShadowStrength;
    float farClip, fogStart, lightOnGround;
    int32 lowCloudR, lowCloudG, lowCloudB;
    int32 bottomCloudR, bottomCloudG, bottomCloudB;
    float waterR, waterG, waterB, waterA;
    float postFx1A, postFx1R, postFx1G, postFx1B;
    float postFx2A, postFx2R, postFx2G, postFx2B;
    float cloudAlpha;
    int32 highLightMinIntensity;
    int32 waterFogAlpha;
    float dirMult;

    char* line;
    for (auto w = 0; w < NUM_WEATHERS; w++) {
        for (auto h = 0; h < NUM_HOURS; h++) {
            while (line = CFileLoader::LoadLine(file)) {
                if (line[0] != '/' && line[0] != '\0') {
                    break;
                }
            }

            // TODO: Fix in original TIMECYC.dat line 320 (255 ... -> 22 22 22 ...)
            const auto n = sscanf(line,
                "%d %d %d " // Static ambience color
                "%d %d %d " // Dynamic ambience color
                "%d %d %d " // Direct light color - NOP
                "%d %d %d " // Sky top color
                "%d %d %d " // Sky bottom color
                "%d %d %d " // Sun core color
                "%d %d %d " // Sun corona color
                "%f "       // Sun core size
                "%f "       // Sun corona size
                "%f "       // Sprite brightness
                "%d "       // Pole shading value
                "%d "       // Light shading value
                "%d "       // Pole shading value
                "%f "       // Far clipping offset
                "%f "       // Fog start offset
                "%f "       // Light on ground
                "%d %d %d " // Lower clouds color
                "%d %d %d " // Upper clouds bottom color
                "%f %f %f " // Water color
                "%f "       // Water alpha level
                "%f "       // Color correction 1 alpha
                "%f %f %f " // Color correction 1
                "%f "       // Color correction 2 alpha
                "%f %f %f " // Color correction 2
                "%f "       // Lower clouds alpha level
                "%d "       // Highlight min intensity
                "%d "       // Water fog alpha
                "%f",       // Directional multiplier
                &ambR, &ambG, &ambB,
                &ambObjR, &ambObjG, &ambObjB,
                &dirR, &dirG, &dirB,
                &skyTopR, &skyTopG, &skyTopB,
                &skyBotR, &skyBotG, &skyBotB,
                &sunCoreR, &sunCoreG, &sunCoreB,
                &sunCoronaR, &sunCoronaG, &sunCoronaB,
                &sunSize, &spriteSize, &spriteBrightness,
                &shadowStrength, &lightShadowStrength, &poleShadowStrength,
                &farClip, &fogStart, &lightOnGround,
                &lowCloudR, &lowCloudG, &lowCloudB,
                &bottomCloudR, &bottomCloudG, &bottomCloudB,
                &waterR, &waterG, &waterB, &waterA,
                &postFx1A, &postFx1R, &postFx1G, &postFx1B,
                &postFx2A, &postFx2R, &postFx2G, &postFx2B,
                &cloudAlpha, &highLightMinIntensity, &waterFogAlpha, &dirMult
            );
            if (n != 52 && n != 51) {
                NOTSA_LOG_WARN("Bad timecyc line: '{}'", line); // TODO: fix
            }

            m_nAmbientRed[h][w]   = ambR;
            m_nAmbientGreen[h][w] = ambG;
            m_nAmbientBlue[h][w]  = ambB;

            m_nAmbientRed_Obj[h][w]   = ambObjR;
            m_nAmbientGreen_Obj[h][w] = ambObjG;
            m_nAmbientBlue_Obj[h][w]  = ambObjB;

            // code dir RGB?

            m_nSkyTopRed[h][w]   = skyTopR;
            m_nSkyTopGreen[h][w] = skyTopG;
            m_nSkyTopBlue[h][w]  = skyTopB;

            m_nSkyBottomRed[h][w]   = skyBotR;
            m_nSkyBottomGreen[h][w] = skyBotG;
            m_nSkyBottomBlue[h][w]  = skyBotB;

            m_nSunCoreRed[h][w]   = sunCoreR;
            m_nSunCoreGreen[h][w] = sunCoreG;
            m_nSunCoreBlue[h][w]  = sunCoreB;

            m_nSunCoronaRed[h][w]   = sunCoronaR;
            m_nSunCoronaGreen[h][w] = sunCoronaG;
            m_nSunCoronaBlue[h][w]  = sunCoronaB;

            m_fSunSize[h][w] = int8(sunSize * 10.0f + 0.5f);
            m_fSpriteSize[h][w] = int8(spriteSize * 10.0f + 0.5f);
            m_fSpriteBrightness[h][w] = int8(spriteBrightness * 10.0f + 0.5f);

            m_nShadowStrength[h][w]      = shadowStrength;
            m_nLightShadowStrength[h][w] = lightShadowStrength;
            m_nPoleShadowStrength[h][w]  = poleShadowStrength;

            m_fFarClip[h][w]  = (int16)farClip;
            m_fFogStart[h][w] = (int16)fogStart;
            m_fLightsOnGroundBrightness[h][w] = uint8(lightOnGround * 10.0f + 0.5f);

            m_nLowCloudsRed[h][w]   = lowCloudR;
            m_nLowCloudsGreen[h][w] = lowCloudG;
            m_nLowCloudsBlue[h][w]  = lowCloudB;

            m_nFluffyCloudsBottomRed[h][w]   = (uint8)bottomCloudR;
            m_nFluffyCloudsBottomGreen[h][w] = (uint8)bottomCloudG;
            m_nFluffyCloudsBottomBlue[h][w]  = (uint8)bottomCloudB;

            m_fWaterRed[h][w]   = (uint8)waterR;
            m_fWaterGreen[h][w] = (uint8)waterG;
            m_fWaterBlue[h][w]  = (uint8)waterB;
            m_fWaterAlpha[h][w] = (uint8)waterA;

            m_fPostFx1Red[h][w]   = (uint8)postFx1R;
            m_fPostFx1Green[h][w] = (uint8)postFx1G;
            m_fPostFx1Blue[h][w]  = (uint8)postFx1B;

            m_fPostFx2Red[h][w]   = (uint8)postFx2R;
            m_fPostFx2Green[h][w] = (uint8)postFx2G;
            m_fPostFx2Blue[h][w]  = (uint8)postFx2B;

            m_fPostFx1Alpha[h][w] = uint8(postFx1A * 2.f);
            m_fPostFx2Alpha[h][w] = uint8(postFx2A * 2.f);

            m_fCloudAlpha[h][w]            = (uint8)cloudAlpha;
            m_nHighLightMinIntensity[h][w] = (uint8)highLightMinIntensity;
            m_nWaterFogAlpha[h][w]         = (uint8)waterFogAlpha;
            m_nDirectionalMult[h][w]       = uint8((uint8)dirMult * 100.0f);
        }
    }

    CFileMgr::CloseFile(file);
                                                                               // Android values
    m_vecDirnLightToSun.x = std::cos(-3.0f * PI / 4.0f) * std::cos(PI / 4.0f); // -0.5f
    m_vecDirnLightToSun.y = std::sin(-3.0f * PI / 4.0f) * std::cos(PI / 4.0f); // -0.5f
    m_vecDirnLightToSun.z = std::sin(PI / 4.0f);                               // std::cos(-3 * rwPI / 3.0f) * std::cos(rwPI / 4.0f);
    m_vecDirnLightToSun.Normalise();
    m_FogReduction = 0;
    m_bExtraColourOn = false;
}

// 0x5601F0
void CTimeCycle::InitForRestart() {
    StopExtraColour(false);
}

// 0x5601E0
void CTimeCycle::Shutdown() {
    // NOP
}

// 0x561760
void CTimeCycle::Update() {
    ZoneScoped;

    CalcColoursForPoint(TheCamera.GetPosition(), &m_CurrentColours);
}

// 0x55FEC0
void CTimeCycle::StartExtraColour(int32 color, bool bNoExtraColorInterior) {
    m_ExtraColourWeatherType = color / NUM_HOURS + WEATHER_EXTRA_START;
    m_ExtraColour = color % NUM_HOURS;
    m_bExtraColourOn = true;
    m_ExtraColourInter = 0.0f;
    if (!bNoExtraColorInterior) {
        m_ExtraColourInter = 1.0f;
    }
}

// 0x55FF20
void CTimeCycle::StopExtraColour(bool bNoExtraColorInterior) {
    m_bExtraColourOn = false;
    if (!bNoExtraColorInterior) {
        m_ExtraColourInter = 0.0f;
    }
}

// 0x55FF40
void CTimeCycle::AddOne(CBox& box, int16 farClip, int32 m_ExtraColor, float strength, float falloff, float lodDistMult) {
    m_aBoxes[m_NumBoxes].Box = box;
    m_aBoxes[m_NumBoxes].FarClip = farClip;
    m_aBoxes[m_NumBoxes].ExtraColor = m_ExtraColor;
    m_aBoxes[m_NumBoxes].Strength = strength / 100.0f;
    m_aBoxes[m_NumBoxes].Falloff = falloff;
    m_aBoxes[m_NumBoxes].LodDistMult = (uint8)(std::min(lodDistMult, 4.0f) * 32.0f);
    m_NumBoxes++;
}

// 0x5603D0
void CTimeCycle::CalcColoursForPoint(CVector point, CColourSet* set) {
    constexpr auto TimeSamples         = std::to_array({ 0, 5, 6, 7, 12, 19, 20, 22, 24 }); // 0x8CDECC
    constexpr auto GreyValuesDuringDay = std::to_array({ 30, 30, 30, 50, 60, 60, 50, 35 }); // 0x8CDED8

    CTimeCycleBox *lodBoxA, *farBoxA, *farBoxB, *weatherBox;
    float lodBoxA_T, farBoxA_T, farBoxB_T, weatherBox_T;

    // Find LOD box
    FindTimeCycleBox(point, &lodBoxA, &lodBoxA_T, true, false, nullptr);

    // Find far boxes
    FindTimeCycleBox(point, &farBoxA, &farBoxA_T, false, true, nullptr);
    if (farBoxA) {
        FindTimeCycleBox(point, &farBoxB, &farBoxB_T, false, true, farBoxA);
        if (farBoxB && farBoxB->Box.GetWidth() > farBoxA->Box.GetWidth()) {
            std::swap(farBoxA, farBoxB);
            std::swap(farBoxA_T, farBoxB_T);
        }
    } else {
        farBoxB = nullptr;
    }

    // Find current weather box
    FindTimeCycleBox(point, &weatherBox, &weatherBox_T, false, false, nullptr);

    // 0x560502 - Clamped hours
    const auto hours = std::min(23.999f, CClock::GetHoursToday());

    // 0x560528 - Find sample index for current hour
    int32 currSampleIdx = 0;
    while (hours >= (float)(TimeSamples[currSampleIdx + 1])) {
        currSampleIdx++;
    }
    const auto nextSampleIdx = (currSampleIdx + 1) % NUM_HOURS;

    const float timeT = invLerp((float)(TimeSamples[currSampleIdx]), (float)(TimeSamples[currSampleIdx + 1]), hours);
    const float invTimeT    = 1.0f - timeT;

    const float t    = CWeather::InterpolationValue;

    // 0x5605D5
    eWeatherType boxWeather{ WEATHER_UNDEFINED };
    int32        boxHour;
    if (weatherBox) {
        boxHour    = weatherBox->ExtraColor % 8;
        boxWeather = weatherBox->ExtraColor >= 8
            ? WEATHER_EXTRACOLOURS_2
            : WEATHER_EXTRACOLOURS_1;
    }
    
    const auto& camPos = TheCamera.GetPosition();
    float f = std::clamp((camPos.z - 20.0f) / 200.0f, 0.0f, 1.0f);

    { // 0x5606B7
        CColourSet currentOld(currSampleIdx, CWeather::OldWeatherType);
        CColourSet nextOld(nextSampleIdx,   CWeather::OldWeatherType);

        CColourSet currentNew(currSampleIdx, CWeather::NewWeatherType);
        CColourSet nextNew(nextSampleIdx,   CWeather::NewWeatherType);

        if (CWeather::OldWeatherType == WEATHER_EXTRASUNNY_SMOG_LA) {
            CColourSet set1(currSampleIdx, WEATHER_EXTRASUNNY_LA);
            currentOld.Interpolate(&currentOld, &set1, 1.0f - f, f, false);

            CColourSet set2(nextSampleIdx, WEATHER_EXTRASUNNY_LA);
            nextOld.Interpolate(&nextOld, &set2, 1.0f - f, f, false);
        } else if (CWeather::OldWeatherType == WEATHER_SUNNY_SMOG_LA) {
            CColourSet set1(currSampleIdx, WEATHER_SUNNY_LA);
            currentOld.Interpolate(&currentOld, &set1, 1.0f - f, f, false);

            CColourSet set2(nextSampleIdx, WEATHER_SUNNY_LA);
            nextOld.Interpolate(&nextOld, &set2, 1.0f - f, f, false);
        }

        if (CWeather::NewWeatherType == WEATHER_EXTRASUNNY_SMOG_LA) {
            CColourSet set1(currSampleIdx, WEATHER_EXTRASUNNY_LA);
            currentNew.Interpolate(&currentNew, &set1, 1.0f - f, f, false);

            CColourSet set2(nextSampleIdx, WEATHER_EXTRASUNNY_LA);
            nextNew.Interpolate(&nextNew, &set2, 1.0f - f, f, false);
        } else if (CWeather::NewWeatherType == WEATHER_SUNNY_SMOG_LA) {
            CColourSet set1(currSampleIdx, WEATHER_SUNNY_LA);
            currentNew.Interpolate(&currentNew, &set1, 1.0f - f, f, false);

            CColourSet set2(nextSampleIdx, WEATHER_SUNNY_LA);
            nextNew.Interpolate(&nextNew, &set2, 1.0f - f, f, false);
        }

        { // 0x560877
            CColourSet a{}, b{};
            a.Interpolate(&currentOld, &nextOld, invTimeT, timeT, false);
            b.Interpolate(&currentNew, &nextNew, invTimeT, timeT, false);
            set->Interpolate(&a, &b, 1.f - t, t, false);
        }
    }

    // 0x5608E0 - Calculate sky colors
    {
        const float lightMult = (1.0f / CCoronas::LightsMult + 3.0f) * 0.25f;

        set->m_nSkyTopRed   = std::min<uint16>((uint16)((float)(set->m_nSkyTopRed) * lightMult), 255);
        set->m_nSkyTopGreen = std::min<uint16>((uint16)((float)(set->m_nSkyTopGreen) * lightMult), 255);
        set->m_nSkyTopBlue  = std::min<uint16>((uint16)((float)(set->m_nSkyTopBlue) * lightMult), 255);

        set->m_nSkyBottomRed   = std::min<uint16>((uint16)((float)(set->m_nSkyBottomRed) * lightMult), 255);
        set->m_nSkyBottomGreen = std::min<uint16>((uint16)((float)(set->m_nSkyBottomGreen) * lightMult), 255);
        set->m_nSkyBottomBlue  = std::min<uint16>((uint16)((float)(set->m_nSkyBottomBlue) * lightMult), 255);
    }

    if (m_FogReduction) {
        set->m_fFarClip = std::max(set->m_fFarClip, (float)m_FogReduction * 10.15625f); // 10.15625f = 1/64 * 10 + 10 ?
    }

    // 0x560A59
    m_CurrentStoredValue = (m_CurrentStoredValue + 1) & 15;

    // 0x560A67
    const float sunAngle = CClock::GetMinutesToday() * PI / 720.0f;
    m_VectorToSun[m_CurrentStoredValue] = CVector{
        +0.7f + std::sin(sunAngle),
        -0.7f,
        +0.2f - std::cos(sunAngle),
    }.Normalized();

    // 0x560AAE
    if (weatherBox && weatherBox->ExtraColor >= 0) {
        float boxf    = weatherBox_T * weatherBox->Strength;
        float invboxf = 1.0f - boxf;

        set->m_nSkyTopRed   = (uint16)(lerp<float>(set->m_nSkyTopRed, m_nSkyTopRed[boxHour][boxWeather], boxf));
        set->m_nSkyTopGreen = (uint16)(lerp<float>(set->m_nSkyTopGreen, m_nSkyTopGreen[boxHour][boxWeather], boxf));
        set->m_nSkyTopBlue  = (uint16)(lerp<float>(set->m_nSkyTopBlue, m_nSkyTopBlue[boxHour][boxWeather], boxf));

        if (m_nSkyBottomRed[boxHour][boxWeather] != 255) { // 0x560B6A
            set->m_nSkyBottomRed   = (uint16)(lerp<float>(set->m_nSkyBottomRed, m_nSkyBottomRed[boxHour][boxWeather], boxf));
            set->m_nSkyBottomGreen = (uint16)(lerp<float>(set->m_nSkyBottomGreen, m_nSkyBottomGreen[boxHour][boxWeather], boxf));
            set->m_nSkyBottomBlue  = (uint16)(lerp<float>(set->m_nSkyBottomBlue, m_nSkyBottomBlue[boxHour][boxWeather], boxf));
        }
        if (m_fWaterRed[boxHour][boxWeather] != 255) { // 0x560BED
            set->m_fWaterRed   = lerp<float>(set->m_fWaterRed, m_fWaterRed[boxHour][boxWeather], boxf);
            set->m_fWaterGreen = lerp<float>(set->m_fWaterGreen, m_fWaterGreen[boxHour][boxWeather], boxf);
            set->m_fWaterBlue  = lerp<float>(set->m_fWaterBlue, m_fWaterBlue[boxHour][boxWeather], boxf);
            set->m_fWaterAlpha = lerp<float>(set->m_fWaterAlpha, m_fWaterAlpha[boxHour][boxWeather], boxf);
        }
        if (m_nAmbientRed[boxHour][boxWeather] != 255) { // 0x560C5E
            set->m_fAmbientRed   = lerp<float>(set->m_fAmbientRed, m_nAmbientRed[boxHour][boxWeather], boxf);
            set->m_fAmbientGreen = lerp<float>(set->m_fAmbientGreen, m_nAmbientGreen[boxHour][boxWeather], boxf);
            set->m_fAmbientBlue  = lerp<float>(set->m_fAmbientBlue, m_nAmbientBlue[boxHour][boxWeather], boxf);
        }
        if (m_nAmbientRed_Obj[boxHour][boxWeather] != 255) { // 0x560CB2
            set->m_fAmbientRed_Obj   = lerp<float>(set->m_fAmbientRed_Obj, m_nAmbientRed_Obj[boxHour][boxWeather], boxf);
            set->m_fAmbientGreen_Obj = lerp<float>(set->m_fAmbientGreen_Obj, m_nAmbientGreen_Obj[boxHour][boxWeather], boxf);
            set->m_fAmbientBlue_Obj  = lerp<float>(set->m_fAmbientBlue_Obj, m_nAmbientBlue_Obj[boxHour][boxWeather], boxf);
        }
        if (m_fFarClip[boxHour][boxWeather] != -1) { // 0x560D08
            if (m_fFarClip[boxHour][boxWeather] < set->m_fFarClip) {
                set->m_fFarClip = set->m_fFarClip * invboxf + (float)m_fFarClip[boxHour][boxWeather] * boxf;
            }
        }
        if (m_fFogStart[boxHour][boxWeather] != -1) { // 0x560D3E
            set->m_fFogStart = lerp(set->m_fFogStart, (float)m_fFogStart[boxHour][boxWeather], boxf);
        }
        if (m_fPostFx1Red[boxHour][boxWeather] != 255) { // 0x560D63
            set->m_fPostFx1Red   = lerp(set->m_fPostFx1Red, (float)(m_fPostFx1Red[boxHour][boxWeather]), boxf);
            set->m_fPostFx1Green = lerp(set->m_fPostFx1Green, (float)(m_fPostFx1Green[boxHour][boxWeather]), boxf);
            set->m_fPostFx1Blue  = lerp(set->m_fPostFx1Blue, (float)(m_fPostFx1Blue[boxHour][boxWeather]), boxf);
            set->m_fPostFx1Alpha = lerp(set->m_fPostFx1Alpha, (float)(m_fPostFx1Alpha[boxHour][boxWeather]), boxf);
        }
        if (m_fPostFx2Red[boxHour][boxWeather] != 255) { // 0x560DE0
            set->m_fPostFx2Red   = lerp(set->m_fPostFx2Red, (float)(m_fPostFx2Red[boxHour][boxWeather]), boxf);
            set->m_fPostFx2Green = lerp(set->m_fPostFx2Green, (float)(m_fPostFx2Green[boxHour][boxWeather]), boxf);
            set->m_fPostFx2Blue  = lerp(set->m_fPostFx2Blue, (float)(m_fPostFx2Blue[boxHour][boxWeather]), boxf);
            set->m_fPostFx2Alpha = lerp(set->m_fPostFx2Alpha, (float)(m_fPostFx2Alpha[boxHour][boxWeather]), boxf);
        }
    }

    if (lodBoxA) {
        float newLodMult = (float)lodBoxA->LodDistMult / 32.0f; // 0.03125f = 1/32
        set->m_fLodDistMult  = lerp(set->m_fLodDistMult, newLodMult, lodBoxA_T);
    }

    if (farBoxA) {
        set->m_fFarClip = lerp(set->m_fFarClip, std::min(set->m_fFarClip, (float)farBoxA->FarClip), farBoxA_T);
    }
    if (farBoxB) {
        set->m_fFarClip = lerp(set->m_fFarClip, std::min(set->m_fFarClip, (float)farBoxB->FarClip), farBoxB_T);
    }

    float inc = CTimer::GetTimeStep() / 120.0f;
    m_ExtraColourInter = std::clamp(m_ExtraColourInter + (m_bExtraColourOn ? inc : -inc), 0.0f, 1.0f);

    if (m_ExtraColourInter > 0.0f) {
        CColourSet extra(m_ExtraColour, m_ExtraColourWeatherType);
        bool ignoreSky = ShouldIgnoreSky();
        set->Interpolate(set, &extra, 1.0f - m_ExtraColourInter, m_ExtraColourInter, ignoreSky);
    }

    if (CWeather::UnderWaterness > 0.0f) {
        CColourSet current(currSampleIdx, 20);
        CColourSet next(nextSampleIdx, 20);
        CColourSet tmp{};
        tmp.Interpolate(&current, &next, invTimeT, timeT, false);
        set->Interpolate(set, &tmp, 1.0f - CWeather::UnderWaterness, CWeather::UnderWaterness, false);
    }

    if (CWeather::InTunnelness > 0.0f) {
        CColourSet tunnel(TunnelWeather % NUM_HOURS, TunnelWeather / NUM_HOURS + WEATHER_EXTRA_START);
        bool ignoreSky = ShouldIgnoreSky();
        set->Interpolate(set, &tunnel, 1.0f - CWeather::InTunnelness, CWeather::InTunnelness, ignoreSky);
    }

    set->m_fAmbientRed /= 255.0f;
    set->m_fAmbientGreen /= 255.0f;
    set->m_fAmbientBlue /= 255.0f;
    set->m_fAmbientRed_Obj /= 255.0f;
    set->m_fAmbientGreen_Obj /= 255.0f;
    set->m_fAmbientBlue_Obj /= 255.0f;

    // 0x5612AA
    CShadows::CalcPedShadowValues(
        m_VectorToSun[m_CurrentStoredValue],
        m_fShadowFrontX[m_CurrentStoredValue],
        m_fShadowFrontY[m_CurrentStoredValue],
        m_fShadowSideX[m_CurrentStoredValue],
        m_fShadowSideY[m_CurrentStoredValue],
        m_fShadowDisplacementX[m_CurrentStoredValue],
        m_fShadowDisplacementY[m_CurrentStoredValue]
    );

    if (TheCamera.m_mCameraMatrix.GetForward().z < -0.9f || !CWeather::bScriptsForceRain
        && (CCullZones::PlayerNoRain() || CCullZones::CamNoRain() || CCutsceneMgr::ms_running)) {
        m_FogReduction = std::min(m_FogReduction + 1, 64);
    } else {
        m_FogReduction = std::max(m_FogReduction - 1, 0);
    }

    if (camPos.z > 200.0f) {
        if (set->m_fFarClip > 1000.0f) {
            if (camPos.z <= 500.0f) {
                float t_alt = (camPos.z - 200.0f) / 300.0f;
                set->m_fFarClip = lerp(set->m_fFarClip, 1000.0f, t_alt);
            } else {
                set->m_fFarClip = 1000.0f;
            }
        }
    }

    float horizon = lerp((float)GreyValuesDuringDay[currSampleIdx], (float)GreyValuesDuringDay[nextSampleIdx], timeT);
    m_BelowHorizonGrey.red   = (uint8)lerp(horizon, (float)m_CurrentColours.m_nSkyBottomRed, CWeather::UnderWaterness);
    m_BelowHorizonGrey.green = (uint8)lerp(horizon, (float)m_CurrentColours.m_nSkyBottomGreen, CWeather::UnderWaterness);
    m_BelowHorizonGrey.blue  = (uint8)lerp(horizon, (float)m_CurrentColours.m_nSkyBottomBlue, CWeather::UnderWaterness);

    set->m_fAmbientBeforeBrightnessRed   = set->m_fAmbientRed;
    set->m_fAmbientBeforeBrightnessGreen = set->m_fAmbientGreen;
    set->m_fAmbientBeforeBrightnessBlue  = set->m_fAmbientBlue;

    // 0x561468
    const auto brightness = (float)FrontEndMenuManager.m_PrefsBrightness;
    if (brightness >= 256.0f) {
        f = (brightness - 256.0f) / 128.0f + 1.0f;
        float max = std::max({ set->m_fAmbientRed, set->m_fAmbientGreen, set->m_fAmbientBlue });
        max = max * f - max;
        set->m_fAmbientRed   += max;
        set->m_fAmbientGreen += max;
        set->m_fAmbientBlue  += max;
    } else {
        f = brightness / 256.0f * 0.8f + 0.2f;
        set->m_fAmbientRed   *= f;
        set->m_fAmbientGreen *= f;
        set->m_fAmbientBlue  *= f;
    }

    if (f > 1.0f) {
        float r, g, b;
        f = (f - 1.0f) * 0.06f;
        float max = std::max({ set->m_fAmbientRed, set->m_fAmbientGreen, set->m_fAmbientBlue });
        r = set->m_fAmbientRed;
        g = set->m_fAmbientGreen;
        b = set->m_fAmbientBlue;
        if (max == 0.0f) {
            max = 0.001f;
            set->m_fAmbientRed   = 0.001f;
            set->m_fAmbientGreen = 0.001f;
            set->m_fAmbientBlue  = 0.001f;
        }
        if (f > max) {
            f /= max;
            set->m_fAmbientRed   *= f;
            set->m_fAmbientGreen *= f;
            set->m_fAmbientBlue  *= f;
        }
        m_BrightnessAddedToAmbientRed   = set->m_fAmbientRed   - r;
        m_BrightnessAddedToAmbientGreen = set->m_fAmbientGreen - g;
        m_BrightnessAddedToAmbientBlue  = set->m_fAmbientBlue  - b;
    }

    f = 0.0f;

    if (point.x < -3000.0f) { // todo: WORLD_BOUNDS
        f = -(point.x + 3000.0f);
    } else if (point.x > 3000.0f) {
        f = point.x - 3000.0f;
    }

    if (point.y < -3000.0f) {
        f += -(point.y + 3000.0f);
    } else if (point.y > 3000.0f) {
        f += point.y - 3000.0f;
    }

    if (f >= 1000.0f) {
        set->m_fLodDistMult *= 2.0f;
    } else if (f > 0.0f) {
        set->m_fLodDistMult *= (f / 1000.0f + 1.0f);
    }

    SetConstantParametersForPostFX();
}

// 0x5616E0
float CTimeCycle::FindFarClipForCoors(CVector cameraPos) {
    CColourSet set{};
    bool extraOn = m_bExtraColourOn != 0;
    float extraInter = m_ExtraColourInter;
    m_bExtraColourOn = 0;
    m_ExtraColourInter = 0.0f;
    CalcColoursForPoint(cameraPos, &set);
    m_bExtraColourOn = extraOn;
    m_ExtraColourInter = extraInter;
    return set.m_fFarClip;
}

// 0x55FFD0
void CTimeCycle::FindTimeCycleBox(
    CVector         pos,
    CTimeCycleBox** curr,
    float*          interpolation,
    bool            isLOD,
    bool            isFarClip,
    CTimeCycleBox*  ignored
) {
    *curr          = nullptr;
    *interpolation = 0.0f;

    for (auto& v : std::span{ m_aBoxes, m_NumBoxes }) {
        if (isLOD && v.LodDistMult == 32.f) { // 0x560013
            continue;
        }
        if (isFarClip && !v.FarClip) { // 0x560038
            continue;
        }
        if (ignored == &v) {
            continue;
        }

        // Check if the point is at least within the `FallOff`
        const auto CheckPointWithin = [&](int32 i, float tolerance) {
            return v.Box.m_vecMin[i] - tolerance > pos[i] || v.Box.m_vecMax[i] + tolerance < pos[i];
        };
        if (CheckPointWithin(0, v.Falloff) || CheckPointWithin(1, v.Falloff) || CheckPointWithin(2, v.Falloff / 3.f)) {
            continue;
        }

        // Calculate the distance to the box and interpolation
        const auto vdist = v.Box.GetShortestVectorDistToPt(pos); // 0x5600EC
        const auto dist  = CVector{ vdist.x, vdist.y, vdist.z * 3.f }.Magnitude(); // 0x560188
        if (dist > 0.f) { // Point not inside the box, but within `FallOff`
            const auto t = 1.f - dist / v.Falloff;
            if (t > *interpolation) {
                *curr          = &v;
                *interpolation = t;
            }
        } else { // Point inside the box
            *curr          = &v;
            *interpolation = 1.f;
        }
    }
}

// 0x560210
void CTimeCycle::SetConstantParametersForPostFX() {
    if (!CPostEffects::IsVisionFXActive())
        return;

    if (CPostEffects::m_bNightVision) {
        m_CurrentColours.m_nShadowStrength = 0;
        m_CurrentColours.m_nLightShadowStrength = 0;
        m_CurrentColours.m_nPoleShadowStrength = 0;
        m_CurrentColours.m_fAmbientRed = 0.0f;
        m_CurrentColours.m_fAmbientGreen = 0.4f;
        m_CurrentColours.m_fAmbientBlue = 0.0f;
        m_CurrentColours.m_fAmbientRed_Obj = 0.0f;
        m_CurrentColours.m_fAmbientGreen_Obj = 0.4f;
        m_CurrentColours.m_fAmbientBlue_Obj = 0.0f;
        m_CurrentColours.m_nSkyTopRed = 0;
        m_CurrentColours.m_nSkyTopGreen = 128;
        m_CurrentColours.m_nSkyTopBlue = 0;
        m_CurrentColours.m_nSkyBottomRed = 0;
        m_CurrentColours.m_nSkyBottomGreen = 128;
        m_CurrentColours.m_nSkyBottomBlue = 0;
    }

    if (CPostEffects::m_bInfraredVision) {
        m_CurrentColours.m_nShadowStrength = 0;
        m_CurrentColours.m_nLightShadowStrength = 0;
        m_CurrentColours.m_nPoleShadowStrength = 0;
        m_CurrentColours.m_fLightsOnGroundBrightness = 0.0f;
        m_CurrentColours.m_nHighLightMinIntensity = 0;
        m_CurrentColours.m_nWaterFogAlpha = 0;
        m_CurrentColours.m_fAmbientRed = 0.0f;
        m_CurrentColours.m_fAmbientGreen = 0.0f;
        m_CurrentColours.m_fAmbientBlue = 1.0f;
        m_CurrentColours.m_fAmbientRed_Obj = 0.0f;
        m_CurrentColours.m_fAmbientGreen_Obj = 0.0f;
        m_CurrentColours.m_fAmbientBlue_Obj = 1.0f;
        m_CurrentColours.m_nSkyTopRed = 0;
        m_CurrentColours.m_nSkyTopGreen = 0;
        m_CurrentColours.m_nSkyTopBlue = 128;
        m_CurrentColours.m_nSkyBottomRed = 0;
        m_CurrentColours.m_nSkyBottomGreen = 0;
        m_CurrentColours.m_nSkyBottomBlue = 128;
    }
}
