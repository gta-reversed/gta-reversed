#include "StdInc.h"

#include "Draw.h"

float& CDraw::ms_fFOV = *(float *)0x8D5038;
float& CDraw::ms_fLODDistance = *(float *)0xC3EF98;
float& CDraw::ms_fFarClipZ = *(float *)0xC3EF9C;
float& CDraw::ms_fNearClipZ = *(float *)0xC3EFA0;
float& CDraw::ms_fAspectRatio = *(float *)0xC3EFA4;
uint8& CDraw::FadeRed = *(uint8*)0xC3EFA8;
uint8& CDraw::FadeGreen = *(uint8*)0xC3EFA9;
uint8& CDraw::FadeBlue = *(uint8*)0xC3EFAA;
uint8& CDraw::FadeValue = *(uint8*)0xC3EFAB;

uint32& FadeTimer = *(uint32*)0xC16EE0;
uint32& FadeOutTime = *(uint32*)0x8D2BD4;

void CDraw::InjectHooks() {
    RH_ScopedClass(CDraw);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetFOV, 0x6FF410);
    RH_ScopedInstall(CalculateAspectRatio, 0x6FF420);
}

// 0x6FF410
void CDraw::SetFOV(float fov) {
    ms_fFOV = fov;
}

// 0x6FF420
void CDraw::CalculateAspectRatio() {
    if (FrontEndMenuManager.m_bWidescreenOn)
        ms_fAspectRatio = 16.f / 9.f;
    else if (TheCamera.m_bWideScreenOn)
        ms_fAspectRatio = 5.f / 4.f;
    else
        ms_fAspectRatio = 4.f / 3.f;
}

// 0x53E600
void DoFade() {
    ZoneScoped;

    if (CTimer::GetIsPaused())
        return;

    if (JustLoadedDontFadeInYet) {
        FadeTimer = CTimer::GetTimeInMS();
        JustLoadedDontFadeInYet = false;
    }

    if (StillToFadeOut) {
        if (CTimer::GetTimeInMS() - FadeTimer <= FadeOutTime) {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
            TheCamera.ProcessFade();
        } else {
            StillToFadeOut = false;

            TheCamera.Fade(3.0f, eFadeFlag::FADE_OUT);
            TheCamera.ProcessFade();
            TheCamera.ProcessMusicFade();
        }
    }

    if (CDraw::IsFading()) {
        CRGBA color(0, 0, 0, 0);

        if (!TheCamera.m_bFadeTargetIsSplashScreen) {
            color.Set(
                (CDraw::FadeRed   * CDraw::FadeValue) / CDraw::FadeValue,
                (CDraw::FadeGreen * CDraw::FadeValue) / CDraw::FadeValue,
                (CDraw::FadeBlue  * CDraw::FadeValue) / CDraw::FadeValue,
                std::max<uint8>(0, CDraw::FadeValue)
            );
        }

        #if FIX_BUGS
        CSprite2d::DrawRect({-5.0f, -5.0f, SCREEN_WIDTH + 5.0f, SCREEN_HEIGHT + 5.0f}, color);
        #else
        CSprite2d::DrawRect({-5.0f, SCREEN_HEIGHT + 5.0f, SCREEN_WIDTH + 5.0f, -5.0f}, color);
        #endif
    }
}
