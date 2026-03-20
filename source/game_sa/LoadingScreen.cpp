/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "platform.h"
#include "LoadingScreen.h"

#include "extensions/Configs/FastLoader.hpp"

void CLoadingScreen::InjectHooks() {
    RH_ScopedClass(CLoadingScreen);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5902B0);
    RH_ScopedInstall(Shutdown, 0x58FF10);
    RH_ScopedInstall(RenderSplash, 0x58FF60);
    RH_ScopedInstall(LoadSplashes, 0x5900B0);
    RH_ScopedInstall(DisplayMessage, 0x590220);
    RH_ScopedInstall(SetLoadingBarMsg, 0x590240);
    RH_ScopedInstall(GetClockTime, 0x590280);
    RH_ScopedInstall(Pause, 0x590310);
    RH_ScopedInstall(Continue, 0x590320);
    RH_ScopedInstall(RenderLoadingBar, 0x590370);
    RH_ScopedInstall(DisplayNextSplash, 0x5904D0);
    RH_ScopedInstall(StartFading, 0x590530);
    RH_ScopedInstall(DisplayPCScreen, 0x590570);
    RH_ScopedInstall(Update, 0x5905E0);
    RH_ScopedInstall(DoPCTitleFadeIn, 0x590860);
    RH_ScopedInstall(DoPCTitleFadeOut, 0x590990);
    RH_ScopedInstall(DoPCScreenChange, 0x590AC0);
    RH_ScopedInstall(NewChunkLoaded, 0x590D00);
    RH_ScopedInstall(IsActive, 0x744DB5);

    RH_ScopedGlobalInstall(LoadingScreen, 0x53DED0);
}

// 0x5902B0
void CLoadingScreen::Init(bool isLegalScreen, bool isReloadDisabled) {
    if (IsActive()) {
        return;
    }

    if (!isReloadDisabled) {
        LoadSplashes(false, eLoadingLogo::EAX);
    }

    m_currDisplayedSplash = eDisplayedSplash::NONE;

    m_timeSinceLastScreen = GetClockTime();

    m_bActive = true;
}

// 0x58FF10
void CLoadingScreen::Shutdown(bool isForce) {
    m_bActive = false;

    // Release splash textures
    for (auto& splash : m_aSplashes) {
        if (splash.m_pTexture) {
            splash.Delete();
        }
    }

    if (const auto slot = CTxdStore::FindTxdSlot("loadsc0"); slot != -1) {
        CTxdStore::RemoveTxd(slot);
        CTxdStore::RemoveTxdSlot(slot);
    }
}

// 0x58FF60
// Edit in Mobile
void CLoadingScreen::RenderSplash() {
    // Screen dimensions
    constexpr float SCREEN_MARGIN = 5.0f;

    CSprite2d::InitPerFrame();

    constexpr CRect screenRect{
        -SCREEN_MARGIN,
        -SCREEN_MARGIN,
        SCREEN_WIDTH + SCREEN_MARGIN,
        SCREEN_HEIGHT + SCREEN_MARGIN
    };

    CRGBA color(255, 255, 255, 255);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSCLAMP));

    if (m_bFading) {
        GetCurrentDisplayedSplash().Draw(screenRect, color);

        if (m_bFadeInNextSplashFromBlack || m_bFadeOutCurrSplashToBlack) {
            color.Set(0, 0, 0);
            color.a = m_bFadeInNextSplashFromBlack
                ? (255 - m_FadeAlpha)
                : m_FadeAlpha;
            CSprite2d::DrawRect(screenRect, color);
        } else {
            color.a = 255 - m_FadeAlpha;
            m_aSplashes[+m_currDisplayedSplash - 1].Draw(screenRect, color);
        }
    } else if (!m_bReadyToDelete) {
        GetCurrentDisplayedSplash().Draw(screenRect, color);
    }
}

// 0x5900B0
// Edit in Mobile
void CLoadingScreen::LoadSplashes(bool useSplashId, eLoadingLogo id) {
    CFileMgr::SetDir("MODELS\\TXD\\");

    auto slot = CTxdStore::AddTxdSlot("loadsc0");
    CTxdStore::LoadTxd(slot, "loadscs.txd");
    CTxdStore::AddRef(slot);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(slot);

    // Initialize RNG -- affects the global state, can not be omitted.
    LARGE_INTEGER pc;
    QueryPerformanceCounter(&pc);
    srand(pc.u.LowPart);

    // NOTSA
    // Generate random splash order
    std::array<uint8, +eDisplayedSplash::COUNT> indices{};
    rng::iota(indices, 0);

    // exclude 0, title_pcXX.
    std::shuffle(indices.begin() + 1, indices.end(), std::mt19937{ std::random_device{}() });

    for (auto i = +eDisplayedSplash::COPYRIGHT; i < +eDisplayedSplash::COUNT; i++) {
        char texName[20];
        if (useSplashId) {
            const char* logoName;
            switch (id) {
            case eLoadingLogo::EAX:    logoName = "eax"; break;
            case eLoadingLogo::NVIDIA: logoName = "nvidia"; break;
            default:                   NOTSA_UNREACHABLE(); break;
            }
            notsa::format_to_sz(texName, "{}", logoName);
        } else if (i != 0) {
            notsa::format_to_sz(texName, "loadsc{}", indices[i]);
        } else {
#ifdef USE_EU_STUFF
            strcpy_s(texName, "title_pc_EU");
#else
            strcpy_s(texName, "title_pc_US");
#endif
        }
        m_aSplashes[i].SetTexture(texName);
    }

    CTxdStore::PopCurrentTxd();
    CFileMgr::SetDir("");
}

// 0x590220
void CLoadingScreen::DisplayMessage(const char* msg) {
    std::strcpy(m_PopUpMessage, msg);
}

// 0x590240
void CLoadingScreen::SetLoadingBarMsg(const char* msg1, const char* msg2) {
    if (msg1) {
        AsciiToGxtChar(msg1, m_LoadingGxtMsg1);
    } else {
        m_LoadingGxtMsg1[0] = '\0';
    }

    if (msg2) {
        AsciiToGxtChar(msg2, m_LoadingGxtMsg2);
    } else {
        m_LoadingGxtMsg2[0] = '\0';
    }
}

// 0x590280
float CLoadingScreen::GetClockTime(bool isRealTime) {
    const auto secs = (float)(GetMillisecondTime()) / 1000.0f;
    return isRealTime ? secs : secs - m_PauseTime;
}

// 0x590310
void CLoadingScreen::Pause() {
    // NOP
}

// 0x590320
void CLoadingScreen::Continue() {
    if (!IsActive()) {
        return;
    }

    m_bWantToPause = false;
    if (m_bPaused) {
        m_bPaused = false;
        m_PauseTime = GetClockTime() - m_ClockTimeOnPause + m_PauseTime;
    }
}

// 0x590370
void CLoadingScreen::RenderLoadingBar() {
#ifdef PRINT_LOADMSG
    // NOTSA
    // TODO: Fix new-line not rendered when using fastload into a savegame

    char loadingMsg[1024];
    *std::format_to(loadingMsg, "{}\n{}", m_LoadingGxtMsg1, m_LoadingGxtMsg2) = 0;
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetDropShadowPosition(2);
    CFont::SetJustify(false);
    CFont::PrintString(
        SCREEN_STRETCH_X(50.0f),
        SCREEN_STRETCH_Y(40.0f),
        loadingMsg
    );
    CFont::RenderFontBuffer();
#else
    NOTSA_LOG_DEBUG("{} -> {}", AsciiFromGxtChar(m_LoadingGxtMsg1), AsciiFromGxtChar(m_LoadingGxtMsg2));
#endif

    if (m_TimeBarAppeared == 0.0f) {
        m_TimeBarAppeared = GetClockTime();
    }

    if (m_bLegalScreen || m_PercentLoaded <= 0.0f || m_PercentLoaded >= 100.0f) {
        return;
    }

    CSprite2d::DrawBarChart(
        SCREEN_STRETCH_X(50.0f),
        SCREEN_STRETCH_FROM_BOTTOM(40.0f),
        (uint16)SCREEN_STRETCH_X(180.0f),
        (uint8)SCREEN_STRETCH_Y(10.0f),
        m_PercentLoaded,
        0,
        false,
        true,
        HudColour.GetRGBA(HUD_COLOUR_LIGHT_GRAY, 255),
        CRGBA{ 0, 0, 0, 0 }
    );
}

// 0x5904D0
void CLoadingScreen::DisplayNextSplash() {
    if (m_currDisplayedSplash == eDisplayedSplash::SPLASH6 || m_bFading) {
        return;
    }

    m_FadeAlpha = 255;

    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState2d();
        RenderSplash();
        RenderLoadingBar();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }

    m_currDisplayedSplash = (eDisplayedSplash)(+m_currDisplayedSplash + 1);
}

// 0x590530
void CLoadingScreen::StartFading() {
    m_bFading = true;
    m_FadeAlpha = 0;
    m_StartFadeTime = GetClockTime(false);
}

// inline
// 0x590570
inline void CLoadingScreen::DisplayPCScreen() {
    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState2d();
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RenderSplash();
        if (!g_FastLoaderConfig.NoLoadBar) {
            if (m_currDisplayedSplash > eDisplayedSplash::COPYRIGHT && (!m_bFading || m_currDisplayedSplash != eDisplayedSplash::SPLASH_1_NO_BAR)) {
                RenderLoadingBar();
            }
        }
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }
}

// 0x590860
void CLoadingScreen::DoPCTitleFadeIn() {
    m_bFadeInNextSplashFromBlack = true;
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT;
    m_bFading = true;

    for (auto i = 0; i < 50; i++) {
        m_FadeAlpha = static_cast<uint8>(i * 5.0f);
        DisplayPCScreen();
    }

    m_FadeAlpha = 255;
    DisplayPCScreen();
    m_bFading = false;
}

// 0x590990
void CLoadingScreen::DoPCTitleFadeOut() {
    m_bFadeInNextSplashFromBlack = true;
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT;
    m_bFading = true;

    for (auto i = 200u; i > 0; i--) {
        m_FadeAlpha = 255;
        DisplayPCScreen();
    }

    for (auto i = 50u; i > 0u; i--) {
        m_FadeAlpha = static_cast<uint8>(i * 5.0f);
        DisplayPCScreen();
    }

    m_bFadeInNextSplashFromBlack = true;
}

// 0x590AC0
// Edit in Mobile
void CLoadingScreen::DoPCScreenChange(bool isLastOne) {
    m_bFading = true;

    if (isLastOne) {
        m_bFadeOutCurrSplashToBlack = true;
    } else {
        if (notsa::IsFixBugs()) { // Fix incorrect looping behaviour
            m_currDisplayedSplash = (eDisplayedSplash)(std::max((+m_currDisplayedSplash + 1) % std::size(m_aSplashes), 1u)); // 1u = skip copyright screen
        } else {
            m_currDisplayedSplash = (eDisplayedSplash)(+m_currDisplayedSplash + 1);
        }
    }

    if (!g_FastLoaderConfig.NoFading) {
        for (auto i = 20; i > 0; i--) {
            m_FadeAlpha = 0;
            DisplayPCScreen();
        }
    }

    for (auto i = 0u; i < 50u; i++) {
        float alpha = (float)i * 5.0f;
        m_FadeAlpha = (uint8)alpha;

        if (isLastOne || m_bFadeInNextSplashFromBlack) {
            float amp = (isLastOne && m_bFadeOutCurrSplashToBlack)
                ? ((255.0f - alpha) / 25.0f)
                : 1.0f;
            AudioEngine.ServiceLoadingTune(amp);
        }

        DisplayPCScreen();
    }

    m_FadeAlpha = 255;
    DisplayPCScreen();
    m_bFadeInNextSplashFromBlack = false;
    m_bFading = false;

    if (isLastOne) {
        Shutdown();
    }
}

// 0x590D00
// Edit in Mobile
void CLoadingScreen::NewChunkLoaded() {
    if (!IsActive()) {
        return;
    }

    ++m_numChunksLoaded;

    if (m_chunkBarAppeared != -1) {
        auto loadedSinceAppeared = m_numChunksLoaded - m_chunkBarAppeared;
        auto totalChunksRemaining = 140 - m_chunkBarAppeared;

        m_PercentLoaded = (float)(loadedSinceAppeared * 100) / totalChunksRemaining;
    }

    auto now = GetClockTime();
    auto delta = now - m_timeSinceLastScreen;

    if (m_numChunksLoaded == 140) {
        return DoPCScreenChange(true);
    }

#ifdef FIX_BUGS // Fix copyright screen appearing instead of an actual loading screen splash
    if (m_currDisplayedSplash > eDisplayedSplash::COPYRIGHT && delta < g_FastLoaderConfig.ScreenChangeTime) {
#else
    if ((m_currDisplayedSplash > eDisplayedSplash::COPYRIGHT && delta < 5.0f) || (m_currDisplayedSplash == eDisplayedSplash::COPYRIGHT && delta < 5.5f)) {
#endif
        if (!g_FastLoaderConfig.NoLoadScreen || !g_FastLoaderConfig.NoLoadBar) {
            DisplayPCScreen();
        }
    } else { // New splash screen
        DoPCScreenChange(false);
        m_timeSinceLastScreen = now;

        if (m_chunkBarAppeared == -1) {
            m_chunkBarAppeared = m_numChunksLoaded;
        }
    }
}

// 0x5905E0
// unused
void CLoadingScreen::Update() {
    // Timing constants, defines?
    constexpr float LEGAL_SCREEN_TIMEOUT = 5.5f;
    constexpr float FADE_TIME_STANDARD = 0.6f;
    constexpr float FADE_TIME_LEGAL = 2.0f;

    // Calculate progress
    if (m_TimeBarAppeared > 0.0f) {
        float elapsed = GetClockTime(false) - m_TimeBarAppeared;
        float total = m_TimeBarAppeared - m_TimeStartedLoading;
        if (total > 0.0f) {
            m_PercentLoaded = std::min(elapsed / total, 1.0f);
        }
    }

    // Handle deletion queue
    if (!m_bReadyToDelete && m_bLegalScreen) {
        if (GetClockTime(false) - m_TimeStartedLoading > LEGAL_SCREEN_TIMEOUT) {
            DisplayNextSplash();
        }
    }

    if (!m_bReadyToDelete && m_bSignalDelete && !m_bFading && m_PercentLoaded >= 1.0f) {
        // 0x5906ED
        m_bReadyToDelete = true;
        m_bFadeOutCurrSplashToBlack = true;
        StartFading();
    }

    // 0x590706
    // Update fade
    if (m_bFading) {
        float elapsed = GetClockTime(false) - m_StartFadeTime;
        float duration = (m_currDisplayedSplash == eDisplayedSplash::NONE || m_bFadeInNextSplashFromBlack || m_bLegalScreen)
            ? FADE_TIME_LEGAL
            : FADE_TIME_STANDARD;

        if (elapsed > duration) {
            m_bFading = false;
            m_FadeAlpha = 255;

            if (m_bLegalScreen && m_bFadeOutCurrSplashToBlack) {
                m_bFadeOutCurrSplashToBlack = false;
                m_bLegalScreen = false;
                m_bFadeInNextSplashFromBlack = true;
                DisplayNextSplash();
            } else {
                m_bFadeInNextSplashFromBlack = false;
                m_bFadeOutCurrSplashToBlack = false;
            }
        } else {
            m_FadeAlpha = static_cast<uint8>((elapsed / duration) * 255.0f);
        }
    } else {
        m_FadeAlpha = 255;
    }

    // 0x5907D7
    // Update audio
    if (!m_bLegalScreen) {
        float volume = 1.0f;
        if (m_bFadeInNextSplashFromBlack) {
            volume = (float)m_FadeAlpha / 255.0f;
        } else if (m_bFadeOutCurrSplashToBlack) {
            volume = (255.0f - (float)m_FadeAlpha) / 255.0f;
        }
        AudioEngine.ServiceLoadingTune(volume);
    }
}

// NOTSA
void CLoadingScreen::SkipCopyrightSplash() {
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT; // Copyright splash
#ifndef FIX_BUGS // Fixed this in DoPCScreenChange
    m_timeSinceLastScreen -= 1000.f; // Decrease timeSinceLastScreen, so it will change immediately
#endif // !FIX_BUGS
    m_bFadeInNextSplashFromBlack = true; // First Loading Splash
}

// 0x53DED0
void LoadingScreen(const char* msg1, const char* msg2, const char* msg3) {
    if (msg1) {
        if (!g_FastLoaderConfig.NoDbgLogScreens) { // Very slow, so skip it
            NOTSA_LOG_DEBUG("Loadingscreen: {} [{}][{}]", msg1, msg2 ? msg2 : "NULL", msg3 ? msg3 : "NULL");
        }
        CLoadingScreen::SetLoadingBarMsg(msg1, msg2);
    }
    CLoadingScreen::NewChunkLoaded();
}
