/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

class CTimer {
    /*!
    * Thanks guys for figuring this out for me!
    * 
    * So basically, a timestep is just fraction of the frametime (timestep = frametime / TIMESTEP_PER_SECOND)
    * this timestep is used basically everywhere to calculate physics, etc.
    */
    static constexpr float TIMESTEP_PER_SECOND = 50.f;                         //!< Number of steps/second
    static constexpr float TIMESTEP_LEN_IN_MS  = 1000.f / TIMESTEP_PER_SECOND; //!< How long (in ms) a timestep is

public:
    typedef uint64(__cdecl* TimerFunction_t)();
    static TimerFunction_t& ms_fnTimerFunction;

    // class variables
    static bool& m_sbEnableTimeDebug;
    static bool& bSkipProcessThisFrame;
    static bool& bSlowMotionActive;
    static float& game_FPS;

    static bool& m_CodePause;
    static bool& m_UserPause;
    static uint32& m_FrameCounter;
    static float& ms_fTimeStepNonClipped;
    static float& ms_fTimeStep;
    static uint32& m_snTimerDivider;

    static float& ms_fOldTimeStep;
    static float& ms_fSlowMotionScale;

    // game speed
    static float& ms_fTimeScale;
    static uint32& m_snTimeInMillisecondsPauseMode;
    static uint32& m_snTimeInMillisecondsNonClipped;
    static uint32& m_snPreviousTimeInMillisecondsNonClipped;
    static uint32& m_snTimeInMilliseconds;
    static uint64& m_snRenderStartTime;
    static uint64& m_snRenderPauseTime;
    static uint32& m_snRenderTimerPauseCount;

    // Freshly from R*:
    static uint32& m_snPPPPreviousTimeInMilliseconds;
    static uint32& m_snPPPreviousTimeInMilliseconds;
    static uint32& m_snPPreviousTimeInMilliseconds;
    static uint32& m_snPreviousTimeInMilliseconds;

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
