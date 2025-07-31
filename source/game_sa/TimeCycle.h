#pragma once

#include "ColourSet.h"
#include "eWeatherType.h"

class CTimeCycleBox;
class CBox;

class CTimeCycleBox {
public:
    CBox  Box;
    int16 FarClip;
    uint8 LodDistMult;
    int32 ExtraColor;
    float Strength;
    float Falloff;
};
VALIDATE_SIZE(CTimeCycleBox, 0x28);

static inline float& gfLaRiotsLightMult = *(float*)0x8CD060; // 1.0f

enum eTimeType {
    TIME_MIDNIGHT,
    TIME_5AM,
    TIME_6AM,
    TIME_7AM,
    TIME_MIDDAY, // TIME_NOON
    TIME_7PM,
    TIME_8PM,
    TIME_10PM,

    NUM_HOURS
};

class CTimeCycle {
public:
    template<typename T>
    using Colors = notsa::mdarray<T, NUM_HOURS, NUM_WEATHERS>;

    static inline auto& m_nAmbientRed = StaticRef<Colors<uint8>>(0xB7C3C8);
    static inline auto& m_nAmbientGreen = StaticRef<Colors<uint8>>(0xB7C310);
    static inline auto& m_nAmbientBlue = StaticRef<Colors<uint8>>(0xB7C258);

    static inline auto& m_nAmbientRed_Obj = StaticRef<Colors<uint8>>(0xB7C1A0);
    static inline auto& m_nAmbientGreen_Obj = StaticRef<Colors<uint8>>(0xB7C0E8);
    static inline auto& m_nAmbientBlue_Obj = StaticRef<Colors<uint8>>(0xB7C030);

    static inline auto& m_nSkyTopRed = StaticRef<Colors<uint8>>(0xB7BF78);
    static inline auto& m_nSkyTopGreen = StaticRef<Colors<uint8>>(0xB7BEC0);
    static inline auto& m_nSkyTopBlue = StaticRef<Colors<uint8>>(0xB7BE08);

    static inline auto& m_nSkyBottomRed = StaticRef<Colors<uint8>>(0xB7BD50);
    static inline auto& m_nSkyBottomGreen = StaticRef<Colors<uint8>>(0xB7BC98);
    static inline auto& m_nSkyBottomBlue = StaticRef<Colors<uint8>>(0xB7BBE0);

    static inline auto& m_nSunCoreRed = StaticRef<Colors<uint8>>(0xB7BB28);
    static inline auto& m_nSunCoreGreen = StaticRef<Colors<uint8>>(0xB7BA70);
    static inline auto& m_nSunCoreBlue = StaticRef<Colors<uint8>>(0xB7B9B8);

    static inline auto& m_nSunCoronaRed = StaticRef<Colors<uint8>>(0xB7B900);
    static inline auto& m_nSunCoronaGreen = StaticRef<Colors<uint8>>(0xB7B848);
    static inline auto& m_nSunCoronaBlue = StaticRef<Colors<uint8>>(0xB7B790);

    static inline auto& m_fSunSize = StaticRef<Colors<int8>>(0xB7B6D8);

    static inline auto& m_fSpriteSize = StaticRef<Colors<int8>>(0xB7B620);
    static inline auto& m_fSpriteBrightness = StaticRef<Colors<int8>>(0xB7B568);

    static inline auto& m_nShadowStrength = StaticRef<Colors<uint8>>(0xB7B4B0);
    static inline auto& m_nLightShadowStrength = StaticRef<Colors<uint8>>(0xB7B3F8);
    static inline auto& m_nPoleShadowStrength = StaticRef<Colors<uint8>>(0xB7B340);

    static inline auto& m_fFarClip = StaticRef<Colors<int16>>(0xB7B1D0);
    static inline auto& m_fFogStart = StaticRef<Colors<int16>>(0xB7B060);
    static inline auto& m_fLightsOnGroundBrightness = StaticRef<Colors<uint8>>(0xB7AFA8);

    static inline auto& m_nLowCloudsRed = StaticRef<Colors<uint8>>(0xB7AEF0);
    static inline auto& m_nLowCloudsGreen = StaticRef<Colors<uint8>>(0xB7AE38);
    static inline auto& m_nLowCloudsBlue = StaticRef<Colors<uint8>>(0xB7AD80);

    static inline auto& m_nFluffyCloudsBottomRed = StaticRef<Colors<uint8>>(0xB7ACC8);
    static inline auto& m_nFluffyCloudsBottomGreen = StaticRef<Colors<uint8>>(0xB7AC10);
    static inline auto& m_nFluffyCloudsBottomBlue = StaticRef<Colors<uint8>>(0xB7AB58);

    static inline auto& m_fWaterRed = StaticRef<Colors<uint8>>(0xB7AAA0);
    static inline auto& m_fWaterGreen = StaticRef<Colors<uint8>>(0xB7A9E8);
    static inline auto& m_fWaterBlue = StaticRef<Colors<uint8>>(0xB7A930);
    static inline auto& m_fWaterAlpha = StaticRef<Colors<uint8>>(0xB7A878);

    static inline auto& m_fPostFx1Red = StaticRef<Colors<uint8>>(0xB7A7C0);
    static inline auto& m_fPostFx1Green = StaticRef<Colors<uint8>>(0xB7A708);
    static inline auto& m_fPostFx1Blue = StaticRef<Colors<uint8>>(0xB7A650);
    static inline auto& m_fPostFx1Alpha = StaticRef<Colors<uint8>>(0xB7A598);

    static inline auto& m_fPostFx2Red = StaticRef<Colors<uint8>>(0xB7A4E0);
    static inline auto& m_fPostFx2Green = StaticRef<Colors<uint8>>(0xB7A428);
    static inline auto& m_fPostFx2Blue = StaticRef<Colors<uint8>>(0xB7A370);
    static inline auto& m_fPostFx2Alpha = StaticRef<Colors<uint8>>(0xB7A2B8);

    static inline auto& m_fCloudAlpha = StaticRef<Colors<uint8>>(0xB7A200);
    static inline auto& m_nHighLightMinIntensity = StaticRef<Colors<uint8>>(0xB7A148);
    static inline auto& m_nWaterFogAlpha = StaticRef<Colors<uint8>>(0xB7A090);
    static inline auto& m_nDirectionalMult = StaticRef<Colors<uint8>>(0xB79FD8);

    static inline CColourSet& m_CurrentColours = *(CColourSet*)0xB7C4A0;

    static inline CTimeCycleBox (&m_aBoxes)[20] = *(CTimeCycleBox(*)[20])0xB7C550;
    static inline uint32& m_NumBoxes = *(uint32*)0xB7C480;

    static inline uint32& m_CurrentStoredValue = *(uint32*)0xB79FD0;
    static inline CVector (&m_VectorToSun)[16] = *(CVector(*)[16])0xB7CA50;

    // TODO: CVector2D?
    static inline float (&m_fShadowFrontX)[16] = *(float(*)[16])0xB79F90;
    static inline float (&m_fShadowFrontY)[16] = *(float(*)[16])0xB79F50;
    static inline float (&m_fShadowSideX)[16] = *(float(*)[16])0xB79F10;
    static inline float (&m_fShadowSideY)[16] = *(float(*)[16])0xB79ED0;
    static inline float (&m_fShadowDisplacementX)[16] = *(float(*)[16])0xB79E90;
    static inline float (&m_fShadowDisplacementY)[16] = *(float(*)[16])0xB79E50;

    static inline int32& m_FogReduction = *(int32*)0xB79E48;

    static inline int32& m_ExtraColour = *(int32*)0xB79E44;
    static inline int32& m_ExtraColourWeatherType = *(int32*)0xB79E40;
    static inline uint32& m_bExtraColourOn = *(uint32*)0xB7C484;
    static inline float& m_ExtraColourInter = *(float*)0xB79E3C;

    static inline RwRGBA& m_BelowHorizonGrey = *(RwRGBA*)0xB7CB10;

    static inline float& m_BrightnessAddedToAmbientRed = *(float*)0xB79E38;
    static inline float& m_BrightnessAddedToAmbientGreen = *(float*)0xB79E34;
    static inline float& m_BrightnessAddedToAmbientBlue = *(float*)0xB79E30;

    static inline CVector& m_vecDirnLightToSun = *(CVector*)0xB7CB14;

public:
    static void InjectHooks();

    static void Initialise(bool padFile);
    static void Update();
    static void CalcColoursForPoint(CVector point, CColourSet* set);
    static float FindFarClipForCoors(CVector cameraPos);
    static void Shutdown();


    static float GetAmbientRed();
    static float GetAmbientGreen();
    static float GetAmbientBlue();

    static float GetAmbientRed_Obj();
    static float GetAmbientGreen_Obj();
    static float GetAmbientBlue_Obj();

    static float GetAmbientRed_BeforeBrightness();
    static float GetAmbientGreen_BeforeBrightness();
    static float GetAmbientBlue_BeforeBrightness();

    static uint16 GetSkyTopRed() { return m_CurrentColours.m_nSkyTopRed; };
    static uint16 GetSkyTopGreen() { return m_CurrentColours.m_nSkyTopGreen; };
    static uint16 GetSkyTopBlue() { return m_CurrentColours.m_nSkyTopBlue; };

    static uint16 GetSkyBottomRed()   { return m_CurrentColours.m_nSkyBottomRed; };
    static uint16 GetSkyBottomGreen() { return m_CurrentColours.m_nSkyBottomGreen; };
    static uint16 GetSkyBottomBlue()  { return m_CurrentColours.m_nSkyBottomBlue; }

    static uint16 GetFogRed()   { return m_CurrentColours.m_nSkyBottomRed; }
    static uint16 GetFogGreen() { return m_CurrentColours.m_nSkyBottomGreen; }
    static uint16 GetFogBlue()  { return m_CurrentColours.m_nSkyBottomBlue; }

    static uint16 GetSunCoreRed()   { return m_CurrentColours.m_nSunCoreRed; };
    static uint16 GetSunCoreGreen() { return m_CurrentColours.m_nSunCoreGreen; };
    static uint16 GetSunCoreBlue()  { return m_CurrentColours.m_nSunCoreBlue; }

    static uint16 GetSunCoronaRed()   { return m_CurrentColours.m_nSunCoronaRed; }
    static uint16 GetSunCoronaGreen() { return m_CurrentColours.m_nSunCoronaGreen; }
    static uint16 GetSunCoronaBlue()  { return m_CurrentColours.m_nSunCoronaBlue; }

    static float GetSunSize() { return m_CurrentColours.m_fSunSize; }
    static float GetSpriteSize() { return m_CurrentColours.m_fSpriteSize; }
    static float GetSpriteBrightness() { return m_CurrentColours.m_fSpriteBrightness; }

    static int16 GetShadowStrength() { return m_CurrentColours.m_nShadowStrength; }
    static int16 GetLightShadowStrength() { return m_CurrentColours.m_nLightShadowStrength; }
    static int16 GetPoleShadowStrength() { return m_CurrentColours.m_nPoleShadowStrength; }

    static float GetFarClip() { return m_CurrentColours.m_fFarClip; }
    static float GetFogStart() { return m_CurrentColours.m_fFogStart; }
    static float GetLightsOnGroundBrightness() { return m_CurrentColours.m_fLightsOnGroundBrightness; }

    static uint16 GetLowCloudsRed()   { return m_CurrentColours.m_nLowCloudsRed; }
    static uint16 GetLowCloudsGreen() { return m_CurrentColours.m_nLowCloudsGreen; }
    static uint16 GetLowCloudsBlue()  { return m_CurrentColours.m_nLowCloudsBlue; }

    static uint16 GetFluffyCloudsBottomRed()   { return m_CurrentColours.m_nFluffyCloudsBottomRed; }
    static uint16 GetFluffyCloudsBottomGreen() { return m_CurrentColours.m_nFluffyCloudsBottomGreen; }
    static uint16 GetFluffyCloudsBottomBlue()  { return m_CurrentColours.m_nFluffyCloudsBottomBlue; }

    static float GetWaterRed()   { return m_CurrentColours.m_fWaterRed; }
    static float GetWaterGreen() { return m_CurrentColours.m_fWaterGreen; }
    static float GetWaterBlue()  { return m_CurrentColours.m_fWaterBlue; }
    static float GetWaterAlpha() { return m_CurrentColours.m_fWaterAlpha; }

    static float GetPostFx1Red()  { return m_CurrentColours.m_fPostFx1Red; }
    static float GetPostFx1Green() { return m_CurrentColours.m_fPostFx1Green; }
    static float GetPostFx1Blue()  { return m_CurrentColours.m_fPostFx1Blue; }
    static float GetPostFx1Alpha() { return m_CurrentColours.m_fPostFx1Alpha; }

    static float GetPostFx2Red()   { return m_CurrentColours.m_fPostFx2Red; }
    static float GetPostFx2Green() { return m_CurrentColours.m_fPostFx2Green; }
    static float GetPostFx2Blue()  { return m_CurrentColours.m_fPostFx2Blue; }
    static float GetPostFx2Alpha() { return m_CurrentColours.m_fPostFx2Alpha; }

    static float GetCloudAlpha() { return m_CurrentColours.m_fCloudAlpha; }
    static int32 GetHighLightMinIntensity() { return m_CurrentColours.m_nHighLightMinIntensity; }


    static float GetLODMultiplier() { return m_CurrentColours.m_fLodDistMult; }

    static void SetFogRed(uint16 value) { m_CurrentColours.m_nSkyBottomRed = value; }
    static void SetFogGreen(uint16 value) { m_CurrentColours.m_nSkyBottomGreen = value; }
    static void SetFogBlue(uint16 value) { m_CurrentColours.m_nSkyBottomBlue = value; }
    

    static void StartExtraColour(int32 colour, bool bNoExtraColorInterior);
    static void StopExtraColour(bool bNoExtraColorInterior);


    static void AddOne(CBox& box, int16 farClip, int32 extraColor, float strength, float falloff, float lodDistMult);
    static void FindTimeCycleBox(CVector pos, CTimeCycleBox** out, float* currentBox, bool isLOD, bool isFarClip, CTimeCycleBox* alreadyFoundBox);
    static void InitForRestart();


    static void SetConstantParametersForPostFX();

public: // NOTSA
    static auto GetVectorToSun() { return m_VectorToSun[m_CurrentStoredValue]; }
    static auto GetShadowSide() { return CVector2D{ m_fShadowSideX[m_CurrentStoredValue], m_fShadowSideY[m_CurrentStoredValue] }; }

    static CRGBA GetCurrentSkyBottomColor()         { return m_CurrentColours.GetSkyBottom(); }

    static auto GetPostFxColors() { return std::make_pair(m_CurrentColours.GetPostFx1(), m_CurrentColours.GetPostFx2()); }

    static float SumOfCurrentRGB1() { return m_CurrentColours.m_fPostFx1Blue + m_CurrentColours.m_fPostFx1Green + m_CurrentColours.m_fPostFx1Red; }
    static float SumOfCurrentRGB2() { return m_CurrentColours.m_fPostFx2Blue + m_CurrentColours.m_fPostFx2Green + m_CurrentColours.m_fPostFx2Red; }

    static auto GetBoxes() { return std::span{ m_aBoxes, m_NumBoxes}; }
    static bool ShouldIgnoreSky() {
        return (
            m_nSkyTopRed[m_ExtraColour][m_ExtraColourWeatherType]   == 0 &&
            m_nSkyTopGreen[m_ExtraColour][m_ExtraColourWeatherType] == 0 &&
            m_nSkyTopBlue[m_ExtraColour][m_ExtraColourWeatherType]  == 0
        );
    }
};
// VALIDATE_SIZE(CTimeCycle, 0x919); Maybe?
