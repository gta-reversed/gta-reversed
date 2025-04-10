#include "StdInc.h"

#include "TimeCycle.h"
#include "PostEffects.h"
#include "Shadows.h"

static uint8 gTimecycleHours[]      = { 5, 6, 7, 12, 19, 20, 22, 24 };    // 0x8CDECD
static uint8 gTimecycleStartHours[] = { 30, 30, 30, 50, 60, 60, 50, 35 }; // 0x8CDED8
int &TunnelWeather = *(int*)0x8CDEE0; // 9 = WEATHER_FOGGY_SF, unchanged

void CTimeCycle::InjectHooks() {
    RH_ScopedClass(CTimeCycle);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BBAC0, { .reversed = false });
    RH_ScopedInstall(InitForRestart, 0x5601F0);
    RH_ScopedInstall(Shutdown, 0x5601E0);
    RH_ScopedInstall(Update, 0x561760);
    RH_ScopedInstall(StartExtraColour, 0x55FEC0);
    RH_ScopedInstall(StopExtraColour, 0x55FF20);
    RH_ScopedInstall(AddOne, 0x55FF40);
    RH_ScopedInstall(CalcColoursForPoint, 0x5603D0, { .reversed = false });
    RH_ScopedInstall(FindFarClipForCoors, 0x5616E0);
    RH_ScopedInstall(FindTimeCycleBox, 0x55FFD0, { .reversed = false });
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
    return plugin::Call<0x5BBAC0>(); // looks differ

    CFileMgr::SetDir("DATA");
    auto file = CFileMgr::OpenFile("TIMECYC.DAT", "rb");
    CFileMgr::SetDir("");

    if (!file) { // NOTSA
        DEV_LOG("[CTimeCycle] Failed to open TIMECYC.DAT");
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
    float postFx1a, postFx1R, postFx1G, postFx1B;
    float postFx2a, postFx2R, postFx2G, postFx2B;
    float cloudAlpha;
    int32 highLightMinIntensity;
    int32 waterFogAlpha;
    float dirMult;

    char* line;
    for (auto w = 0; w < NUM_WEATHERS; w++) {
        for (auto h = 0; h < NUM_HOURS; h++) {
            while (line = CFileLoader::LoadLine(file), line) {
                if (line[0] != '/' && line[0] != '\0') {
                    break;
                }
            }

            VERIFY(sscanf_s(line,
                "%d %d %d  %d %d %d  %d %d %d  %d %d %d  %d %d %d" "%d %d %d  %d %d %d"
                "%f %f %f %d %d %d %f %f %f"
                "%d %d %d  %d %d %d  %f %f %f  %f %f %f  %f %f %f" "%f %f %f"
                "%f %d %d %f",
                &ambR,       &ambG,       &ambB,
                &ambObjR,    &ambObjG,    &ambObjB,
                &dirR,       &dirG,       &dirB,
                &skyTopR,    &skyTopG,    &skyTopB,
                &skyBotR,    &skyBotG,    &skyBotB,
                &sunCoreR,   &sunCoreG,   &sunCoreB,
                &sunCoronaR, &sunCoronaG, &sunCoronaB,
                &sunSize, &spriteSize, &spriteBrightness, &shadowStrength, &lightShadowStrength, &poleShadowStrength, &farClip, &fogStart, &lightOnGround,
                &lowCloudR, &lowCloudG, &lowCloudB,
                &bottomCloudR, &bottomCloudG, &bottomCloudB,
                &waterR, &waterG, &waterB, &waterA,
                &postFx1a, &postFx1R, &postFx1G, &postFx1B,
                &postFx2a, &postFx2R, &postFx2G, &postFx2B,
                &cloudAlpha, &highLightMinIntensity, &waterFogAlpha, &dirMult
            ) == 52);

            m_nAmbientRed[h][w]   = ambR;
            m_nAmbientGreen[h][w] = ambG;
            m_nAmbientBlue[h][w]  = ambB;

            m_nAmbientRed_Obj[h][w]   = ambObjR;
            m_nAmbientGreen_Obj[h][w] = ambObjG;
            m_nAmbientBlue_Obj[h][w]  = ambObjB;

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

            m_fSunSize[h][w]    = uint8(sunSize * 10.0f + 0.5f);
            m_fSpriteSize[h][w] = uint8(spriteSize * 10.0f + 0.5f);
            m_fSpriteBrightness[h][w]    = uint8(spriteBrightness * 10.0f + 0.5f);
            m_nShadowStrength[h][w]      = shadowStrength;
            m_nLightShadowStrength[h][w] = lightShadowStrength;
            m_nPoleShadowStrength[h][w]  = poleShadowStrength;
            m_fFarClip[h][w]  = (uint16)farClip;
            m_fFogStart[h][w] = (uint16)fogStart;
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

            m_fPostFx1Alpha[h][w] = (uint8)sq(postFx1a);
            m_fPostFx2Alpha[h][w] = (uint8)sq(postFx2a);

            m_fCloudAlpha[h][w]            = (uint8)cloudAlpha;
            m_nHighLightMinIntensity[h][w] = (uint8)highLightMinIntensity;
            m_nWaterFogAlpha[h][w]         = (uint8)waterFogAlpha;
            m_nDirectionalMult[h][w]       = uint8(dirMult * 100.0f);
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
    m_aBoxes[m_NumBoxes].m_Box = box;
    m_aBoxes[m_NumBoxes].m_FarClip = farClip;
    m_aBoxes[m_NumBoxes].m_ExtraColor = m_ExtraColor;
    m_aBoxes[m_NumBoxes].m_Strength = strength / 100.0f;
    m_aBoxes[m_NumBoxes].m_Falloff = falloff;
    m_aBoxes[m_NumBoxes].m_LodDistMult = (uint8)(std::min(lodDistMult, 4.0f) * 32.0f);
    m_NumBoxes++;
}

// 0x5603D0
void CTimeCycle::CalcColoursForPoint(CVector point, CColourSet* set) {
    return plugin::Call<0x5603D0, CVector, CColourSet*>(point, set);

    // untested
    CTimeCycleBox *lodBox, *farBox1, *farBox2, *weatherBox;
    float lodBoxInterp, farBox1Interp, farBox2Interp, weatherBoxInterp;
    FindTimeCycleBox(point, &lodBox, &lodBoxInterp, true, false, nullptr);
    FindTimeCycleBox(point, &farBox1, &farBox1Interp, false, true, nullptr);

    if (farBox1) {
        FindTimeCycleBox(point, &farBox2, &farBox2Interp, false, true, farBox1);
        if (farBox2 && farBox2->m_Box.GetWidth() > farBox1->m_Box.GetWidth()) {
            std::swap(farBox1, farBox2);
            std::swap(farBox1Interp, farBox2Interp);
        }
    } else {
        farBox2 = nullptr;
    }
    FindTimeCycleBox(point, &weatherBox, &weatherBoxInterp, false, false, nullptr);

    float time = (float)CClock::GetGameClockMinutes() / 60.0f + (float)CClock::GetGameClockSeconds() / 3600.0f + (float)CClock::GetGameClockHours();
    time = std::min(time, 24.0f); // 23.999f ?

    int curHourSel, nextHourSel;
    for (curHourSel = 0; time >= (float)gTimecycleHours[curHourSel + 1]; curHourSel++) {
        ;
    }

    nextHourSel = (curHourSel + 1) % NUM_HOURS;
    auto curHour = gTimecycleHours[curHourSel];
    auto nextHour = gTimecycleHours[curHourSel + 1];

    float timeInterp       = (time - (float)curHour) / (float)(nextHour - curHour);
    float invTimeInterp    = 1.0f - timeInterp;
    float weatherInterp    = CWeather::InterpolationValue;
    float invWeatherInterp = 1.0f - weatherInterp;

    int boxHour, boxWeather;
    if (weatherBox) {
        boxHour = weatherBox->m_ExtraColor % 8;
        boxWeather = (weatherBox->m_ExtraColor / NUM_HOURS) + 21;
    }

    CColourSet currentOld(curHourSel, CWeather::OldWeatherType);
    CColourSet nextOld(nextHourSel,   CWeather::OldWeatherType);
    CColourSet currentNew(curHourSel, CWeather::NewWeatherType);
    CColourSet nextNew(nextHourSel,   CWeather::NewWeatherType);

    auto& camPos = TheCamera.GetPosition();
    float f = std::clamp((camPos.z - 20.0f) / 200.0f, 0.0f, 1.0f);
    if (CWeather::OldWeatherType == WEATHER_EXTRASUNNY_SMOG_LA) {
        CColourSet set1(curHourSel, WEATHER_EXTRASUNNY_LA);
        currentOld.Interpolate(&currentOld, &set1, 1.0f - f, f, false);
        CColourSet set2(nextHourSel, WEATHER_EXTRASUNNY_LA);
        nextOld.Interpolate(&nextOld, &set2, 1.0f - f, f, false);
    } else if (CWeather::OldWeatherType == WEATHER_SUNNY_SMOG_LA) {
        CColourSet set1(curHourSel, WEATHER_SUNNY_LA);
        currentOld.Interpolate(&currentOld, &set1, 1.0f - f, f, false);
        CColourSet set2(nextHourSel, WEATHER_SUNNY_LA);
        nextOld.Interpolate(&nextOld, &set2, 1.0f - f, f, false);
    }
    if (CWeather::NewWeatherType == WEATHER_EXTRASUNNY_SMOG_LA) {
        CColourSet set1(curHourSel, WEATHER_EXTRASUNNY_LA);
        currentNew.Interpolate(&currentNew, &set1, 1.0f - f, f, false);
        CColourSet set2(nextHourSel, WEATHER_EXTRASUNNY_LA);
        nextNew.Interpolate(&nextNew, &set2, 1.0f - f, f, false);
    } else if (CWeather::NewWeatherType == WEATHER_SUNNY_SMOG_LA) {
        CColourSet set1(curHourSel, WEATHER_SUNNY_LA);
        currentNew.Interpolate(&currentNew, &set1, 1.0f - f, f, false);
        CColourSet set2(nextHourSel, WEATHER_SUNNY_LA);
        nextNew.Interpolate(&nextNew, &set2, 1.0f - f, f, false);
    }

    CColourSet oldInterp{}, newInterp{};
    oldInterp.Interpolate(&currentOld, &nextOld, invTimeInterp, timeInterp, false);
    newInterp.Interpolate(&currentNew, &nextNew, invTimeInterp, timeInterp, false);
    set->Interpolate(&oldInterp, &newInterp, invWeatherInterp, weatherInterp, false);

    float lightMult = (1.0f / CCoronas::LightsMult + 3.0f) * 0.25f;
    set->m_nSkyTopRed      = std::min(uint16(float(set->m_nSkyTopRed)      * lightMult), uint16(255));
    set->m_nSkyTopGreen    = std::min(uint16(float(set->m_nSkyTopGreen)    * lightMult), uint16(255));
    set->m_nSkyTopBlue     = std::min(uint16(float(set->m_nSkyTopBlue)     * lightMult), uint16(255));
    set->m_nSkyBottomRed   = std::min(uint16(float(set->m_nSkyBottomRed)   * lightMult), uint16(255));
    set->m_nSkyBottomGreen = std::min(uint16(float(set->m_nSkyBottomGreen) * lightMult), uint16(255));
    set->m_nSkyBottomBlue  = std::min(uint16(float(set->m_nSkyBottomBlue)  * lightMult), uint16(255));

    if (m_FogReduction) {
        set->m_fFarClip = set->m_fFarClip > float(m_FogReduction) * 10.15625f ? set->m_fFarClip : float(m_FogReduction) * 10.15625f;
    }

    m_CurrentStoredValue = (m_CurrentStoredValue + 1) & 15;
    CVector* vec = &m_VectorToSun[m_CurrentStoredValue];
    float sunAngle = CClock::GetMinutesToday() * PI / 720.0f;
    vec->x = +0.7f + std::sin(sunAngle);
    vec->y = -0.7f;
    vec->z = +0.2f - std::cos(sunAngle);
    vec->Normalise();

    // 0x560AAE
    if (weatherBox && weatherBox->m_ExtraColor >= 0) {
        float boxf = weatherBoxInterp * weatherBox->m_Strength;
        float invboxf = 1.0f - boxf;

        set->m_nSkyTopRed      = uint16((float)set->m_nSkyTopRed   * invboxf + (float)m_nSkyTopRed[boxHour][boxWeather] * boxf);
        set->m_nSkyTopGreen    = uint16((float)set->m_nSkyTopGreen * invboxf + (float)m_nSkyTopGreen[boxHour][boxWeather] * boxf);
        set->m_nSkyTopBlue     = uint16((float)set->m_nSkyTopBlue  * invboxf + (float)m_nSkyTopBlue[boxHour][boxWeather] * boxf);

        set->m_nSkyBottomRed   = uint16((float)set->m_nSkyBottomRed   * invboxf + (float)m_nSkyBottomRed[boxHour][boxWeather] * boxf);
        set->m_nSkyBottomGreen = uint16((float)set->m_nSkyBottomGreen * invboxf + (float)m_nSkyBottomGreen[boxHour][boxWeather] * boxf);
        set->m_nSkyBottomBlue  = uint16((float)set->m_nSkyBottomBlue  * invboxf + (float)m_nSkyBottomBlue[boxHour][boxWeather] * boxf);

        set->m_fWaterRed   *= invboxf + (float)m_fWaterRed[boxHour][boxWeather] * boxf;
        set->m_fWaterGreen *= invboxf + (float)m_fWaterGreen[boxHour][boxWeather] * boxf;
        set->m_fWaterBlue  *= invboxf + (float)m_fWaterBlue[boxHour][boxWeather] * boxf;
        set->m_fWaterAlpha *= invboxf + (float)m_fWaterAlpha[boxHour][boxWeather] * boxf;

        set->m_fAmbientRed   *= invboxf + (float)m_nAmbientRed[boxHour][boxWeather] * boxf;
        set->m_fAmbientGreen *= invboxf + (float)m_nAmbientGreen[boxHour][boxWeather] * boxf;
        set->m_fAmbientBlue  *= invboxf + (float)m_nAmbientBlue[boxHour][boxWeather] * boxf;

        set->m_fAmbientRed_Obj   *= invboxf + (float)m_nAmbientRed_Obj[boxHour][boxWeather] * boxf;
        set->m_fAmbientGreen_Obj *= invboxf + (float)m_nAmbientGreen_Obj[boxHour][boxWeather] * boxf;
        set->m_fAmbientBlue_Obj  *= invboxf + (float)m_nAmbientBlue_Obj[boxHour][boxWeather] * boxf;

        if ((float)m_fFarClip[boxHour][boxWeather] < set->m_fFarClip) {
            set->m_fFarClip = set->m_fFarClip * invboxf + (float)m_fFarClip[boxHour][boxWeather] * boxf;
        }

        set->m_fFogStart *= invboxf + (float)m_fFogStart[boxHour][boxWeather] * boxf;

        set->m_fPostFx1Red   *= invboxf + (float)m_fPostFx1Red[boxHour][boxWeather] * boxf;
        set->m_fPostFx1Green *= invboxf + (float)m_fPostFx1Green[boxHour][boxWeather] * boxf;
        set->m_fPostFx1Blue  *= invboxf + (float)m_fPostFx1Blue[boxHour][boxWeather] * boxf;
        set->m_fPostFx1Alpha *= invboxf + (float)m_fPostFx1Alpha[boxHour][boxWeather] * boxf;

        set->m_fPostFx2Red   *= invboxf + (float)m_fPostFx2Red[boxHour][boxWeather] * boxf;
        set->m_fPostFx2Green *= invboxf + (float)m_fPostFx2Green[boxHour][boxWeather] * boxf;
        set->m_fPostFx2Blue  *= invboxf + (float)m_fPostFx2Blue[boxHour][boxWeather] * boxf;
        set->m_fPostFx2Alpha *= invboxf + (float)m_fPostFx2Alpha[boxHour][boxWeather] * boxf;
    }

    if (lodBox) {
        set->m_fLodDistMult *= (1.0f - lodBoxInterp) + (float)lodBox->m_LodDistMult / 32.0f * lodBoxInterp;
    }

    if (farBox1 && (float)farBox1->m_FarClip < set->m_fFarClip) {
        set->m_fFarClip = set->m_fFarClip * (1.0f - farBox1Interp) + (float)farBox1->m_FarClip * farBox1Interp;
    }
    if (farBox2 && (float)farBox2->m_FarClip < set->m_fFarClip) {
        set->m_fFarClip = set->m_fFarClip * (1.0f - farBox2Interp) + (float)farBox2->m_FarClip * farBox2Interp;
    }

    float inc = CTimer::GetTimeStep() / 120.0f;
    if (m_bExtraColourOn) {
        m_ExtraColourInter += inc;
        m_ExtraColourInter = std::min(m_ExtraColourInter, 1.0f);
    } else {
        m_ExtraColourInter -= inc;
        m_ExtraColourInter = std::max(m_ExtraColourInter, 0.0f);
    }

    if (m_ExtraColourInter > 0.0f) {
        CColourSet extra(m_ExtraColour, m_ExtraColourWeatherType);
        bool ignoreSky = ShouldIgnoreSky();
        set->Interpolate(set, &extra, 1.0f - m_ExtraColourInter, m_ExtraColourInter, ignoreSky);
    }

    if (CWeather::UnderWaterness > 0.0f) {
        CColourSet current(curHourSel, 20);
        CColourSet next(nextHourSel, 20);
        CColourSet tmp{};
        tmp.Interpolate(&current, &next, invTimeInterp, timeInterp, false);
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
        m_fShadowDisplacementX[m_CurrentStoredValue], m_fShadowDisplacementY[m_CurrentStoredValue],
        m_fShadowFrontX[m_CurrentStoredValue], m_fShadowFrontY[m_CurrentStoredValue],
        m_fShadowSideX[m_CurrentStoredValue],  m_fShadowSideY[m_CurrentStoredValue]
    );

    if (TheCamera.m_mCameraMatrix.GetForward().z < -0.9f
        || !CWeather::bScriptsForceRain
        && (CCullZones::PlayerNoRain() || CCullZones::CamNoRain() || CCutsceneMgr::ms_running)
    ) {
        m_FogReduction++;
        if (m_FogReduction > 64)
            m_FogReduction = 64;
    } else {
        m_FogReduction--;
        if (m_FogReduction < 0)
            m_FogReduction = 0;
    }

    if (camPos.z > 200.0f) {
        if (camPos.z <= 500.0f)
        {
            set->m_fFarClip = set->m_fFarClip * (1.0f - (camPos.z - 200.0f) / 300.0f) + 1000.0f * (camPos.z - 200.0f) / 300.0f;
        }
        else if (set->m_fFarClip >= 1000.0f)
        {
            set->m_fFarClip = 1000.0f;
        }
    }

    float horizon = (float)gTimecycleStartHours[curHourSel] * invTimeInterp + (float)gTimecycleStartHours[nextHourSel] * timeInterp;
    m_BelowHorizonGrey.red   = uint8((float)m_CurrentColours.m_nSkyBottomRed   * CWeather::UnderWaterness + horizon * (1.0f - CWeather::UnderWaterness));
    m_BelowHorizonGrey.green = uint8((float)m_CurrentColours.m_nSkyBottomGreen * CWeather::UnderWaterness + horizon * (1.0f - CWeather::UnderWaterness));
    m_BelowHorizonGrey.blue  = uint8((float)m_CurrentColours.m_nSkyBottomBlue  * CWeather::UnderWaterness + horizon * (1.0f - CWeather::UnderWaterness));

    set->m_fAmbientBeforeBrightnessRed   = set->m_fAmbientRed;
    set->m_fAmbientBeforeBrightnessGreen = set->m_fAmbientGreen;
    set->m_fAmbientBeforeBrightnessBlue  = set->m_fAmbientBlue;

    const auto brightness = (float)FrontEndMenuManager.m_PrefsBrightness;
    if (brightness >= 256.0f) {
        f = (brightness - 256.0f) / 128.0f + 1.0f;
        float max = set->m_fAmbientRed;
        max = std::max(max, set->m_fAmbientGreen);
        max = std::max(max, set->m_fAmbientBlue);
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
        float max = set->m_fAmbientRed;
        max = std::max(max, set->m_fAmbientGreen);
        max = std::max(max, set->m_fAmbientBlue);
        r = set->m_fAmbientRed;
        g = set->m_fAmbientGreen;
        b = set->m_fAmbientBlue;
        if (max == 0.0f) {
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
void CTimeCycle::FindTimeCycleBox(CVector pos, CTimeCycleBox** outBox, float* interpolation, bool bCheckLod, bool bCheckFar, CTimeCycleBox* exclude) {
    return plugin::Call<0x55FFD0, CVector, CTimeCycleBox**, float*, bool, bool, CTimeCycleBox*>(pos, outBox, interpolation, bCheckLod, bCheckFar, exclude);

    *outBox = nullptr;
    *interpolation = 0.0f;

    for (auto& box : std::span{ m_aBoxes, m_NumBoxes }) {

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
