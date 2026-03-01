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

    static inline char (&m_PopUpMessage)[16] = *(char (*)[16])0xBAB268;
    static inline GxtChar (&m_LoadingGxtMsg1)[80] = *(GxtChar (*)[80])0xBAB2C8;
    static inline GxtChar (&m_LoadingGxtMsg2)[80] = *(GxtChar (*)[80])0xBAB278;
    static inline std::array<CSprite2d, MAX_SPLASHES>& m_aSplashes = *(std::array<CSprite2d, MAX_SPLASHES>*)0xBAB35C;

    static inline int32& m_currDisplayedSplash = *(int32*)0x8D093C;
    static inline int32& m_numChunksLoaded = *(int32*)0x8D0940; // -1
    static inline int32& m_chunkBarAppeared = *(int32*)0x8D0944; // -1

    static inline bool& m_bActive = *(bool*)0xBAB318;
    static inline bool& m_bWantToPause = *(bool*)0xBAB319;
    static inline bool& m_bPaused = *(bool*)0xBAB31A;
    static inline bool& m_bFading = *(bool*)0xBAB31C;
    static inline bool& m_bLegalScreen = *(bool*)0xBAB31D;
    static inline bool& m_bFadeInNextSplashFromBlack = *(bool*)0xBAB31E;
    static inline bool& m_bFadeOutCurrSplashToBlack = *(bool*)0xBAB31F;
    static inline bool& m_bReadyToDelete = *(bool*)0xBAB33D;

    static inline uint8& m_FadeAlpha = *(uint8*)0xBAB320;
    static inline float& m_StartFadeTime = *(float*)0xBAB324;
    static inline float& m_ClockTimeOnPause = *(float*)0xBAB328;
    static inline float& m_PauseTime = *(float*)0xBAB32C;

    static inline float& gfLoadingPercentage = *(float*)0xBAB330;
    static inline float& m_TimeBarAppeared = *(float*)0xBAB338;

    static inline float& m_timeSinceLastScreen = *(float*)0xBAB340;

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
