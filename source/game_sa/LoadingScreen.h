/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once


enum class eLoadingLogo : uint8 {
    EAX = 0,
    NVIDIA = 1
};

enum class eDisplayedSplash : int32 {
    NONE = -1,

    COPYRIGHT = 0,
    SPLASH_1_NO_BAR = 1,
    SPLASH2,
    SPLASH3,
    SPLASH4,
    SPLASH5,
    SPLASH6,

    COUNT
};

class CSprite2d;

class CLoadingScreen {
private:
    static inline auto& m_bActive = StaticRef<bool>(0xBAB318);
    static inline auto& m_bWantToPause = StaticRef<bool>(0xBAB319); // unused
    static inline auto& m_bPaused = StaticRef<bool>(0xBAB31A); // unused
    static bool m_bForceShutdown; // unknown, unused
    static inline auto& m_aSplashes = StaticRef<std::array<CSprite2d, +eDisplayedSplash::COUNT>>(0xBAB35C);
    static inline auto& m_currDisplayedSplash = StaticRef<eDisplayedSplash>(0x8D093C);

    static inline auto& m_bFading = StaticRef<bool>(0xBAB31C);
    static inline auto& m_bLegalScreen = StaticRef<bool>(0xBAB31D);
    static inline auto& m_bFadeInNextSplashFromBlack = StaticRef<bool>(0xBAB31E);
    static inline auto& m_bFadeOutCurrSplashToBlack = StaticRef<bool>(0xBAB31F);
    static inline auto& m_FadeAlpha = StaticRef<uint8>(0xBAB320);
    static inline auto& m_StartFadeTime = StaticRef<float>(0xBAB324);
    static inline auto& m_ClockTimeOnPause = StaticRef<float>(0xBAB328); // unused
    static inline auto& m_PauseTime = StaticRef<float>(0xBAB32C);

    static inline auto& m_PercentLoaded = StaticRef<float>(0xBAB330);
    static inline auto& m_TimeStartedLoading = StaticRef<float>(0xBAB334); // unused
    static inline auto& m_TimeBarAppeared = StaticRef<float>(0xBAB338);

    static inline auto& m_LoadingGxtMsg1 = StaticRef<GxtChar[80]>(0xBAB2C8);
    static inline auto& m_LoadingGxtMsg2 = StaticRef<GxtChar[80]>(0xBAB278);

    static inline auto& m_PopUpMessage = StaticRef<char[16]>(0xBAB268);

    static inline auto& m_bSignalDelete = StaticRef<bool>(0xBAB33C); // unused
    static inline auto& m_bReadyToDelete = StaticRef<bool>(0xBAB33D);

    static inline auto& m_numChunksLoaded = StaticRef<int32>(0x8D0940); // -1
    // static int32 m_nChunksToLoad; // Added in Mobile
    static inline auto& m_chunkBarAppeared = StaticRef<int32>(0x8D0944); // -1
    static inline auto& m_timeSinceLastScreen = StaticRef<float>(0xBAB340);

public:
    static void LoadSplashes(bool useSplashId, eLoadingLogo id);

    [[nodiscard]] static bool IsActive() { return m_bActive; } // 0x744DB5
    [[nodiscard]] static bool IsPaused() { return m_bPaused; }

    static void Init(bool legalScreen, bool dontReload);
    static void Shutdown(bool force = false);

    // static bool Paused(); // unknown
    static void Pause();
    static void Continue();

    static void SetLoadingBarMsg(const char* msg, const char* msg2);

    static void DisplayMessage(const char* msg);
    static void DisplayNextSplash();
    static void RenderSplash();
    static void NewChunkLoaded();

    // static void Pump(); // Added in Mobile

    // static void SetChunksToLoad(int32 num, bool AddTo); // Added in Mobile

    static void DoPCTitleFadeOut();
    static void DoPCTitleFadeIn();
    static void DoPCScreenChange(bool lastOne);

private:
    static void DisplayPCScreen();

    static void RenderLoadingBar();

    static void Update();

    static void StartFading();

    [[nodiscard]] static float GetClockTime(bool realTime = true);

private: // NOTSA
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    static CSprite2d& GetCurrentDisplayedSplash() { return m_aSplashes[+m_currDisplayedSplash]; }

    // Skips the copyright splash
    static void SkipCopyrightSplash();
};

void LoadingScreen(const char* msg1, const char* msg2 = nullptr, const char* msg3 = nullptr);
