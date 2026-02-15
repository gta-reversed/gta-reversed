#pragma once

enum class eLoadType : uint32 {
    PED_AI = 0,
    COLLISION,
    NUM_STREAMING_REQUESTS,
    MOVE_SPEED,

    NUM_LOAD_TYPES,

    TOP_INDEX_USED_FOR_PROCESSOR_USAGE = 3
};

enum class eProcessingLevel : uint32 {
    OK = 0,
    MED, // MEDIUM
    HIGH
};

enum class eLoadMonitorDisplay : uint32 {
    NONE = 0,
    COMPACT,
    COMPACT_PLUS_GRAPH
};

class CLoadMonitor {
private:
    uint32           m_bInFrame; // unused
    uint32           m_bUseLoadMonitor; // unused
    eProcessingLevel m_bForceProcLevel; // unused

    eLoadMonitorDisplay m_DisplayType; // unused
    eLoadMonitorDisplay m_VarConsoleDisplayType; // unused

    eProcessingLevel m_eProcLevel; // unused

    eProcessingLevel m_eProcLevelToForce; // unused

    bool m_bEnableAmbientCrime;

    uint32 m_LastTime; // CTimer::GetTimeInMS
    uint32 m_NumFramesThisSec;
    uint32 m_FPS; // unused

    uint32 m_iStartTimes[+eLoadType::NUM_LOAD_TYPES]; // CTimer::GetCurrentTimeInCycles
    uint32 m_iCyclesThisFrame[+eLoadType::NUM_LOAD_TYPES];
    uint32 m_iMaxCycles[+eLoadType::NUM_LOAD_TYPES];

    float m_fSmoothedValues[+eLoadType::NUM_LOAD_TYPES]; // unused

    float m_fPeakLevels[+eLoadType::NUM_LOAD_TYPES];

    float m_fNormalizedPeakRangeValues[+eLoadType::NUM_LOAD_TYPES]; // unused

    float m_fAveragedCyclesThisSecond[+eLoadType::NUM_LOAD_TYPES]; // unused

    uint32 m_iCyclesHistory[+eLoadType::NUM_LOAD_TYPES][8];

    uint8 m_GraphPoints[+eLoadType::NUM_LOAD_TYPES][100]; // unused
    int32 m_iCurrentGraphIndex; // unused

public:
    CLoadMonitor();
    ~CLoadMonitor() = default; // 0x53D020

    void BeginFrame();
    void EndFrame();

    void StartTimer(eLoadType timerIndex);
    void EndTimer(eLoadType timerIndex);

    void Render();

    eProcessingLevel GetProcLevel() { return m_eProcLevel; }

    // bool GetInUse();       // unknown, unused
    // void SetInUse(bool b); // unknown, unused

    bool IsForcingProcLevel() { return m_bForceProcLevel != eProcessingLevel::OK; } // unknown, unused
    void StopForcingProcLevel() { m_bForceProcLevel = eProcessingLevel::OK; } // unknown, unused

    void ForceProcLevel(eProcessingLevel v) { m_bForceProcLevel = v; } // unused

    bool IsAmbientCrimeEnabled() { return m_bEnableAmbientCrime; }

    void EnableAmbientCrime() { m_bEnableAmbientCrime = true; }
    void DisableAmbientCrime() { m_bEnableAmbientCrime = false; }

    void SetTimeForThisFrame(eLoadType t, uint32 v) { m_iCyclesThisFrame[+t] = v; }

private: // NOTSA:
    friend void InjectHooksMain();
    static void InjectHooks();

    CLoadMonitor* Constructor() {
        this->CLoadMonitor::CLoadMonitor();
        return this;
    }

    CLoadMonitor* Destructor() {
        this->CLoadMonitor::~CLoadMonitor();
        return this;
    }
};

VALIDATE_SIZE(CLoadMonitor, 0x2B0);

extern CLoadMonitor& g_LoadMonitor;
