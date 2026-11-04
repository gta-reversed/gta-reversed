#include "StdInc.h"

#include "ColourSet.h"

void CColourSet::InjectHooks() {
    RH_ScopedClass(CColourSet);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x55F4B0);
    RH_ScopedInstall(Interpolate, 0x55F870);
}

// 0x55F4B0
CColourSet::CColourSet(int32 timeId, int32 weatherId) {
    m_fAmbientRed               = CTimeCycle::m_nAmbientRed[timeId][weatherId];
    m_fAmbientGreen             = CTimeCycle::m_nAmbientGreen[timeId][weatherId];
    m_fAmbientBlue              = CTimeCycle::m_nAmbientBlue[timeId][weatherId];

    m_fAmbientRed_Obj           = CTimeCycle::m_nAmbientRed_Obj[timeId][weatherId];
    m_fAmbientGreen_Obj         = CTimeCycle::m_nAmbientGreen_Obj[timeId][weatherId];
    m_fAmbientBlue_Obj          = CTimeCycle::m_nAmbientBlue_Obj[timeId][weatherId];

    m_nSkyTopRed                = CTimeCycle::m_nSkyTopRed[timeId][weatherId];
    m_nSkyTopGreen              = CTimeCycle::m_nSkyTopGreen[timeId][weatherId];
    m_nSkyTopBlue               = CTimeCycle::m_nSkyTopBlue[timeId][weatherId];

    m_nSkyBottomRed             = CTimeCycle::m_nSkyBottomRed[timeId][weatherId];
    m_nSkyBottomGreen           = CTimeCycle::m_nSkyBottomGreen[timeId][weatherId];
    m_nSkyBottomBlue            = CTimeCycle::m_nSkyBottomBlue[timeId][weatherId];

    m_nSunCoreRed               = CTimeCycle::m_nSunCoreRed[timeId][weatherId];
    m_nSunCoreGreen             = CTimeCycle::m_nSunCoreGreen[timeId][weatherId];
    m_nSunCoreBlue              = CTimeCycle::m_nSunCoreBlue[timeId][weatherId];

    m_nSunCoronaRed             = CTimeCycle::m_nSunCoronaRed[timeId][weatherId];
    m_nSunCoronaGreen           = CTimeCycle::m_nSunCoronaGreen[timeId][weatherId];
    m_nSunCoronaBlue            = CTimeCycle::m_nSunCoronaBlue[timeId][weatherId];

    m_fSunSize                  = CTimeCycle::m_fSunSize[timeId][weatherId];
    m_fSpriteSize               = CTimeCycle::m_fSpriteSize[timeId][weatherId];
    m_fSpriteBrightness         = CTimeCycle::m_fSpriteBrightness[timeId][weatherId];

    m_nShadowStrength           = CTimeCycle::m_nShadowStrength[timeId][weatherId];
    m_nLightShadowStrength      = CTimeCycle::m_nLightShadowStrength[timeId][weatherId];
    m_nPoleShadowStrength       = CTimeCycle::m_nPoleShadowStrength[timeId][weatherId];

    m_fFarClip                  = CTimeCycle::m_fFarClip[timeId][weatherId];
    m_fFogStart                 = CTimeCycle::m_fFogStart[timeId][weatherId];
    m_fLightsOnGroundBrightness = CTimeCycle::m_fLightsOnGroundBrightness[timeId][weatherId];

    m_nLowCloudsRed             = CTimeCycle::m_nLowCloudsRed[timeId][weatherId];
    m_nLowCloudsGreen           = CTimeCycle::m_nLowCloudsGreen[timeId][weatherId];
    m_nLowCloudsBlue            = CTimeCycle::m_nLowCloudsBlue[timeId][weatherId];

    m_nFluffyCloudsBottomRed    = CTimeCycle::m_nFluffyCloudsBottomRed[timeId][weatherId];
    m_nFluffyCloudsBottomGreen  = CTimeCycle::m_nFluffyCloudsBottomGreen[timeId][weatherId];
    m_nFluffyCloudsBottomBlue   = CTimeCycle::m_nFluffyCloudsBottomBlue[timeId][weatherId];

    m_fWaterRed                 = CTimeCycle::m_fWaterRed[timeId][weatherId];
    m_fWaterGreen               = CTimeCycle::m_fWaterGreen[timeId][weatherId];
    m_fWaterBlue                = CTimeCycle::m_fWaterBlue[timeId][weatherId];
    m_fWaterAlpha               = CTimeCycle::m_fWaterAlpha[timeId][weatherId];

    m_fPostFx1Red               = CTimeCycle::m_fPostFx1Red[timeId][weatherId];
    m_fPostFx1Green             = CTimeCycle::m_fPostFx1Green[timeId][weatherId];
    m_fPostFx1Blue              = CTimeCycle::m_fPostFx1Blue[timeId][weatherId];
    m_fPostFx1Alpha             = CTimeCycle::m_fPostFx1Alpha[timeId][weatherId];

    m_fPostFx2Red               = CTimeCycle::m_fPostFx2Red[timeId][weatherId];
    m_fPostFx2Green             = CTimeCycle::m_fPostFx2Green[timeId][weatherId];
    m_fPostFx2Blue              = CTimeCycle::m_fPostFx2Blue[timeId][weatherId];
    m_fPostFx2Alpha             = CTimeCycle::m_fPostFx2Alpha[timeId][weatherId];

    m_fCloudAlpha               = CTimeCycle::m_fCloudAlpha[timeId][weatherId];
    m_nHighLightMinIntensity    = CTimeCycle::m_nHighLightMinIntensity[timeId][weatherId];
    m_nWaterFogAlpha            = CTimeCycle::m_nWaterFogAlpha[timeId][weatherId];
    m_fLodDistMult              = 1.0f;
    m_fIllumination             = CTimeCycle::m_nDirectionalMult[timeId][weatherId] / 100.0f;
}

/*!
 * @brief Interpolates colour sets
 * @param A First Color Set
 * @param B Second Color Set
 * @param multA First Factor
 * @param multB Second Factor
 * @param ignoreSky
 * @addr 0x55F870
 */
void CColourSet::Interpolate(CColourSet* A, CColourSet* B, float multA, float multB, bool ignoreSky) {
    if (!ignoreSky) {
        m_nSkyTopRed   = (uint16)(A->m_nSkyTopRed   * multA + B->m_nSkyTopRed   * multB);
        m_nSkyTopGreen = (uint16)(A->m_nSkyTopGreen * multA + B->m_nSkyTopGreen * multB);
        m_nSkyTopBlue  = (uint16)(A->m_nSkyTopBlue  * multA + B->m_nSkyTopBlue  * multB);

        m_nSkyBottomRed   = (uint16)(A->m_nSkyBottomRed   * multA + B->m_nSkyBottomRed   * multB);
        m_nSkyBottomGreen = (uint16)(A->m_nSkyBottomGreen * multA + B->m_nSkyBottomGreen * multB);
        m_nSkyBottomBlue  = (uint16)(A->m_nSkyBottomBlue  * multA + B->m_nSkyBottomBlue  * multB);

        m_nSunCoreRed   = (uint16)(A->m_nSunCoreRed   * multA + B->m_nSunCoreRed   * multB);
        m_nSunCoreGreen = (uint16)(A->m_nSunCoreGreen * multA + B->m_nSunCoreGreen * multB);
        m_nSunCoreBlue  = (uint16)(A->m_nSunCoreBlue  * multA + B->m_nSunCoreBlue  * multB);

        m_nSunCoronaRed   = (uint16)(A->m_nSunCoronaRed   * multA + B->m_nSunCoronaRed   * multB);
        m_nSunCoronaGreen = (uint16)(A->m_nSunCoronaGreen * multA + B->m_nSunCoronaGreen * multB);
        m_nSunCoronaBlue  = (uint16)(A->m_nSunCoronaBlue  * multA + B->m_nSunCoronaBlue  * multB);

        m_fSunSize = multA * A->m_fSunSize + multB * B->m_fSunSize;

        m_nLowCloudsRed   = (uint16)(A->m_nLowCloudsRed   * multA + B->m_nLowCloudsRed   * multB);
        m_nLowCloudsGreen = (uint16)(A->m_nLowCloudsGreen * multA + B->m_nLowCloudsGreen * multB);
        m_nLowCloudsBlue  = (uint16)(A->m_nLowCloudsBlue  * multA + B->m_nLowCloudsBlue  * multB);

        m_nFluffyCloudsBottomRed   = (uint16)(A->m_nFluffyCloudsBottomRed   * multA + B->m_nFluffyCloudsBottomRed   * multB);
        m_nFluffyCloudsBottomGreen = (uint16)(A->m_nFluffyCloudsBottomGreen * multA + B->m_nFluffyCloudsBottomGreen * multB);
        m_nFluffyCloudsBottomBlue  = (uint16)(A->m_nFluffyCloudsBottomBlue  * multA + B->m_nFluffyCloudsBottomBlue  * multB);
    }
    m_fAmbientRed   = multA * A->m_fAmbientRed   + multB * B->m_fAmbientRed;
    m_fAmbientGreen = multA * A->m_fAmbientGreen + multB * B->m_fAmbientGreen;
    m_fAmbientBlue  = multA * A->m_fAmbientBlue  + multB * B->m_fAmbientBlue;

    m_fAmbientRed_Obj   = multA * A->m_fAmbientRed_Obj   + multB * B->m_fAmbientRed_Obj;
    m_fAmbientGreen_Obj = multA * A->m_fAmbientGreen_Obj + multB * B->m_fAmbientGreen_Obj;
    m_fAmbientBlue_Obj  = multA * A->m_fAmbientBlue_Obj  + multB * B->m_fAmbientBlue_Obj;

    m_fSpriteSize       = multA * A->m_fSpriteSize       + multB * B->m_fSpriteSize;
    m_fSpriteBrightness = multA * A->m_fSpriteBrightness + multB * B->m_fSpriteBrightness;

    m_nShadowStrength      = (int16)(A->m_nShadowStrength      * multA + B->m_nShadowStrength      * multB);
    m_nLightShadowStrength = (int16)(A->m_nLightShadowStrength * multA + B->m_nLightShadowStrength * multB);
    m_nPoleShadowStrength  = (int16)(A->m_nPoleShadowStrength  * multA + B->m_nPoleShadowStrength  * multB);

    m_fFarClip  = multA * A->m_fFarClip  + multB * B->m_fFarClip;
    m_fFogStart = multA * A->m_fFogStart + multB * B->m_fFogStart;

    m_fPostFx1Red   = multA * A->m_fPostFx1Red   + multB * B->m_fPostFx1Red;
    m_fPostFx1Green = multA * A->m_fPostFx1Green + multB * B->m_fPostFx1Green;
    m_fPostFx1Blue  = multA * A->m_fPostFx1Blue  + multB * B->m_fPostFx1Blue;
    m_fPostFx1Alpha = multA * A->m_fPostFx1Alpha + multB * B->m_fPostFx1Alpha;

    m_fPostFx2Red   = multA * A->m_fPostFx2Red   + multB * B->m_fPostFx2Red;
    m_fPostFx2Green = multA * A->m_fPostFx2Green + multB * B->m_fPostFx2Green;
    m_fPostFx2Blue  = multA * A->m_fPostFx2Blue  + multB * B->m_fPostFx2Blue;
    m_fPostFx2Alpha = multA * A->m_fPostFx2Alpha + multB * B->m_fPostFx2Alpha;

    m_fLightsOnGroundBrightness = multA * A->m_fLightsOnGroundBrightness + multB * B->m_fLightsOnGroundBrightness;
    m_fCloudAlpha = multA * A->m_fCloudAlpha + multB * B->m_fCloudAlpha;
    m_nHighLightMinIntensity = (uint16)(A->m_nHighLightMinIntensity * multA + B->m_nHighLightMinIntensity * multB);
    m_nWaterFogAlpha = (uint16)(A->m_nWaterFogAlpha * multA + B->m_nWaterFogAlpha * multB);
    m_fIllumination = multA * A->m_fIllumination + multB * B->m_fIllumination;
    m_fLodDistMult = multA * A->m_fLodDistMult + multB * B->m_fLodDistMult;

    m_fWaterRed   = multA * A->m_fWaterRed   + multB * B->m_fWaterRed;
    m_fWaterGreen = multA * A->m_fWaterGreen + multB * B->m_fWaterGreen;
    m_fWaterBlue  = multA * A->m_fWaterBlue  + multB * B->m_fWaterBlue;
    m_fWaterAlpha = multA * A->m_fWaterAlpha + multB * B->m_fWaterAlpha;
}
