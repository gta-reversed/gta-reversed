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
    m_fAmbientRed               = (float)CTimeCycle::m_nAmbientRed[timeId][weatherId];
    m_fAmbientGreen             = (float)CTimeCycle::m_nAmbientGreen[timeId][weatherId];
    m_fAmbientBlue              = (float)CTimeCycle::m_nAmbientBlue[timeId][weatherId];

    m_fAmbientRed_Obj           = (float)CTimeCycle::m_nAmbientRed_Obj[timeId][weatherId];
    m_fAmbientGreen_Obj         = (float)CTimeCycle::m_nAmbientGreen_Obj[timeId][weatherId];
    m_fAmbientBlue_Obj          = (float)CTimeCycle::m_nAmbientBlue_Obj[timeId][weatherId];

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

    m_fSunSize                  = (float)CTimeCycle::m_fSunSize[timeId][weatherId];
    m_fSpriteSize               = (float)CTimeCycle::m_fSpriteSize[timeId][weatherId];

    m_fSpriteBrightness         = (float)CTimeCycle::m_fSpriteBrightness[timeId][weatherId];
    m_nShadowStrength           = CTimeCycle::m_nShadowStrength[timeId][weatherId];
    m_nLightShadowStrength      = CTimeCycle::m_nPoleShadowStrength[timeId][weatherId];
    m_nPoleShadowStrength       = CTimeCycle::m_nPoleShadowStrength[timeId][weatherId];

    m_fFarClip                  = (float)CTimeCycle::m_fFarClip[timeId][weatherId];
    m_fFogStart                 = (float)CTimeCycle::m_fFogStart[timeId][weatherId];

    m_fLightsOnGroundBrightness = (float)CTimeCycle::m_fLightsOnGroundBrightness[timeId][weatherId];
    m_nLowCloudsRed             = CTimeCycle::m_nLowCloudsRed[timeId][weatherId];
    m_nLowCloudsGreen           = CTimeCycle::m_nLowCloudsGreen[timeId][weatherId];
    m_nLowCloudsBlue            = CTimeCycle::m_nLowCloudsBlue[timeId][weatherId];

    m_nFluffyCloudsBottomRed    = CTimeCycle::m_nFluffyCloudsBottomRed[timeId][weatherId];
    m_nFluffyCloudsBottomGreen  = CTimeCycle::m_nFluffyCloudsBottomGreen[timeId][weatherId];
    m_nFluffyCloudsBottomBlue   = CTimeCycle::m_nFluffyCloudsBottomBlue[timeId][weatherId];

    m_fWaterRed                 = (float)CTimeCycle::m_fWaterRed[timeId][weatherId];
    m_fWaterGreen               = (float)CTimeCycle::m_fWaterGreen[timeId][weatherId];
    m_fWaterBlue                = (float)CTimeCycle::m_fWaterBlue[timeId][weatherId];
    m_fWaterAlpha               = (float)CTimeCycle::m_fWaterAlpha[timeId][weatherId];

    m_fPostFx1Red               = (float)CTimeCycle::m_fPostFx1Red[timeId][weatherId];
    m_fPostFx1Green             = (float)CTimeCycle::m_fPostFx1Green[timeId][weatherId];
    m_fPostFx1Blue              = (float)CTimeCycle::m_fPostFx1Blue[timeId][weatherId];
    m_fPostFx1Alpha             = (float)CTimeCycle::m_fPostFx1Alpha[timeId][weatherId];

    m_fPostFx2Red               = (float)CTimeCycle::m_fPostFx2Red[timeId][weatherId];
    m_fPostFx2Green             = (float)CTimeCycle::m_fPostFx2Green[timeId][weatherId];
    m_fPostFx2Blue              = (float)CTimeCycle::m_fPostFx2Blue[timeId][weatherId];
    m_fPostFx2Alpha             = (float)CTimeCycle::m_fPostFx2Alpha[timeId][weatherId];

    m_fCloudAlpha               = (float)CTimeCycle::m_fCloudAlpha[timeId][weatherId];
    m_nHighLightMinIntensity    = CTimeCycle::m_nHighLightMinIntensity[timeId][weatherId];
    m_nWaterFogAlpha            = CTimeCycle::m_nWaterFogAlpha[timeId][weatherId];
    m_fLodDistMult              = 1.0f;
    m_fIllumination             = (float)CTimeCycle::m_nDirectionalMult[timeId][weatherId] / 100.0f;

    // Originally not initialized
    m_fAmbientBeforeBrightnessRed = 0.0f;
    m_fAmbientBeforeBrightnessGreen = 0.0f;
    m_fAmbientBeforeBrightnessBlue  = 0.0f;
}

/*!
 * @brief Interpolates colour sets
 * @param a First Color Set
 * @param b Second Color Set
 * @param fa First Factor
 * @param fb Second Factor
 * @param bIgnoreSky
 * @addr 0x55F870
 */
void CColourSet::Interpolate(CColourSet* a, CColourSet* b, float fa, float fb, bool bIgnoreSky) {
    if (!bIgnoreSky) {
        m_nSkyTopRed   = (uint16)((float)a->m_nSkyTopRed   * fa + (float)b->m_nSkyTopRed   * fb);
        m_nSkyTopGreen = (uint16)((float)a->m_nSkyTopGreen * fa + (float)b->m_nSkyTopGreen * fb);
        m_nSkyTopBlue  = (uint16)((float)a->m_nSkyTopBlue  * fa + (float)b->m_nSkyTopBlue  * fb);

        m_nSkyBottomRed   = (uint16)((float)a->m_nSkyBottomRed   * fa + (float)b->m_nSkyBottomRed   * fb);
        m_nSkyBottomGreen = (uint16)((float)a->m_nSkyBottomGreen * fa + (float)b->m_nSkyBottomGreen * fb);
        m_nSkyBottomBlue  = (uint16)((float)a->m_nSkyBottomBlue  * fa + (float)b->m_nSkyBottomBlue  * fb);

        m_nSunCoreRed   = (uint16)((float)a->m_nSunCoreRed   * fa + (float)b->m_nSunCoreRed   * fb);
        m_nSunCoreGreen = (uint16)((float)a->m_nSunCoreGreen * fa + (float)b->m_nSunCoreGreen * fb);
        m_nSunCoreBlue  = (uint16)((float)a->m_nSunCoreBlue  * fa + (float)b->m_nSunCoreBlue  * fb);

        m_nSunCoronaRed   = (uint16)((float)a->m_nSunCoronaRed   * fa + (float)b->m_nSunCoronaRed   * fb);
        m_nSunCoronaGreen = (uint16)((float)a->m_nSunCoronaGreen * fa + (float)b->m_nSunCoronaGreen * fb);
        m_nSunCoronaBlue  = (uint16)((float)a->m_nSunCoronaBlue  * fa + (float)b->m_nSunCoronaBlue  * fb);

        m_fSunSize = fa * a->m_fSunSize + fb * b->m_fSunSize;

        m_nLowCloudsRed   = (uint16)((float)a->m_nLowCloudsRed   * fa + (float)b->m_nLowCloudsRed   * fb);
        m_nLowCloudsGreen = (uint16)((float)a->m_nLowCloudsGreen * fa + (float)b->m_nLowCloudsGreen * fb);
        m_nLowCloudsBlue  = (uint16)((float)a->m_nLowCloudsBlue  * fa + (float)b->m_nLowCloudsBlue  * fb);

        m_nFluffyCloudsBottomRed   = (uint16)((float)a->m_nFluffyCloudsBottomRed   * fa + (float)b->m_nFluffyCloudsBottomRed   * fb);
        m_nFluffyCloudsBottomGreen = (uint16)((float)a->m_nFluffyCloudsBottomGreen * fa + (float)b->m_nFluffyCloudsBottomGreen * fb);
        m_nFluffyCloudsBottomBlue  = (uint16)((float)a->m_nFluffyCloudsBottomBlue  * fa + (float)b->m_nFluffyCloudsBottomBlue  * fb);
    }
    m_fAmbientRed   = fa * a->m_fAmbientRed   + fb * b->m_fAmbientRed;
    m_fAmbientGreen = fa * a->m_fAmbientGreen + fb * b->m_fAmbientGreen;
    m_fAmbientBlue  = fa * a->m_fAmbientBlue  + fb * b->m_fAmbientBlue;

    m_fAmbientRed_Obj   = fa * a->m_fAmbientRed_Obj   + fb * b->m_fAmbientRed_Obj;
    m_fAmbientGreen_Obj = fa * a->m_fAmbientGreen_Obj + fb * b->m_fAmbientGreen_Obj;
    m_fAmbientBlue_Obj  = fa * a->m_fAmbientBlue_Obj  + fb * b->m_fAmbientBlue_Obj;

    m_fSpriteSize       = fa * a->m_fSpriteSize       + fb * b->m_fSpriteSize;
    m_fSpriteBrightness = fa * a->m_fSpriteBrightness + fb * b->m_fSpriteBrightness;

    m_nShadowStrength      = (uint16)((float)(int16)a->m_nShadowStrength      * fa + (float)(int16)b->m_nShadowStrength      * fb);
    m_nLightShadowStrength = (uint16)((float)(int16)a->m_nLightShadowStrength * fa + (float)(int16)b->m_nLightShadowStrength * fb);
    m_nPoleShadowStrength  = (uint16)((float)(int16)a->m_nPoleShadowStrength  * fa + (float)(int16)b->m_nPoleShadowStrength  * fb);

    m_fFarClip  = fa * a->m_fFarClip  + fb * b->m_fFarClip;
    m_fFogStart = fa * a->m_fFogStart + fb * b->m_fFogStart;

    m_fPostFx1Red   = fa * a->m_fPostFx1Red   + fb * b->m_fPostFx1Red;
    m_fPostFx1Green = fa * a->m_fPostFx1Green + fb * b->m_fPostFx1Green;
    m_fPostFx1Blue  = fa * a->m_fPostFx1Blue  + fb * b->m_fPostFx1Blue;
    m_fPostFx1Alpha = fa * a->m_fPostFx1Alpha + fb * b->m_fPostFx1Alpha;

    m_fPostFx2Red   = fa * a->m_fPostFx2Red   + fb * b->m_fPostFx2Red;
    m_fPostFx2Green = fa * a->m_fPostFx2Green + fb * b->m_fPostFx2Green;
    m_fPostFx2Blue  = fa * a->m_fPostFx2Blue  + fb * b->m_fPostFx2Blue;
    m_fPostFx2Alpha = fa * a->m_fPostFx2Alpha + fb * b->m_fPostFx2Alpha;

    m_fLightsOnGroundBrightness = fa * a->m_fLightsOnGroundBrightness + fb * b->m_fLightsOnGroundBrightness;
    m_fCloudAlpha = fa * a->m_fCloudAlpha + fb * b->m_fCloudAlpha;
    m_nHighLightMinIntensity = (uint16)((float)a->m_nHighLightMinIntensity * fa + (float)b->m_nHighLightMinIntensity * fb);
    m_nWaterFogAlpha = (uint16)((float)a->m_nWaterFogAlpha * fa + (float)b->m_nWaterFogAlpha * fb);
    m_fIllumination = fa * a->m_fIllumination + fb * b->m_fIllumination;
    m_fLodDistMult = fa * a->m_fLodDistMult + fb * b->m_fLodDistMult;

    m_fWaterRed   = fa * a->m_fWaterRed   + fb * b->m_fWaterRed;
    m_fWaterGreen = fa * a->m_fWaterGreen + fb * b->m_fWaterGreen;
    m_fWaterBlue  = fa * a->m_fWaterBlue  + fb * b->m_fWaterBlue;
    m_fWaterAlpha = fa * a->m_fWaterAlpha + fb * b->m_fWaterAlpha;
}
