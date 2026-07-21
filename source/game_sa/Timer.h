/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CTimer {
public:
    /*!
    * Thanks guys for figuring this out for me!
    * 
    * So basically, a timestep is just fraction of the frametime (timestep = frametime / TIMESTEP_PER_SECOND)
    * this timestep is used basically everywhere to calculate physics, etc.
    */
    static constexpr float TIMESTEP_PER_SECOND = 50.f;                         //!< Number of steps/second
    static constexpr float TIMESTEP_LEN_IN_MS  = 1000.f / TIMESTEP_PER_SECOND; //!< How long (in ms) a timestep is

    typedef uint64(__cdecl* TimerFunction_t)();
    static inline auto& ms_fnTimerFunction = StaticRef<TimerFunction_t>(0xB7CB28); // Izzotop: Added in commit 50ce121c. That should be just timerDef.

    // class variables
    static inline auto& m_sbEnableTimeDebug = StaticRef<bool>(0xB7CB40);
    static inline auto& bSkipProcessThisFrame = StaticRef<bool>(0xB7CB89);
    static inline auto& bSlowMotionActive = StaticRef<bool>(0xB7CB88);
    static inline auto& game_FPS = StaticRef<float>(0xB7CB50);

    static inline auto& m_CodePause = StaticRef<bool>(0xB7CB48);
    static inline auto& m_UserPause = StaticRef<bool>(0xB7CB49);
    static inline auto& m_FrameCounter = StaticRef<uint32>(0xB7CB4C);
    static inline auto& ms_fTimeStepNonClipped = StaticRef<float>(0xB7CB58);
    static inline auto& ms_fTimeStep = StaticRef<float>(0xB7CB5C);
    static inline auto& m_snTimerDivider = StaticRef<uint32>(0xB7CB2C);

    static inline auto& ms_fOldTimeStep = StaticRef<float>(0xB7CB54);
    static inline auto& ms_fSlowMotionScale = StaticRef<float>(0xB7CB60);

    // game speed
    static inline auto& ms_fTimeScale = StaticRef<float>(0xB7CB64);
    static inline auto& m_snTimeInMillisecondsPauseMode = StaticRef<uint32>(0xB7CB7C);
    static inline auto& m_snTimeInMillisecondsNonClipped = StaticRef<uint32>(0xB7CB80);
    static inline auto& m_snPreviousTimeInMillisecondsNonClipped = StaticRef<uint32>(0xB7CB68);
    static inline auto& m_snTimeInMilliseconds = StaticRef<uint32>(0xB7CB84);
    static inline auto& m_snRenderStartTime = StaticRef<uint64>(0xB7CB38);
    static inline auto& m_snRenderPauseTime = StaticRef<uint64>(0xB7CB30);
    static inline auto& m_snRenderTimerPauseCount = StaticRef<uint32>(0xB7CB44);

    // Freshly from R*:
    static inline auto& m_snPPPPreviousTimeInMilliseconds = StaticRef<uint32>(0xB7CB6C);
    static inline auto& m_snPPPreviousTimeInMilliseconds = StaticRef<uint32>(0xB7CB70);
    static inline auto& m_snPPreviousTimeInMilliseconds = StaticRef<uint32>(0xB7CB74);
    static inline auto& m_snPreviousTimeInMilliseconds = StaticRef<uint32>(0xB7CB78);

public:
    static void InjectHooks();

    static void   Initialise();
    static void   Shutdown();
    static void   Suspend();
    static void   Resume();
    static void   Stop();
    static void   StartUserPause();
    static void   EndUserPause();
    static uint32 GetCyclesPerMillisecond();
    static uint32 GetCyclesPerFrame();
    static uint32 GetCurrentTimeInCycles();
    static bool   GetIsSlowMotionActive();
    static void   UpdateVariables(float timeElapsed);
    static void   Update();

    static float GetTimestepPerSecond() { return TIMESTEP_PER_SECOND; }

    // Inlined funcs
    // They could have used functions with a longer name, ex:
    // GetTimeInMillisecond, we have shorter GetTimeInMS
    static float  GetTimeScale() { return ms_fTimeScale; }
    static void   SetTimeScale(float ts) { ms_fTimeScale = ts; }
    static void   ResetTimeScale() { ms_fTimeScale = 1.0f; }

    static float  GetTimeStep() { return ms_fTimeStep; }
    static void   SetTimeStep(float ts) { ms_fTimeStep = ts; }
    static void   UpdateTimeStep(float ts) { ms_fTimeStep = std::max(ts, 0.00001f); }
    static float  GetTimeStepInSeconds() { return ms_fTimeStep / TIMESTEP_PER_SECOND; }
    static float  GetTimeStepInMS() { return GetTimeStepInSeconds() * 1000.0f; } // pattern: CTimer::ms_fTimeStep * 0.02f * 1000.0f

    static float  GetTimeStepNonClipped() { return ms_fTimeStepNonClipped; }
    static float  GetTimeStepNonClippedInSeconds() { return ms_fTimeStepNonClipped / 50.0f; }
    static float  GetTimeStepNonClippedInMS() { return GetTimeStepNonClippedInSeconds() * 1000.0f; }
    static void   SetTimeStepNonClipped(float ts) { ms_fTimeStepNonClipped = ts; }

    static uint32 GetFrameCounter() { return m_FrameCounter; }
    static void   SetFrameCounter(uint32 fc) { m_FrameCounter = fc; }

    static uint32 GetTimeInMS() { return m_snTimeInMilliseconds; }
    static void   SetTimeInMS(uint32 t) { m_snTimeInMilliseconds = t; }

    static uint32 GetTimeInMSNonClipped() { return m_snTimeInMillisecondsNonClipped; }
    static void   SetTimeInMSNonClipped(uint32 t) { m_snTimeInMillisecondsNonClipped = t; }

    static uint32 GetTimeInMSPauseMode() { return m_snTimeInMillisecondsPauseMode; }
    static void   SetTimeInMSPauseMode(uint32 t) { m_snTimeInMillisecondsPauseMode = t; }

    static uint32 GetPreviousTimeInMS() { return m_snPreviousTimeInMilliseconds; }
    static void   SetPreviousTimeInMS(uint32 t) { m_snPreviousTimeInMilliseconds = t; }

    static bool GetIsPaused() { return m_UserPause || m_CodePause; }
    static bool GetIsUserPaused() { return m_UserPause; }
    static bool GetIsCodePaused() { return m_CodePause; }
    static void SetCodePause(bool pause) { m_CodePause = pause; }

    // NOTSA section

    static bool HasTimePointPassed(uint32 timeMs) { return GetTimeInMS() >= timeMs; }
    static bool IsTimeInRange(uint32 fromMs, uint32 toMs) { return HasTimePointPassed(fromMs) && !HasTimePointPassed(toMs); }
};

uint64 GetMillisecondTime();
bool EachFrames(auto count) { return (CTimer::GetFrameCounter() & count) == 0; }
