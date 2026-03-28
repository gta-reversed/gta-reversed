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

    std::array<uint32, (size_t)eLoadType::NUM_LOAD_TYPES> m_iStartTimes; // CTimer::GetCurrentTimeInCycles
    std::array<uint32, (size_t)eLoadType::NUM_LOAD_TYPES> m_iCyclesThisFrame;
    std::array<uint32, (size_t)eLoadType::NUM_LOAD_TYPES> m_iMaxCycles;

    std::array<float, (size_t)eLoadType::NUM_LOAD_TYPES> m_fSmoothedValues; // unused

    std::array<float, (size_t)eLoadType::NUM_LOAD_TYPES> m_fPeakLevels;

    std::array<float, (size_t)eLoadType::NUM_LOAD_TYPES> m_fNormalizedPeakRangeValues; // unused

    std::array<float, (size_t)eLoadType::NUM_LOAD_TYPES> m_fAveragedCyclesThisSecond; // unused

    notsa::mdarray<uint32, (size_t)eLoadType::NUM_LOAD_TYPES, 8> m_iCyclesHistory;

    notsa::mdarray<uint8, (size_t)eLoadType::NUM_LOAD_TYPES, 100> m_GraphPoints; // unused
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
    void ForceProcLevel(eProcessingLevel v) { // unused
        m_bForceProcLevel = v;
    }

    bool IsAmbientCrimeEnabled() { return m_bEnableAmbientCrime; }
    void EnableAmbientCrime() { m_bEnableAmbientCrime = true; }
    void DisableAmbientCrime() { m_bEnableAmbientCrime = false; }

    void SetTimeForThisFrame(eLoadType t, uint32 v) {
        m_iCyclesThisFrame[+t] = v;
    }

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

public:
    // Getters for debug module
    uint32 GetInFrame() const { return m_bInFrame; }
    uint32 GetUseLoadMonitor() const { return m_bUseLoadMonitor; }
    eProcessingLevel GetForceProcLevel() const { return m_bForceProcLevel; }
    eLoadMonitorDisplay GetDisplayType() const { return m_DisplayType; }
    eLoadMonitorDisplay GetVarConsoleDisplayType() const { return m_VarConsoleDisplayType; }
    eProcessingLevel GetProcLevelToForce() const { return m_eProcLevelToForce; }
    
    uint32 GetLastTime() const { return m_LastTime; }
    uint32 GetNumFramesThisSec() const { return m_NumFramesThisSec; }
    uint32 GetFPS() const { return m_FPS; }
    
    uint32 GetStartTime(eLoadType t) const { return m_iStartTimes[+t]; }
    uint32 GetCyclesThisFrame(eLoadType t) const { return m_iCyclesThisFrame[+t]; }
    uint32 GetMaxCycles(eLoadType t) const { return m_iMaxCycles[+t]; }
    
    float GetSmoothedValue(eLoadType t) const { return m_fSmoothedValues[+t]; }
    float GetPeakLevel(eLoadType t) const { return m_fPeakLevels[+t]; }
    float GetNormalizedPeakRangeValue(eLoadType t) const { return m_fNormalizedPeakRangeValues[+t]; }
    float GetAveragedCyclesThisSecond(eLoadType t) const { return m_fAveragedCyclesThisSecond[+t]; }
    
    uint32 GetCyclesHistory(eLoadType t, size_t idx) const { return m_iCyclesHistory[+t][idx]; }
    uint8 GetGraphPoint(eLoadType t, size_t idx) const { return m_GraphPoints[+t][idx]; }
    int32 GetCurrentGraphIndex() const { return m_iCurrentGraphIndex; }
};

VALIDATE_SIZE(CLoadMonitor, 0x2B0);

extern CLoadMonitor& g_LoadMonitor;
