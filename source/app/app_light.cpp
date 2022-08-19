#include "StdInc.h"

#include "app.h"
#include "PostEffects.h"

// 0x735C80
void ActivateDirectional() {
    RpLightSetFlags(pDirect, rpLIGHTLIGHTATOMICS);
}

// 0x735C70
void DeActivateDirectional() {
    RpLightSetFlags(pDirect, 0x0);
}

// 0x5BA520
void LightsCreate(RpWorld* world) {
    if (!world) {
        return;
    }

    RwRGBAReal color;

    pAmbient = RpLightCreate(rpLIGHTAMBIENT);
    RpLightSetFlags(pAmbient, rpLIGHTLIGHTATOMICS);
    color.red   = 0.25f;
    color.green = 0.25f;
    color.blue  = 0.20f;
    RpLightSetColor(pAmbient, &color);

    pDirect = RpLightCreate(rpLIGHTDIRECTIONAL);
    RpLightSetFlags(pDirect, rpLIGHTLIGHTATOMICS);
    color.red   = 1.00f;
    color.green = 0.85f;
    color.blue  = 0.45f;
    RpLightSetColor(pDirect, &color);
    RpLightSetRadius(pDirect, 2.0f);

    RwFrame* frame = RwFrameCreate();
    rwObjectHasFrameSetFrame(pDirect, frame);
    CVector vecLight = { 1.0f, 1.0f, 0.0f };
    RwFrameRotate(frame, &vecLight, 160.0f, rwCOMBINEPRECONCAT);

    RpWorldAddLight(world, pAmbient);
    RpWorldAddLight(world, pDirect);

    for (auto& light : pExtraDirectionals) {
        light = RpLightCreate(rpLIGHTDIRECTIONAL);
        RpLightSetFlags(light, NULL);

        color.red   = 1.0f;
        color.green = 0.5f;
        color.blue  = 0.0f;
        RpLightSetColor(light, &color);
        RpLightSetRadius(light, 2.0f);
        rwObjectHasFrameSetFrame(light, RwFrameCreate());

        RpWorldAddLight(world, light);
    }
}

// 0x735730
void LightsDestroy(RpWorld* world) {
    if (!world) {
        return;
    }

    if (pAmbient) {
        RpWorldRemoveLight(world, pAmbient);
        RpLightDestroy(pAmbient);
        pAmbient = nullptr;
    }

    if (pDirect) {
        RpWorldRemoveLight(world, pDirect);
        RwFrameDestroy(RpLightGetFrame(pDirect));
        RpLightDestroy(pDirect);
        pDirect = nullptr;
    }

    for (auto& light : pExtraDirectionals) {
        RpWorldRemoveLight(world, light);
        RwFrameDestroy(RpLightGetFrame(light));
        RpLightDestroy(light);
        light = nullptr;
    }
}

// 0x735720 unused
void LightsEnable(int32 enable) {
    // NOP
}

// 0x7354E0
void SetLightsWithTimeOfDayColour(RpWorld* world) {
    assert(world);
    if (pAmbient) {
        AmbientLightColourForFrame.red   = CTimeCycle::GetAmbientRed()   * CCoronas::LightsMult;
        AmbientLightColourForFrame.green = CTimeCycle::GetAmbientGreen() * CCoronas::LightsMult;
        AmbientLightColourForFrame.blue  = CTimeCycle::GetAmbientBlue()  * CCoronas::LightsMult;

        AmbientLightColourForFrame_PedsCarsAndObjects.red   = CTimeCycle::GetAmbientRed_Obj()   * CCoronas::LightsMult;
        AmbientLightColourForFrame_PedsCarsAndObjects.green = CTimeCycle::GetAmbientGreen_Obj() * CCoronas::LightsMult;
        AmbientLightColourForFrame_PedsCarsAndObjects.blue  = CTimeCycle::GetAmbientBlue_Obj()  * CCoronas::LightsMult;

        if (CWeather::LightningFlash) {
            AmbientLightColourForFrame.blue  = 1.0f;
            AmbientLightColourForFrame.green = 1.0f;
            AmbientLightColourForFrame.red   = 1.0f;
            AmbientLightColourForFrame_PedsCarsAndObjects.blue  = 1.0f;
            AmbientLightColourForFrame_PedsCarsAndObjects.green = 1.0f;
            AmbientLightColourForFrame_PedsCarsAndObjects.red   = 1.0f;
        }
        RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    }

    if (pDirect) {
        const float color = CTimeCycle::m_CurrentColours.m_fIllumination * 0.99609375f * CCoronas::LightsMult; // todo: magic
        DirectionalLightColourForFrame.red   = color;
        DirectionalLightColourForFrame.green = color;
        DirectionalLightColourForFrame.blue  = color;
        RpLightSetColor(pDirect, &DirectionalLightColourForFrame);

        CVector vecDir   = CTimeCycle::m_vecDirnLightToSun;
        CVector vecUp    = CrossProduct(CVector(0, 0, 1), vecDir);
        vecUp.Normalise();
        CVector vecRight = CrossProduct(vecUp, vecDir);

        RwMatrix mxTransform;
        mxTransform.right = vecRight;
        mxTransform.up    = vecUp;
        mxTransform.at    = -vecDir;
        RwFrameTransform(RpLightGetFrame(pDirect), &mxTransform, rwCOMBINEREPLACE);
    }
}

// lighting = [ 0.0f; 1.0f ]
// 0x7357E0
void WorldReplaceNormalLightsWithScorched(RpWorld* world, float lighting) {
    RwRGBAReal color{ lighting, lighting, lighting };
    RpLightSetColor(pAmbient, &color);
    DeActivateDirectional();
}

// unused
// 0x735820
void WorldReplaceScorchedLightsWithNormal(RpWorld* world) {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    ActivateDirectional();
}

// 0x735840
void AddAnExtraDirectionalLight(RpWorld* world, float x, float y, float z, float red, float green, float blue) {
    ((void(__cdecl *)(RpWorld*, float, float, float, float, float, float))0x735840)(world, x, y, z, red, green, blue);
}

// 0x7359E0
void RemoveExtraDirectionalLights(RpWorld* world) {
    for (auto& light : pExtraDirectionals) {
        RpLightSetFlags(light, 0x0);
    }
    numExtraDirectionalLights = 0;
}

// fMult = [ 0.0f; 1.0f ]
// used in SetFlashyColours and SetFlashyColours_Mild which unused
// 0x735A20
void SetAmbientAndDirectionalColours(float fMult) {
    AmbientLightColour.red       = fMult * AmbientLightColourForFrame.red;
    AmbientLightColour.green     = fMult * AmbientLightColourForFrame.green;
    AmbientLightColour.blue      = fMult * AmbientLightColourForFrame.blue;

    DirectionalLightColour.red   = fMult * DirectionalLightColourForFrame.red;
    DirectionalLightColour.green = fMult * DirectionalLightColourForFrame.green;
    DirectionalLightColour.blue  = fMult * DirectionalLightColourForFrame.blue;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect,  &DirectionalLightColour);
}

// 0x735C40
void ReSetAmbientAndDirectionalColours() {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    RpLightSetColor(pDirect, &DirectionalLightColourForFrame);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735AB0 unused
void SetFlashyColours(float fMult) {
    if ((CTimer::GetTimeInMS() & 256) != 0) {
        AmbientLightColour.red   = 1.0f;
        AmbientLightColour.green = 1.0f;
        AmbientLightColour.blue  = 1.0f;
        DirectionalLightColour = DirectionalLightColourForFrame;

        RpLightSetColor(pAmbient, &AmbientLightColour);
        RpLightSetColor(pDirect,  &DirectionalLightColour);
        return;
    }
    SetAmbientAndDirectionalColours(fMult * 0.75f);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735B40 unused
void SetFlashyColours_Mild(float fMult) {
    if ((CTimer::GetTimeInMS() & 256) != 0) {
        AmbientLightColour.red   = 0.65f;
        AmbientLightColour.green = 0.65f;
        AmbientLightColour.blue  = 0.65f;
        DirectionalLightColour = DirectionalLightColourForFrame;

        RpLightSetColor(pAmbient, &AmbientLightColour);
        RpLightSetColor(pDirect,  &DirectionalLightColour);
        return;
    }
    SetAmbientAndDirectionalColours(fMult * 0.9f);
}

// 0x735BD0
void SetBrightMarkerColours(float lighting) {
    UNUSED(lighting);
    AmbientLightColour.red   = 0.6f;
    AmbientLightColour.green = 0.6f;
    AmbientLightColour.blue  = 0.6f;

    DirectionalLightColour.red   = 1.0f;
    DirectionalLightColour.green = 1.0f;
    DirectionalLightColour.blue  = 1.0f;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect,  &DirectionalLightColour);
}

// 0x735D70
void SetDirectionalColours(RwRGBAReal* color) {
    assert(color);
    RpLightSetColor(pDirect, color);
}

// unused
// 0x735C90
void SetAmbientColoursToIndicateRoadGroup(int32 idx) {
    static uint8 (&IndicateR)[8] = *reinterpret_cast<uint8 (*)[8]>(0x8D60D0);
    static uint8 (&IndicateG)[8] = *reinterpret_cast<uint8 (*)[8]>(0x8D60D8);
    static uint8 (&IndicateB)[8] = *reinterpret_cast<uint8 (*)[8]>(0x8D60E0);

    AmbientLightColour.red   = (float)IndicateR[idx & std::size(IndicateR)] * (1.0f / 255.0f);
    AmbientLightColour.green = (float)IndicateG[idx & std::size(IndicateG)] * (1.0f / 255.0f);
    AmbientLightColour.blue  = (float)IndicateB[idx & std::size(IndicateB)] * (1.0f / 255.0f);
    RpLightSetColor(pAmbient, &AmbientLightColour);
}

// unused
// 0x735D10
void SetFullAmbient() {
    static RwRGBAReal& FullLight = *reinterpret_cast<RwRGBAReal*>(0x8D60C0); // { 1.0f, 1.0f, 1.0f, 1.0f }
    RpLightSetColor(pAmbient, &FullLight);
}

// 0x735D30
void SetAmbientColours() {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
}

// 0x735D50
void SetAmbientColours(RwRGBAReal* color) {
    assert(color);
    RpLightSetColor(pAmbient, color);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735D90
void SetLightColoursForPedsCarsAndObjects(float fMult) {
    DirectionalLightColour.red   = fMult * DirectionalLightColourForFrame.red;
    DirectionalLightColour.green = fMult * DirectionalLightColourForFrame.green;
    DirectionalLightColour.blue  = fMult * DirectionalLightColourForFrame.blue;

    AmbientLightColour.red       = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.red;
    AmbientLightColour.green     = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.green;
    AmbientLightColour.blue      = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.blue;

    AmbientLightColour.red      += CTimeCycle::m_BrightnessAddedToAmbientRed;
    AmbientLightColour.green    += CTimeCycle::m_BrightnessAddedToAmbientRed;
    AmbientLightColour.blue     += CTimeCycle::m_BrightnessAddedToAmbientRed;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect, &DirectionalLightColour);
}

// 0x735E40
void SetLightsForInfraredVisionHeatObjects() {
    RpLightSetColor(pAmbient, &CPostEffects::m_fInfraredVisionHeatObjectCol);
    RpLightSetColor(pDirect,  &CPostEffects::m_fInfraredVisionHeatObjectCol);
}

// 0x735E70
void StoreAndSetLightsForInfraredVisionHeatObjects() {
    DirectAmbientLight[1] = AmbientLightColour;
    DirectAmbientLight[0] = DirectionalLightColour;
    SetLightsForInfraredVisionHeatObjects();
}

// 0x735EF0
void RestoreLightsForInfraredVisionHeatObjects() {
    RpLightSetColor(pAmbient, &DirectAmbientLight[1]);
    RpLightSetColor(pDirect, &DirectAmbientLight[0]);
}

// 0x735F20
void SetLightsForInfraredVisionDefaultObjects() {
    RwRGBAReal color{ 0.0f, 0.0f, 1.0f, 1.0f };
    RpLightSetColor(pAmbient, &color);
    RpLightSetColor(pDirect,  &color);
}

// 0x735F70
void SetLightsForNightVision() {
    RwRGBAReal color{ 0.0f, 1.0f, 0.0f, 1.0f };
    RpLightSetColor(pAmbient, &color);
    RpLightSetColor(pDirect,  &color);
}

// 0x6FAB30
float GetDayNightBalance() {
    const auto minutes = CClock::GetMinutesToday();

    if (minutes < 6.0f * 60.0f) // 0-6 o'clock
        return 1.0f;

    if (minutes < 7.0f * 60.0f) // 6-7 o'clock
        return (7.0f * 60.0f - minutes) / 60.0f;

    if (minutes < 20.0f * 60.0f) // 7-20 o'clock
        return 0.0f;

    if (minutes >= 21.0f * 60.0f) // 21-0 o'clock
        return 1.0f;

    return 1.0f - (21.0f * 60.0f - minutes) / 60.0f; // 20-21 o'clock
}
