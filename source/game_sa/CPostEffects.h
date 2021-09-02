#pragma once

#include "rwcore.h"

class CRGBA;

enum eHeatHazeFXType {
    HEAT_HAZE_UNDEFINED = -1,
    HEAT_HAZE_0,
    HEAT_HAZE_1,
    HEAT_HAZE_2,
    HEAT_HAZE_3,
    HEAT_HAZE_4,
};

class CPostEffects {
public:
    static float& SCREEN_EXTRA_MULT_BASE_CAP;
    static float& SCREEN_EXTRA_MULT_BASE_MULT;
    static float& SCREEN_EXTRA_MULT_CHANGE_RATE;

    static bool& m_bDisableAllPostEffect;
    static bool& m_bInCutscene;

    static float& m_xoffset;
    static float& m_yoffset;

    static int32& m_defScreenXPosn;
    static int32& m_defScreenYPosn;

    static float& ms_imf;

    static RwRaster*& pRasterFrontBuffer;

    static int32& m_DarknessFilterAlpha;
    static int32& m_DarknessFilterAlphaDefault;
    static int32& m_DarknessFilterRadiosityIntensityLimit;
    static bool& m_bDarknessFilter;

    static bool& m_bCCTV;
    static CRGBA& m_CCTVcol;

    static bool& m_bColorEnable;

    static bool& m_bFog;

    static bool& m_bGrainEnable;
    static RwRaster*& m_pGrainRaster;
    static char (&m_grainStrength)[2];

    static bool& m_bHilightEnable;

    static bool& m_bRadiosity;
    static bool& m_bRadiosityDebug;
    static bool& m_bRadiosityLinearFilter;
    static bool& m_bRadiosityStripCopyMode;
    //static m_RadiosityFilterUCorrection;
    //static m_RadiosityFilterVCorrection;
    static int32& m_RadiosityIntensity;
    static int32& m_RadiosityIntensityLimit;
    static bool& m_bRadiosityBypassTimeCycleIntensityLimit;
    static float& m_RadiosityPixelsX;
    static float& m_RadiosityPixelsY;
    static uint32& m_RadiosityFilterPasses;
    static uint32& m_RadiosityRenderPasses;

    static bool& m_bRainEnable;

    static bool& m_bSavePhotoFromScript;

    static bool& m_bSeamRemover;
    //static m_SeamRemoverMode;
    static bool& m_bSeamRemoverDebugMode;
    static bool& m_bSeamRemoverSeamSearchMode;
    //static m_SeamRemoverShiftBottomRight;
    //static m_SeamRemoverShiftTopLeft;

    static bool& m_bSpeedFX;
    static bool& m_bSpeedFXTestMode;
    static bool& m_bSpeedFXUserFlag;
    static bool& m_bSpeedFXUserFlagCurrentFrame;
    static float& m_fSpeedFXManualSpeedCurrentFrame;
    static int32& m_SpeedFXAlpha;

    static float& m_colour1Multiplier;
    static float& m_colour2Multiplier;
    static float& m_colourLeftUOffset;
    static float& m_colourTopVOffset;
    static float& m_colourRightUOffset;
    static float& m_colourBottomVOffset;

    static float& m_VisionFXDayNightBalance;

    static bool& m_bHeatHazeFX;
    static int32& m_HeatHazeFXSpeedMin;
    static int32& m_HeatHazeFXSpeedMax;
    static int32& m_HeatHazeFXIntensity;
    static int32& m_HeatHazeFXType;
    static int32& m_HeatHazeFXTypeLast;
    static int32& m_HeatHazeFXRandomShift;
    static bool& m_bHeatHazeMaskModeTest;
    static float& m_fHeatHazeFXFadeSpeed;
    static float& m_fHeatHazeFXInsideBuildingFadeSpeed;
    static float& m_HeatHazeFXHourOfDayEnd;
    static float& m_HeatHazeFXHourOfDayStart;
    static float& m_HeatHazeFXRenderSizeX;
    static float& m_HeatHazeFXRenderSizeY;
    static float& m_HeatHazeFXScanSizeX;
    static float& m_HeatHazeFXScanSizeY;

    static bool& m_bInfraredVision;
    static float& m_fInfraredVisionFilterRadius;
    static float& m_fInfraredVisionSwitchOnFXCount;
    static int32& m_InfraredVisionGrainStrength;
    static RwRGBA& m_InfraredVisionCol;
    static RwRGBA& m_InfraredVisionMainCol;
    static RwRGBAReal& m_fInfraredVisionHeatObjectCol;

    static bool& m_bNightVision;
    static float& m_fNightVisionSwitchOnFXCount;
    static float& m_fNightVisionSwitchOnFXTime;
    static int32& m_NightVisionGrainStrength;
    static RwRGBA& m_NightVisionMainCol;

    static bool& m_bWaterDepthDarkness;
    static float& m_fWaterFXStartUnderWaterness;
    static float& m_fWaterFullDarknessDepth;

    static bool& m_waterEnable;
    static float& m_waterStrength;
    static float& m_waterSpeed;
    static float& m_waterFreq;
    static RwRGBA& m_waterCol;

public:
    static void InjectHooks();

    static void Initialise();
    static void Close();
    static void DoScreenModeDependentInitializations();
    static void SetupBackBufferVertex();
    static void Update();

    static void DrawQuad(float x1, float y1, float x2, float y2, char red, uint32 green, uint32 blue, uint8 alpha, RwRaster* raster);
    static void DrawQuadSetDefaultUVs();
    static void DrawQuadSetPixelUVs(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8);
    static void DrawQuadSetUVs(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8);

    static void FilterFX_RestoreDayNightBalance();
    static void FilterFX_StoreAndSetDayNightBalance();

    static void ImmediateModeFilterStuffInitialize();
    static void ImmediateModeRenderStatesSet();
    static void ImmediateModeRenderStatesStore();
    static void ImmediateModeRenderStatesReStore();

    static void RasterCreatePostEffects(RwRect rect);

    static void ScriptCCTVSwitch(bool enable);
    static void ScriptDarknessFilterSwitch(bool enable, int32 alpha);
    static void ScriptHeatHazeFXSwitch(bool enable);
    static void ScriptInfraredVisionSwitch(bool enable);
    static void ScriptNightVisionSwitch(bool enable);
    static void ScriptResetForEffects();

    static void UnderWaterRipple(RwRGBA a1, float a2, float a3, int32 a4, float a5, float a6);
    static void UnderWaterRippleFadeToFX();

    static void HeatHazeFXInit();
    static void HeatHazeFX(float a1, bool a2);

    static bool IsVisionFXActive();

    static void NightVision();
    static void NightVisionSetLights();

    static void SetFilterMainColour(RwRGBA a1, RwRGBA a2);
    static void InfraredVision(RwRGBA color, RwRGBA colorMain);
    static void InfraredVisionSetLightsForDefaultObjects();
    static void InfraredVisionSetLightsForHeatObjects();
    static void InfraredVisionStoreAndSetLightsForHeatObjects(CPed* ped);
    static void InfraredVisionRestoreLightsForHeatObjects();

    static void Fog();
    static void CCTV();
    static void Grain(int32 strength, bool a2);
    static void SpeedFX(float speed);
    static void DarknessFilter(int32 a1);
    static void ColourFilter(int32 pass1, int32 pass2);
    static void Radiosity(int32 a1, int32 a2, int32 a3, int32 a4);

    static void Render();
};

/*
m_hilightMblur
m_hilightOffset
m_hilightScale
m_hilightStrength

m_pDmaPkt

m_smokeyDistance
m_smokeyEnable
m_smokeyStrength
*/
