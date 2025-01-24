/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "oswrapper.h"

CTimer::TimerFunction_t& CTimer::ms_fnTimerFunction = *(TimerFunction_t*)0xB7CB28; // Izzotop: Added in commit 50ce121c. That should be just timerDef.

bool& CTimer::m_sbEnableTimeDebug = *(bool*)0xB7CB40;
bool& CTimer::bSkipProcessThisFrame = *(bool*)0xB7CB89;
bool& CTimer::bSlowMotionActive = *(bool*)0xB7CB88;
float& CTimer::game_FPS = *(float*)0xB7CB50;

bool& CTimer::m_CodePause = *(bool*)0xB7CB48;
bool& CTimer::m_UserPause = *(bool*)0xB7CB49;

uint32& CTimer::m_FrameCounter = *(uint32*)0xB7CB4C;
uint32& CTimer::m_snTimerDivider = *(uint32*)0xB7CB2C;

float& CTimer::ms_fTimeStep = *(float*)0xB7CB5C;
float& CTimer::ms_fTimeStepNonClipped = *(float*)0xB7CB58;
float& CTimer::ms_fOldTimeStep = *(float*)0xB7CB54;

float& CTimer::ms_fTimeScale = *(float*)0xB7CB64;
float& CTimer::ms_fSlowMotionScale = *(float*)0xB7CB60;

uint32& CTimer::m_snTimeInMillisecondsPauseMode = *(uint32*)0xB7CB7C;
uint32& CTimer::m_snTimeInMillisecondsNonClipped = *(uint32*)0xB7CB80;
uint32& CTimer::m_snTimeInMilliseconds = *(uint32*)0xB7CB84;
uint32& CTimer::m_snPreviousTimeInMillisecondsNonClipped = *(uint32*)0xB7CB68;

uint64& CTimer::m_snRenderStartTime = *(uint64*)0xB7CB38;
uint64& CTimer::m_snRenderPauseTime = *(uint64*)0xB7CB30;
uint32& CTimer::m_snRenderTimerPauseCount = *(uint32*)0xB7CB44;

uint32& CTimer::m_snPPPPreviousTimeInMilliseconds = *(uint32*)0xB7CB6C;
uint32& CTimer::m_snPPPreviousTimeInMilliseconds = *(uint32*)0xB7CB70;
uint32& CTimer::m_snPPreviousTimeInMilliseconds = *(uint32*)0xB7CB74;
uint32& CTimer::m_snPreviousTimeInMilliseconds = *(uint32*)0xB7CB78;


void CTimer::InjectHooks()
{
    RH_ScopedClass(CTimer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5617E0);
    RH_ScopedInstall(Shutdown, 0x5618C0);
    RH_ScopedInstall(Suspend, 0x5619D0);
    RH_ScopedInstall(Resume, 0x561A00);
    RH_ScopedInstall(Stop, 0x561AA0);
    RH_ScopedInstall(StartUserPause, 0x561AF0);
    RH_ScopedInstall(EndUserPause, 0x561B00);
    RH_ScopedInstall(GetCyclesPerMillisecond, 0x561A40);
    RH_ScopedInstall(GetCyclesPerFrame, 0x561A50);
    RH_ScopedInstall(GetCurrentTimeInCycles, 0x561A80);
    RH_ScopedInstall(GetIsSlowMotionActive, 0x561AD0);
    RH_ScopedInstall(UpdateVariables, 0x5618D0);
    RH_ScopedInstall(Update, 0x561B10);
}

// 64-bit RsTimer wrapper
// 0x5617C0
uint64 GetMillisecondTime() {
    return plugin::CallAndReturn<uint64, 0x5617C0>();
    // return RsTimer();
}

// 0x5617E0
void CTimer::Initialise()
{
    m_UserPause = false;
    m_CodePause = false;
    bSlowMotionActive = false;
    bSkipProcessThisFrame = false;

    m_snTimeInMilliseconds = 0;
    m_snTimeInMillisecondsPauseMode = 1;
    m_snTimeInMillisecondsNonClipped = 1;
    m_snPreviousTimeInMilliseconds = 0;
    m_snPPreviousTimeInMilliseconds = 0;
    m_snPPPreviousTimeInMilliseconds = 0;
    m_snPPPPreviousTimeInMilliseconds = 0;
    m_snPreviousTimeInMillisecondsNonClipped = 0;

    m_snRenderTimerPauseCount = 0;

    m_FrameCounter = 0;
    m_sbEnableTimeDebug = false;
    game_FPS = 0.0f;

    ms_fTimeScale = 1.0f;
    ms_fSlowMotionScale = -1.0f; // unused
    ms_fTimeStep = 1.0f;
    ms_fOldTimeStep = 1.0f;

    TimerFunction_t timerFunc;
    auto frequency = GetOSWPerformanceFrequency();
    if (frequency) {
        timerFunc = GetOSWPerformanceTime;
        m_snTimerDivider = (uint32)(frequency / 1000);
    } else {
        timerFunc = GetMillisecondTime;
        m_snTimerDivider = 1;
    }
    ms_fnTimerFunction = timerFunc;
    m_snRenderStartTime = timerFunc();
}

// 0x5618C0
void CTimer::Shutdown() {
    m_sbEnableTimeDebug = false;
}

// 0x5619D0
void CTimer::Suspend()
{
    if (m_sbEnableTimeDebug)
    {
        if (++m_snRenderTimerPauseCount <= 1)
            m_snRenderPauseTime = ms_fnTimerFunction();
    }
}

// 0x561A00
void CTimer::Resume()
{
    if (m_sbEnableTimeDebug)
    {
        if (!--m_snRenderTimerPauseCount) {
            m_snRenderStartTime = ms_fnTimerFunction() - m_snRenderPauseTime + m_snRenderStartTime;
        }
    }
}

// 0x561AA0
void CTimer::Stop()
{
    m_snPPPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;
    m_sbEnableTimeDebug = false;
    m_snPreviousTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped;
}

// 0x561AF0
void CTimer::StartUserPause()
{
    m_UserPause = true;
}

// 0x561B00
void CTimer::EndUserPause()
{
    m_UserPause = false;
}

// 0x561A40
uint32 CTimer::GetCyclesPerMillisecond()
{
    return m_snTimerDivider;
}

// cycles per ms * 20
// 0x561A50
uint32 CTimer::GetCyclesPerFrame()
{
    return (uint32)((float)m_snTimerDivider * 20.0f);
}

// 0x561A80
uint64 CTimer::GetCurrentTimeInCycles()
{
    return GetOSWPerformanceTime() - m_snRenderStartTime;
}

// 0x561AD0
bool CTimer::GetIsSlowMotionActive()
{
    return ms_fTimeScale < 1.0f;
}

// 0x5618D0
void CTimer::UpdateVariables(float timeElapsed)
{
    /* Izzotop: from IDA directly to here (tested)
    float step = timeStep / float(m_snTimerDivider);
    m_snTimeInMillisecondsNonClipped += (uint32)(step);
    ms_fTimeStepNonClipped = step * 0.05f; // step / 20.0f;

    if (step > 300.f) {
        step = 300.f;
    }
    m_snTimeInMilliseconds += (uint32)(step);

    if (ms_fTimeStepNonClipped < 0.01f &&
        !GetIsPaused() &&
        !CSpecialFX::bSnapShotActive
    ) {
        ms_fTimeStepNonClipped = 0.01f;
    }

    ms_fOldTimeStep = ms_fTimeStep;

    if (ms_fTimeStepNonClipped > 3.0f) {
        ms_fTimeStep = 3.0f;
    } else if (ms_fTimeStepNonClipped > 0.00001f) {
        ms_fTimeStep = ms_fTimeStepNonClipped;
    } else {
        ms_fTimeStep = 0.00001f;
    }
    */

    // Pirulax: Shorter code, same functionality.
    const float frameDelta = (float)timeElapsed / (float)m_snTimerDivider;
    m_snTimeInMillisecondsNonClipped += (uint32)frameDelta;
    ms_fTimeStepNonClipped = frameDelta / TIMESTEP_LEN_IN_MS;

    m_snTimeInMilliseconds += (uint32)std::min<float>(frameDelta, 300.0f);

    if (!m_UserPause && !m_CodePause && !CSpecialFX::bSnapShotActive) {
        // Make it be something at least, to avoid division by 0
        ms_fTimeStepNonClipped = std::max(ms_fTimeStepNonClipped, 0.01f); 
    }

    ms_fOldTimeStep = ms_fTimeStep;
    SetTimeStep(std::clamp(ms_fTimeStepNonClipped, 0.00001f, 3.0f));
}

// 0x561B10
void CTimer::Update() {
    ZoneScoped;

    if (!ms_fnTimerFunction)
        return;

    m_sbEnableTimeDebug = true;
    game_FPS = float(1000.0f / float(m_snTimeInMillisecondsNonClipped - m_snPreviousTimeInMillisecondsNonClipped));

    // Update history
    m_snPPPPreviousTimeInMilliseconds = m_snPPPreviousTimeInMilliseconds;
    m_snPPPreviousTimeInMilliseconds = m_snPPreviousTimeInMilliseconds;
    m_snPPreviousTimeInMilliseconds = m_snPreviousTimeInMilliseconds;
    m_snPreviousTimeInMilliseconds = m_snTimeInMilliseconds;

    m_snPreviousTimeInMillisecondsNonClipped = m_snTimeInMillisecondsNonClipped;

    const uint64 nRenderTimeBefore = m_snRenderStartTime;
    m_snRenderStartTime = ms_fnTimerFunction();
    auto fTimeDelta = float(m_snRenderStartTime - nRenderTimeBefore);
    if (!GetIsPaused())
        fTimeDelta *= ms_fTimeScale;

    m_snTimeInMillisecondsPauseMode += (uint32)(fTimeDelta / float(m_snTimerDivider));
    if (GetIsPaused())
        fTimeDelta = 0.0f;

    UpdateVariables(fTimeDelta);
    m_FrameCounter++;
}
