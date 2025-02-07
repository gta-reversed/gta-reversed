#pragma once

#include "ColourSet.h"
#include "eWeatherType.h"

class CTimeCycleBox;
class CBox;

class CTimeCycleBox {
public:
    CBox  m_Box;
    int16 m_FarClip;
    uint8 m_LodDistMult;
    int32 m_ExtraColor;
    float m_Strength;
    float m_Falloff;
};
VALIDATE_SIZE(CTimeCycleBox, 0x28);

static inline float& gfLaRiotsLightMult = *(float*)0x8CD060; // 1.0f

class CTimeCycle {
public:
    static constexpr auto NUM_HOURS = 8;

    static inline CVector& m_vecDirnLightToSun = *(CVector*)0xB7CB14;
    static inline RwRGBA& m_BelowHorizonGrey = *(RwRGBA*)0xB7CB10;
    static inline CVector (&m_VectorToSun)[16] = *(CVector(*)[16])0xB7CA50;

    static inline CTimeCycleBox (&m_aBoxes)[20] = *(CTimeCycleBox(*)[20])0xB7C550;
    static inline uint32& m_NumBoxes = *(uint32*)0xB7C480;

    static inline uint32& m_bExtraColourOn = *(uint32*)0xB7C484;
    static inline CColourSet& m_CurrentColours = *(CColourSet*)0xB7C4A0;

    static inline float& m_fCurrentRGB1Red = *(float*)0xB7C518;
    static inline float& m_fCurrentRGB1Green = *(float*)0xB7C51C;
    static inline float& m_fCurrentRGB1Blue = *(float*)0xB7C520;

    static inline float& m_fCurrentRGB2Red = *(float*)0xB7C528;
    static inline float& m_fCurrentRGB2Green = *(float*)0xB7C52C;
    static inline float& m_fCurrentRGB2Blue = *(float*)0xB7C530;

    static inline uint8 (&m_nAmbientRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C3C8;
    static inline uint8 (&m_nAmbientGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C310;
    static inline uint8 (&m_nAmbientBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C258;

    static inline uint8 (&m_nAmbientRed_Obj)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C1A0;
    static inline uint8 (&m_nAmbientGreen_Obj)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C0E8;
    static inline uint8 (&m_nAmbientBlue_Obj)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7C030;

    static inline uint8 (&m_nSkyTopRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BF78;
    static inline uint8 (&m_nSkyTopGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BEC0;
    static inline uint8 (&m_nSkyTopBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BE08;

    static inline uint8 (&m_nSkyBottomRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BD50;
    static inline uint8 (&m_nSkyBottomGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BC98;
    static inline uint8 (&m_nSkyBottomBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BBE0;

    static inline uint8 (&m_fSunSize)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B6D8;

    static inline uint8 (&m_nSunCoronaRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B900;
    static inline uint8 (&m_nSunCoronaGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B848;
    static inline uint8 (&m_nSunCoronaBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B790;

    static inline uint8 (&m_nSunCoreRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BB28;
    static inline uint8 (&m_nSunCoreGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7BA70;
    static inline uint8 (&m_nSunCoreBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B9B8;

    static inline uint16 (&m_fFarClip)[NUM_HOURS][NUM_WEATHERS] = *(uint16(*)[NUM_HOURS][NUM_WEATHERS])0xB7B1D0;
    static inline uint16 (&m_fFogStart)[NUM_HOURS][NUM_WEATHERS] = *(uint16(*)[NUM_HOURS][NUM_WEATHERS])0xB7B060;
    static inline uint8 (&m_fLightsOnGroundBrightness)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AFA8;

    static inline uint8 (&m_nShadowStrength)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B4B0;
    static inline uint8 (&m_nLightShadowStrength)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B3F8;
    static inline uint8 (&m_nPoleShadowStrength)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B340;

    static inline uint8 (&m_fSpriteSize)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B620;
    static inline uint8 (&m_fSpriteBrightness)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7B568;

    static inline uint8 (&m_nLowCloudsRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AEF0;
    static inline uint8 (&m_nLowCloudsGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AE38;
    static inline uint8 (&m_nLowCloudsBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AD80;

    static inline uint8 (&m_nFluffyCloudsBottomRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7ACC8;
    static inline uint8 (&m_nFluffyCloudsBottomGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AC10;
    static inline uint8 (&m_nFluffyCloudsBottomBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AB58;

    static inline uint8 (&m_fWaterRed)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7AAA0;
    static inline uint8 (&m_fWaterGreen)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A9E8;
    static inline uint8 (&m_fWaterBlue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A930;
    static inline uint8 (&m_fWaterAlpha)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A878;

    static inline uint8 (&m_fPostFx1Red)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A7C0;
    static inline uint8 (&m_fPostFx1Green)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A708;
    static inline uint8 (&m_fPostFx1Blue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A650;
    static inline uint8 (&m_fPostFx1Alpha)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A598;

    static inline uint8 (&m_fPostFx2Red)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A4E0;
    static inline uint8 (&m_fPostFx2Green)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A428;
    static inline uint8 (&m_fPostFx2Blue)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A370;
    static inline uint8 (&m_fPostFx2Alpha)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A2B8;

    static inline uint8 (&m_fCloudAlpha)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A200;
    static inline uint8 (&m_nHighLightMinIntensity)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A148;
    static inline uint8 (&m_nWaterFogAlpha)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB7A090;
    static inline uint8 (&m_nDirectionalMult)[NUM_HOURS][NUM_WEATHERS] = *(uint8(*)[NUM_HOURS][NUM_WEATHERS])0xB79FD8;

    static inline int32& m_CurrentStoredValue = *(int32*)0xB79FD0;

    static inline float (&m_fShadowFrontX)[16] = *(float (*)[16])0xB79F90;
    static inline float (&m_fShadowFrontY)[16] = *(float (*)[16])0xB79F50;

    static inline float (&m_fShadowSideX)[16] = *(float (*)[16])0xB79F10;
    static inline float (&m_fShadowSideY)[16] = *(float (*)[16])0xB79ED0;

    static inline float (&m_fShadowDisplacementX)[16] = *(float (*)[16])0xB79E90;
    static inline float (&m_fShadowDisplacementY)[16] = *(float (*)[16])0xB79E50;

    static inline int32& m_FogReduction = *(int32*)0xB79E48;

    static inline int32& m_ExtraColour = *(int32*)0xB79E44;
    static inline int32& m_ExtraColourWeatherType = *(int32*)0xB79E40;
    static inline float& m_ExtraColourInter = *(float*)0xB79E3C;

    static inline float& m_BrightnessAddedToAmbientRed = *(float*)0xB79E38;
    static inline float& m_BrightnessAddedToAmbientGreen = *(float*)0xB79E34;
    static inline float& m_BrightnessAddedToAmbientBlue = *(float*)0xB79E30;

public:
    static void InjectHooks();

    static void Initialise();
    static void InitForRestart();
    static void Shutdown();

    static void Update();

    static void StartExtraColour(int32 colour, bool bNoExtraColorInterior);
    static void StopExtraColour(bool bNoExtraColorInterior);

    static void AddOne(CBox& box, int16 farClip, int32 extraColor, float strength, float falloff, float lodDistMult);
    static void CalcColoursForPoint(CVector point, CColourSet* set);
    static float FindFarClipForCoors(CVector cameraPos);
    static void FindTimeCycleBox(CVector pos, CTimeCycleBox** outBox, float* interpolation, bool bCheckLod, bool bCheckFar, CTimeCycleBox* exclude);
    static void SetConstantParametersForPostFX();

    static float GetAmbientRed()                    { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientRed; }       // 0x560330
    static float GetAmbientGreen()                  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientGreen; }     // 0x560340
    static float GetAmbientBlue()                   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBlue; }      // 0x560350
    static float GetAmbientRed_BeforeBrightness()   { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessRed; }   // 0x560390
    static float GetAmbientGreen_BeforeBrightness() { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessGreen; } // 0x5603A0
    static float GetAmbientBlue_BeforeBrightness()  { return gfLaRiotsLightMult * m_CurrentColours.m_fAmbientBeforeBrightnessBlue; }  // 0x5603B0
    static float GetAmbientRed_Obj()                { return m_CurrentColours.m_fAmbientRed_Obj; }                        // 0x560360
    static float GetAmbientGreen_Obj()              { return m_CurrentColours.m_fAmbientGreen_Obj; }                      // 0x560370
    static float GetAmbientBlue_Obj()               { return m_CurrentColours.m_fAmbientBlue_Obj; }                       // 0x560380

    static auto GetVectorToSun() { return m_VectorToSun[m_CurrentStoredValue]; }
    static auto GetShadowSide() { return CVector2D{ m_fShadowSideX[m_CurrentStoredValue], m_fShadowSideY[m_CurrentStoredValue] }; }

public: // NOTSA
    static CRGBA GetCurrentSkyBottomColor()         { return m_CurrentColours.GetSkyBottom(); }

    static auto GetPostFxColors() { return std::make_pair(m_CurrentColours.GetPostFx1(), m_CurrentColours.GetPostFx2()); }

    static float SumOfCurrentRGB1() { return m_fCurrentRGB1Blue + m_fCurrentRGB1Green + m_fCurrentRGB1Red; }
    static float SumOfCurrentRGB2() { return m_fCurrentRGB2Blue + m_fCurrentRGB2Green + m_fCurrentRGB2Red; }

    static auto GetBoxes() { return std::span{ m_aBoxes, m_NumBoxes}; }
    static bool ShouldIgnoreSky() {
        return (
            m_nSkyTopRed[m_ExtraColour][m_ExtraColourWeatherType]   == 0 &&
            m_nSkyTopGreen[m_ExtraColour][m_ExtraColourWeatherType] == 0 &&
            m_nSkyTopBlue[m_ExtraColour][m_ExtraColourWeatherType]  == 0
        );
    }
};
