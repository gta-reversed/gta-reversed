/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Timer.h"

class CTaskTimer {
public:
    uint32 m_nStartTime{};
    int32  m_nInterval{};
    bool   m_bStarted{};
    bool   m_bStopped{};

public:
    static void InjectHooks();

    CTaskTimer() = default;

    // 0x420E10
    CTaskTimer(uint32 startTimeMs, uint32 interval) :
        m_nStartTime{startTimeMs},
        m_nInterval{(int32)interval},
        m_bStarted{true}
    {
    }

    bool Start(int32 interval) {
        if (interval >= 0) {
            m_nStartTime = CTimer::GetTimeInMS();
            m_nInterval = interval;
            m_bStarted = true;
            return true;
        }
        return false;
    }

    void StartIfNotAlready(int32 interval) { // NOTSA
        if (!m_bStarted) {
            m_nStartTime = CTimer::GetTimeInMS();
            m_nInterval = (int32)interval;
            m_bStarted = true;
        }
    }

    bool Pause() {
        if (m_bStarted) {
            m_bStopped = true;
            m_nInterval -= CTimer::GetTimeInMS() - m_nStartTime;
            return true;
        }
        return false;
    }

    void Stop() {
        m_bStarted = false;
    }

    /*!
    * @brief Make the timer be finished immidiately (So that `IsOutOfTime` returns true)
    */
    void SetOutOfTime() {
        m_nStartTime = CTimer::GetTimeInMS();
        m_nInterval = -1;
        m_bStarted = true;
    }

    bool Reset() {
        if (m_bStarted) {
            if (m_bStopped) {
                m_nStartTime = CTimer::GetTimeInMS();
                m_bStopped = false;
            }
            return true;
        }
        return false;
    }

    /*!
    * @brief Make it so that when `IsOutOfTime()` is called it returns true.
    */
    void SetAsOutOfTime() {
        m_nStartTime = CTimer::GetTimeInMS();
        m_nInterval = -1;
        m_bStarted = true;
    }

    bool IsStarted() const { return m_bStarted; }

    bool IsOutOfTime();
};

VALIDATE_SIZE(CTaskTimer, 0xC);
