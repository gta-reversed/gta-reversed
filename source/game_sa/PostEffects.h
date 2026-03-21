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

    MAX_HEAT_HAZE_TYPES
};

class CPostEffects {
public:
    static void InjectHooks();

    static void Initialise();
    static void Close();
    static void DoScreenModeDependentInitializations();
    static void SetupBackBufferVertex();
    static void Update();

    // X2,Y2 is added to X1,Y1. So they are more like width and height in a rectangle.
    static void DrawQuad(float x1, float y1, float x2, float y2, uint8 red, uint8 green, uint8 blue, uint8 alpha, RwRaster* raster);
    static void DrawQuadSetDefaultUVs();
    static void DrawQuadSetPixelUVs(float u0, float v0, float u1, float v1, float u3, float v3, float u2, float v2);
    static void DrawQuadSetUVs(float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4);

    static void FilterFX_RestoreDayNightBalance();
    static void FilterFX_StoreAndSetDayNightBalance();

    static void ImmediateModeFilterStuffInitialize();
    static void ImmediateModeRenderStatesSet();
    static void ImmediateModeRenderStatesStore();
    static void ImmediateModeRenderStatesReStore();

    static RwRaster* RasterCreatePostEffects(RwRect rect);

    static void ScriptCCTVSwitch(bool enable);
    static void ScriptDarknessFilterSwitch(bool enable, int32 alpha);
    static void ScriptHeatHazeFXSwitch(bool enable);
    static void ScriptInfraredVisionSwitch(bool enable);
    static void ScriptNightVisionSwitch(bool enable);
    static void ScriptResetForEffects();

    static void UnderWaterRipple(CRGBA color, float xoffset, float yoffset, float strength, float speed, float freq);
    static void UnderWaterRippleFadeToFX();

    static void HeatHazeFXInit();
    static void HeatHazeFX(float fIntensity, bool bAlphaMaskMode);

    static bool IsVisionFXActive();

    static void NightVision();
    static void NightVisionSetLights();

    static void SetFilterMainColour(RwRaster* raster, RwRGBA color);
    static void InfraredVision(RwRGBA color, RwRGBA colorMain);
    static void InfraredVisionSetLightsForDefaultObjects();
    static void InfraredVisionSetLightsForHeatObjects();
    static void InfraredVisionStoreAndSetLightsForHeatObjects(CPed* ped);
    static void InfraredVisionRestoreLightsForHeatObjects();

    static void Fog();
    static void CCTV();
    static void Grain(int32 strengthMask, bool update);
    static void SpeedFX(float speed);
    static void DarknessFilter(int32 alpha);
    static void ColourFilter(RwRGBA pass1, RwRGBA pass2);
    static void Radiosity(int32 intensityLimit, int32 filterPasses, int32 renderPasses, int32 intensity);
    static void SetSpeedFXManualSpeedCurrentFrame(float value);

    static void Render();
#if defined(BUILD_ANDROID)
    static void MobileRender();
#endif

public:
    static inline auto& SCREEN_EXTRA_MULT_CHANGE_RATE = StaticRef<float>(0x8D5168); // 0.0005f;
    static inline auto& SCREEN_EXTRA_MULT_BASE_CAP = StaticRef<float>(0x8D516C); // 0.35f;
    static inline auto& SCREEN_EXTRA_MULT_BASE_MULT = StaticRef<float>(0x8D5170); // 1.0f;

    static inline auto& m_bDisableAllPostEffect = StaticRef<bool>(0xC402CF);
    static inline auto& m_bSavePhotoFromScript = StaticRef<bool>(0xC402D0);
    static inline auto& m_bInCutscene = StaticRef<bool>(0xC402B7);

    static inline auto& m_xoffset = StaticRef<float>(0x8D5130); // 4.0f
    static inline auto& m_yoffset = StaticRef<float>(0x8D5134); // 24.0f

    static inline auto& m_colour1Multiplier = StaticRef<float>(0x8D5160); // 1.0f;
    static inline auto& m_colour2Multiplier = StaticRef<float>(0x8D5164); // 1.0f;
    static inline auto& m_colourLeftUOffset = StaticRef<float>(0x8D5150); // 8
    static inline auto& m_colourRightUOffset = StaticRef<float>(0x8D5154); // 8
    static inline auto& m_colourTopVOffset = StaticRef<float>(0x8D5158); // 8
    static inline auto& m_colourBottomVOffset = StaticRef<float>(0x8D515C); // 8

    static inline auto& m_bNightVision = StaticRef<bool>(0xC402B8);
    static inline auto& m_fNightVisionSwitchOnFXCount = StaticRef<float>(0xC40300); // = CPostEffects::m_fNightVisionSwitchOnFXTime
    static inline auto& m_fNightVisionSwitchOnFXTime = StaticRef<float>(0x8D50B0); // 50.0f
    static inline auto& m_NightVisionGrainStrength = StaticRef<int32>(0x8D50A8); // 48
    static inline auto& m_NightVisionMainCol = StaticRef<CRGBA>(0x8D50AC); // { 255, 0, 130, 0};

    static inline auto& m_bDarknessFilter = StaticRef<bool>(0xC402C4);
    static inline auto& m_DarknessFilterAlpha = StaticRef<int32>(0x8D5204); // 170
    static inline auto& m_DarknessFilterAlphaDefault = StaticRef<int32>(0x8D50F4); // 170
    static inline auto& m_DarknessFilterRadiosityIntensityLimit = StaticRef<int32>(0x8D50F8); // 45

    static inline auto& m_fWaterFXStartUnderWaterness = StaticRef<float>(0x8D514C); // 0.535f;
    static inline auto& m_fWaterFullDarknessDepth = StaticRef<float>(0x8D5148); // 90.0f
    static inline auto& m_bWaterDepthDarkness = StaticRef<bool>(0x8D5144);  // true;

    static inline auto& m_bHeatHazeFX = StaticRef<bool>(0xC402BA);
    static inline auto& m_HeatHazeFXSpeedMin = StaticRef<int32>(0x8D50EC); // 6
    static inline auto& m_HeatHazeFXSpeedMax = StaticRef<int32>(0x8D50F0); // 10
    static inline auto& m_HeatHazeFXIntensity = StaticRef<int32>(0x8D50E8); // 150
    static inline auto& m_HeatHazeFXType = StaticRef<int32>(0xC402BC); // 0
    static inline auto& m_HeatHazeFXTypeLast = StaticRef<int32>(0x8D50E4); // -1
    static inline auto& m_HeatHazeFXRandomShift = StaticRef<int32>(0xC402C0);
    static inline auto& m_HeatHazeFXScanSizeX = StaticRef<int32>(0xC40304); // int32(SCREEN_WIDTH_UNIT  * 24.0f);
    static inline auto& m_HeatHazeFXScanSizeY = StaticRef<int32>(0xC40308); // int32(SCREEN_HEIGHT_UNIT * 24.0f);
    static inline auto& m_HeatHazeFXRenderSizeX = StaticRef<int32>(0xC4030C); // int32(SCREEN_WIDTH_UNIT  * 24.0f);
    static inline auto& m_HeatHazeFXRenderSizeY = StaticRef<int32>(0xC40310); // int32(SCREEN_HEIGHT_UNIT * 24.0f);

    static inline auto& m_bFog = StaticRef<bool>(0xC402C6);

    static inline auto& m_bSpeedFX = StaticRef<bool>(0x8D5100); // true;
    static inline auto& m_bSpeedFXTestMode = StaticRef<bool>(0xC402C7);
    static inline auto& m_bSpeedFXUserFlag = StaticRef<bool>(0x8D5108); // always true;
    static inline auto& m_bSpeedFXUserFlagCurrentFrame = StaticRef<bool>(0x8D5109); // true
    static inline auto& m_fSpeedFXManualSpeedCurrentFrame = StaticRef<float>(0xC402C8);
    static inline auto& m_SpeedFXAlpha = StaticRef<int32>(0x8D5104); // 36

    static inline auto& pRasterFrontBuffer = StaticRef<RwRaster*>(0xC402D8);

    static inline auto& m_bGrainEnable = StaticRef<bool>(0xC402B4);
    static inline auto& m_pGrainRaster = StaticRef<RwRaster*>(0xC402B0);
    static inline auto& m_grainStrength = StaticRef<char[2]>(0x8D5094);

    static inline auto& m_bCCTV = StaticRef<bool>(0xC402C5);
    static inline auto& m_CCTVcol = StaticRef<CRGBA>(0x8D50FC); // { 64, 0, 0, 0 }

    static inline auto& m_bRainEnable = StaticRef<bool>(0xC402D1);
    static inline auto& m_bColorEnable = StaticRef<bool>(0x8D518C); // true;

    static inline auto& m_bRadiosity = StaticRef<bool>(0xC402CC);
    static inline auto& m_bRadiosityDebug = StaticRef<bool>(0xC402CD);
    static inline auto& m_bRadiosityLinearFilter = StaticRef<bool>(0x8D510A); // true;
    static inline auto& m_bRadiosityStripCopyMode = StaticRef<bool>(0x8D510B); // true;
    static inline auto& m_RadiosityFilterUCorrection = StaticRef<int32>(0x8D511C); // 2;
    static inline auto& m_RadiosityFilterVCorrection = StaticRef<int32>(0x8D5120); // 2;
    static inline auto& m_RadiosityIntensity = StaticRef<int32>(0x8D5118); // 35
    static inline auto& m_RadiosityIntensityLimit = StaticRef<int32>(0x8D5114); // 220
    static inline auto& m_bRadiosityBypassTimeCycleIntensityLimit = StaticRef<bool>(0xC402CE);
    static inline auto& m_RadiosityPixelsX = StaticRef<float>(0xC40314);  // SCREEN_WIDTH
    static inline auto& m_RadiosityPixelsY = StaticRef<float>(0xC40318);  // SCREEN_HEIGHT
    static inline auto& m_RadiosityFilterPasses = StaticRef<uint32>(0x8D5110); // 1
    static inline auto& m_RadiosityRenderPasses = StaticRef<uint32>(0x8D510C); // 2

    static inline auto& m_VisionFXDayNightBalance = StaticRef<float>(0x8D50A4); // 1.0f

    static inline auto& m_bInfraredVision = StaticRef<bool>(0xC402B9);
    static inline auto& m_InfraredVisionGrainStrength = StaticRef<int32>(0x8D50B4); // 64
    static inline auto& m_fInfraredVisionFilterRadius = StaticRef<float>(0x8D50B8); // 0.003f
    // float& m_fInfraredVisionSwitchOnFXCount;
    static inline auto& m_InfraredVisionCol = StaticRef<CRGBA>(0x8D50CC);      // { FF, 3C, 28, 6E }
    static inline auto& m_InfraredVisionMainCol = StaticRef<CRGBA>(0x8D50D0);      // { FF, C8, 00, 64 }
    static inline auto& m_fInfraredVisionHeatObjectCol = StaticRef<RwRGBAReal>(0x8D50BC); // { 1.0f, 0.0f, 0.0f, 1.0f }
    static inline auto& m_HeatHazeFXHourOfDayStart = StaticRef<int32>(0x8D50D4); // 10
    static inline auto& m_HeatHazeFXHourOfDayEnd = StaticRef<int32>(0x8D50D8); // 19
    static inline auto& m_fHeatHazeFXFadeSpeed = StaticRef<float>(0x8D50DC); // 0.05f
    static inline auto& m_fHeatHazeFXInsideBuildingFadeSpeed = StaticRef<float>(0x8D50DC); // 0.5f

    static inline auto& m_waterEnable = StaticRef<bool>(0xC402D3);
    static inline auto& m_waterStrength = StaticRef<float>(0x8D512C); // 64
    static inline auto& m_waterSpeed = StaticRef<float>(0x8D5138); // 0.0015f
    static inline auto& m_waterFreq = StaticRef<float>(0x8D513C); // 0.04f
    static inline auto& m_waterCol = StaticRef<CRGBA>(0x8D5140); // { 64, 64, 64, 64 }

    // Immediate Mode Filter
    struct imf {
        float                       screenZ;
        float                       recipCameraZ;
        RwRaster*                   RasterDrawBuffer;
        int32                       sizeDrawBufferX;
        int32                       sizeDrawBufferY;
        float                       fFrontBufferU1;
        float                       fFrontBufferV1;
        float                       fFrontBufferU2;
        float                       fFrontBufferV2;
        std::array<RwIm2DVertex, 3> triangle;
        float                       uMinTri;
        float                       uMaxTri;
        float                       vMinTri;
        float                       vMaxTri;
        std::array<RwIm2DVertex, 6> quad;
        RwBlendFunction             blendSrc;
        RwBlendFunction             blendDst;
        RwBool                      bFog;
        RwCullMode                  cullMode;
        RwBool                      bZTest;
        RwBool                      bZWrite;
        RwShadeMode                 shadeMode;
        RwBool                      bVertexAlpha;
        RwTextureAddressMode        textureAddress;
        RwTextureFilterMode         textureFilter;
    };
    VALIDATE_SIZE(imf, 0x158);

    static inline auto& ms_imf = StaticRef<imf>(0xC40150);
};
