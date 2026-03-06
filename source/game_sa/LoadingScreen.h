/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CSprite2d;
class CLoadingScreen {
public:
    static constexpr size_t MAX_SPLASHES = 7u;

    static inline auto& m_PopUpMessage = StaticRef<char[16]>(0xBAB268);
    static inline auto& m_LoadingGxtMsg1 = StaticRef<GxtChar[80]>(0xBAB2C8);
    static inline auto& m_LoadingGxtMsg2 = StaticRef<GxtChar[80]>(0xBAB278);
    static inline auto& m_aSplashes = StaticRef<std::array<CSprite2d, MAX_SPLASHES>>(0xBAB35C);

    static inline auto& m_currDisplayedSplash = StaticRef<int32>(0x8D093C);
    static inline auto& m_numChunksLoaded = StaticRef<int32>(0x8D0940); // -1
    static inline auto& m_chunkBarAppeared = StaticRef<int32>(0x8D0944); // -1

    static inline auto& m_bActive = StaticRef<bool>(0xBAB318);
    static inline auto& m_bWantToPause = StaticRef<bool>(0xBAB319);
    static inline auto& m_bPaused = StaticRef<bool>(0xBAB31A);
    static inline auto& m_bFading = StaticRef<bool>(0xBAB31C);
    static inline auto& m_bLegalScreen = StaticRef<bool>(0xBAB31D);
    static inline auto& m_bFadeInNextSplashFromBlack = StaticRef<bool>(0xBAB31E);
    static inline auto& m_bFadeOutCurrSplashToBlack = StaticRef<bool>(0xBAB31F);
    static inline auto& m_bReadyToDelete = StaticRef<bool>(0xBAB33D);

    static inline auto& m_FadeAlpha = StaticRef<uint8>(0xBAB320);
    static inline auto& m_StartFadeTime = StaticRef<float>(0xBAB324);
    static inline auto& m_ClockTimeOnPause = StaticRef<float>(0xBAB328);
    static inline auto& m_PauseTime = StaticRef<float>(0xBAB32C);

    static inline auto& gfLoadingPercentage = StaticRef<float>(0xBAB330);
    static inline auto& m_TimeBarAppeared = StaticRef<float>(0xBAB338);

    static inline auto& m_timeSinceLastScreen = StaticRef<float>(0xBAB340);

public:
    static void InjectHooks();

    static void Init(bool unusedFlag, bool loaded);
    static void Shutdown();

    static void RenderSplash();
    static void LoadSplashes(bool starting, bool nvidia);
    static void DisplayMessage(const char* message);
    static void SetLoadingBarMsg(const char* msg1, const char* msg2);
    [[nodiscard]] static float GetClockTime(bool ignorePauseTime = true);
    static void Pause();
    static void Continue();
    static void RenderLoadingBar();
    static void inline DisplayNextSplash();
    static void StartFading();
    static void inline DisplayPCScreen();
    static void DoPCTitleFadeOut();
    static void DoPCTitleFadeIn();
    static void DoPCScreenChange(uint32 finish);
    static void NewChunkLoaded();

    static void Update();

    [[nodiscard]] static bool IsActive() { return m_bActive; } // 0x744DB5
    static CSprite2d& GetCurrentDisplayedSplash() { return m_aSplashes[m_currDisplayedSplash]; }

    //! NOTSA - Skips the copyright splash
    static void SkipCopyrightSplash();
};

void LoadingScreen(const char* msg1, const char* msg2 = nullptr, const char* msg3 = nullptr);
