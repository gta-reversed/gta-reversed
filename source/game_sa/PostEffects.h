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

public:
    static inline float& SCREEN_EXTRA_MULT_CHANGE_RATE = *(float*)0x8D5168; // 0.0005f;
    static inline float& SCREEN_EXTRA_MULT_BASE_CAP    = *(float*)0x8D516C; // 0.35f;
    static inline float& SCREEN_EXTRA_MULT_BASE_MULT   = *(float*)0x8D5170; // 1.0f;

    static inline bool& m_bDisableAllPostEffect = *(bool*)0xC402CF;
    static inline bool& m_bSavePhotoFromScript  = *(bool*)0xC402D0;
    static inline bool& m_bInCutscene           = *(bool*)0xC402B7;

    static inline float& m_xoffset = *(float*)0x8D5130; // 4.0f
    static inline float& m_yoffset = *(float*)0x8D5134; // 24.0f

    static inline float& m_colour1Multiplier   = *(float*)0x8D5160; // 1.0f;
    static inline float& m_colour2Multiplier   = *(float*)0x8D5164; // 1.0f;
    static inline float& m_colourLeftUOffset   = *(float*)0x8D5150; // 8
    static inline float& m_colourRightUOffset  = *(float*)0x8D5154; // 8
    static inline float& m_colourTopVOffset    = *(float*)0x8D5158; // 8
    static inline float& m_colourBottomVOffset = *(float*)0x8D515C; // 8

    static inline bool&  m_bNightVision                = *(bool*)0xC402B8;
    static inline float& m_fNightVisionSwitchOnFXCount = *(float*)0xC40300; // = CPostEffects::m_fNightVisionSwitchOnFXTime
    static inline float& m_fNightVisionSwitchOnFXTime  = *(float*)0x8D50B0; // 50.0f
    static inline int32& m_NightVisionGrainStrength    = *(int32*)0x8D50A8; // 48
    static inline CRGBA& m_NightVisionMainCol          = *(CRGBA*)0x8D50AC; // { 255, 0, 130, 0};

    static inline bool&  m_bDarknessFilter                       = *(bool*)0xC402C4;
    static inline int32& m_DarknessFilterAlpha                   = *(int32*)0x8D5204; // 170
    static inline int32& m_DarknessFilterAlphaDefault            = *(int32*)0x8D50F4; // 170
    static inline int32& m_DarknessFilterRadiosityIntensityLimit = *(int32*)0x8D50F8; // 45

    static inline float& m_fWaterFXStartUnderWaterness = *(float*)0x8D514C; // 0.535f;
    static inline float& m_fWaterFullDarknessDepth     = *(float*)0x8D5148; // 90.0f
    static inline bool&  m_bWaterDepthDarkness         = *(bool*)0x8D5144;  // true;

    static inline bool&  m_bHeatHazeFX           = *(bool*)0xC402BA;
    static inline int32& m_HeatHazeFXSpeedMin    = *(int32*)0x8D50EC; // 6
    static inline int32& m_HeatHazeFXSpeedMax    = *(int32*)0x8D50F0; // 10
    static inline int32& m_HeatHazeFXIntensity   = *(int32*)0x8D50E8; // 150
    static inline int32& m_HeatHazeFXType        = *(int32*)0xC402BC; // 0
    static inline int32& m_HeatHazeFXTypeLast    = *(int32*)0x8D50E4; // -1
    static inline int32& m_HeatHazeFXRandomShift = *(int32*)0xC402C0;
    static inline int32& m_HeatHazeFXScanSizeX   = *(int32*)0xC40304; // int32(SCREEN_WIDTH_UNIT  * 24.0f);
    static inline int32& m_HeatHazeFXScanSizeY   = *(int32*)0xC40308; // int32(SCREEN_HEIGHT_UNIT * 24.0f);
    static inline int32& m_HeatHazeFXRenderSizeX = *(int32*)0xC4030C; // int32(SCREEN_WIDTH_UNIT  * 24.0f);
    static inline int32& m_HeatHazeFXRenderSizeY = *(int32*)0xC40310; // int32(SCREEN_HEIGHT_UNIT * 24.0f);

    static inline bool& m_bFog = *(bool*)0xC402C6;

    static inline bool&  m_bSpeedFX                        = *(bool*)0x8D5100; // true;
    static inline bool&  m_bSpeedFXTestMode                = *(bool*)0xC402C7;
    static inline bool&  m_bSpeedFXUserFlag                = *(bool*)0x8D5108; // always true;
    static inline bool&  m_bSpeedFXUserFlagCurrentFrame    = *(bool*)0x8D5109; // true
    static inline float& m_fSpeedFXManualSpeedCurrentFrame = *(float*)0xC402C8;
    static inline int32& m_SpeedFXAlpha                    = *(int32*)0x8D5104; // 36

    static inline RwRaster*& pRasterFrontBuffer = *(RwRaster**)0xC402D8;

    static inline bool&      m_bGrainEnable  = *(bool*)0xC402B4;
    static inline RwRaster*& m_pGrainRaster  = *(RwRaster**)0xC402B0;
    static inline char (&m_grainStrength)[2] = *(char (*)[2])0x8D5094;

    static inline bool&  m_bCCTV   = *(bool*)0xC402C5;
    static inline CRGBA& m_CCTVcol = *(CRGBA*)0x8D50FC; // { 0, 0, 0, 64 }

    static inline bool& m_bRainEnable  = *(bool*)0xC402D1;
    static inline bool& m_bColorEnable = *(bool*)0x8D518C; // true;

    static inline bool& m_bRadiosity = *(bool*)0xC402CC;
    static inline bool&  m_bRadiosityDebug = *(bool*)0xC402CD;
    static inline bool&  m_bRadiosityLinearFilter = *(bool*)0x8D510A; // true;
    static inline bool&  m_bRadiosityStripCopyMode = *(bool*)0x8D510B; // true;
    static inline int32&  m_RadiosityFilterUCorrection = *(int32*)0x8D511C; // 2;
    static inline int32&  m_RadiosityFilterVCorrection = *(int32*)0x8D5120; // 2;
    static inline int32&  m_RadiosityIntensity                      = *(int32*)0x8D5118; // 35
    static inline int32&  m_RadiosityIntensityLimit                 = *(int32*)0x8D5114; // 220
    static inline bool&   m_bRadiosityBypassTimeCycleIntensityLimit = *(bool*)0xC402CE;
    static inline float&  m_RadiosityPixelsX                        = *(float*)0xC40314;  // SCREEN_WIDTH
    static inline float&  m_RadiosityPixelsY                        = *(float*)0xC40318;  // SCREEN_HEIGHT
    static inline uint32& m_RadiosityFilterPasses                   = *(uint32*)0x8D5110; // 1
    static inline uint32& m_RadiosityRenderPasses                   = *(uint32*)0x8D510C; // 2

    static inline float& m_VisionFXDayNightBalance = *(float*)0x8D50A4; // 1.0f

    static inline bool&  m_bInfraredVision             = *(bool*)0xC402B9;
    static inline int32& m_InfraredVisionGrainStrength = *(int32*)0x8D50B4; // 64
    static inline float& m_fInfraredVisionFilterRadius = *(float*)0x8D50B8; // 0.003f
    // float& m_fInfraredVisionSwitchOnFXCount;
    static inline CRGBA&      m_InfraredVisionCol            = *(CRGBA*)0x8D50CC;      // { FF, 3C, 28, 6E }
    static inline CRGBA&      m_InfraredVisionMainCol        = *(CRGBA*)0x8D50D0;      // { FF, C8, 00, 64 }
    static inline RwRGBAReal& m_fInfraredVisionHeatObjectCol = *(RwRGBAReal*)0x8D50BC; // { 1.0f, 0.0f, 0.0f, 1.0f }
    static inline int32& m_HeatHazeFXHourOfDayStart = *(int32*)0x8D50D4; // 10
    static inline int32& m_HeatHazeFXHourOfDayEnd   = *(int32*)0x8D50D8; // 19
    static inline float& m_fHeatHazeFXFadeSpeed = *(float*)0x8D50DC; // 0.05f
    static inline float& m_fHeatHazeFXInsideBuildingFadeSpeed = *(float*)0x8D50DC; // 0.5f

    static inline bool&  m_waterEnable   = *(bool*)0xC402D3;
    static inline float& m_waterStrength = *(float*)0x8D512C; // 64
    static inline float& m_waterSpeed    = *(float*)0x8D5138; // 0.0015f
    static inline float& m_waterFreq     = *(float*)0x8D513C; // 0.04f
    static inline CRGBA& m_waterCol      = *(CRGBA*)0x8D5140; // { 64, 64, 64, 64 }

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

    static inline imf& ms_imf = *(imf*)0xC40150;
};
