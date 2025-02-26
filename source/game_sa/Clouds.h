/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"
#include <rwplcore.h>

constexpr auto MAX_MOVING_FOG = 350;
constexpr auto MAX_VOLUMETRIC_CLOUDS = 180;

constexpr auto SKYP_WIDTH_MULT = 1.4f;
constexpr auto SKYP_CAM_DIST = 30.f;

constexpr auto SKYP_ABOVE_HORIZON_Z = 0.5f;
constexpr auto SKYP_HORIZON_Z = 0.0f;
constexpr auto SKYP_SEA_HORIZON_Z = -0.1f;
constexpr auto SKYP_BELOW_HORIZON_Z = -0.3f;

struct tMovingFog {
    bool    m_bFogSlots[MAX_MOVING_FOG];
    CVector m_vecPosn[MAX_MOVING_FOG];
    float   m_fSize[MAX_MOVING_FOG];
    float   m_fIntensity[MAX_MOVING_FOG];
    float   m_fMaxIntensity[MAX_MOVING_FOG];
    CVector m_vecWind;
    float   m_Speed[MAX_MOVING_FOG];
    int     field_27B4[12];
    std::array<uint32, 6> m_nPrimIndices;
};

struct tVolumetricClouds {
    bool       bUsed[MAX_VOLUMETRIC_CLOUDS];
    bool       bJustCreated[MAX_VOLUMETRIC_CLOUDS];

    CVector    pos[MAX_VOLUMETRIC_CLOUDS];
    CVector    size[MAX_VOLUMETRIC_CLOUDS];

    int32      alpha[MAX_VOLUMETRIC_CLOUDS];

    RwTexture* texture;

    CVector    quadNormal[3];

    float      modelX[18];
    float      modelY[18];
    float      modelZ[18];

    float      modelU[18];
    float      modelV[18];
};

class CClouds {
public:
    static inline auto& m_fVolumetricCloudDensity = StaticRef<float, 0x8D5388>();
    static inline auto& m_bVolumetricCloudHeightSwitch = StaticRef<int8, 0x8D538C>();
    static inline auto& m_fVolumetricCloudWindMoveFactor = StaticRef<float, 0x8D5390>();
    static float& m_fVolumetricCloudMaxDistance;
    static uint32& m_VolumetricCloudsUsedNum;
    static float& ms_cameraRoll;
    static int32& IndividualRotation;
    static float& CloudRotation;
    static tVolumetricClouds& ms_vc;
    static tMovingFog& ms_mf;
    static CVector& PlayerCoords;
    static CVector& CameraCoors;

    static inline struct DebugSettings {
        struct RenderSettingPair {
            bool Enabled = true, Force = false;
        } Moon, Rockstar, LowClouds, Rainbow, Streaks, VolumetricClouds;
    } s_DebugSettings;

public:
    static void InjectHooks();

    static void Init();
    static void Update();
    static void Shutdown();

    static void SetUpOneSkyPoly(CVector vert1pos, CVector vert2pos, CVector vert3pos, CVector vert4pos, uint8 topRed, uint8 topGreen, uint8 topBlue, uint8 bottomRed, uint8 bottomGreen, uint8 bottomBlue);

    static void MovingFogInit();
    static void MovingFog_Create(CVector* posn);
    static void MovingFog_Delete(int32 fogSlotIndex);
    static void MovingFog_Update();
    static void MovingFogRender();
    static void Render_MaybeRenderMoon(float colorBalance);
    static void Render_MaybeRenderRockstarLogo(float colorBalance);
    static void Render_RenderLowClouds(float colorBalance);
    static void Render_MaybeRenderRainbows();
    static void Render_MaybeRenderStreaks();
    static float MovingFog_GetFXIntensity();
    static CVector MovingFog_GetWind();
    static int32 MovingFog_GetFirstFreeSlot();

    static void VolumetricCloudsInit();
    static void VolumetricClouds_Create(CVector* posn);
    static void VolumetricClouds_Delete(int32 vcSlotIndex);
    static float VolumetricCloudsGetMaxDistance();
    static int32 VolumetricClouds_GetFirstFreeSlot();
    static void VolumetricCloudsRender();

    static void Render();
    static void RenderSkyPolys();
    static void RenderBottomFromHeight();
};

extern float& CurrentFogIntensity;               // default 1.0f
extern RwTexture*& gpMoonMask;
extern RwTexture*& gpCloudTex;
extern RwTexture*& gpCloudMaskTex;
extern float& flt_C6E954;
extern float& flt_C6E970;

extern int32& dword_C6E974;
