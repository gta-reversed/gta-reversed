#include "StdInc.h"

CLoadMonitor& g_LoadMonitor = *reinterpret_cast<CLoadMonitor*>(0xB72978);

void CLoadMonitor::InjectHooks() {
    RH_ScopedClass(CLoadMonitor);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x53CFA0);
    RH_ScopedInstall(Destructor, 0x856430);

    RH_ScopedInstall(BeginFrame, 0x53D030);
    RH_ScopedInstall(EndFrame, 0x53D0B0);
    RH_ScopedInstall(StartTimer, 0x53D050);
    RH_ScopedInstall(EndTimer, 0x53D070);
    RH_ScopedInstall(Render, 0x53D480);
}

// 0x53CFA0
CLoadMonitor::CLoadMonitor() {
    m_bInFrame            = 0;
    m_bUseLoadMonitor     = 1;
    m_eProcLevel          = eProcessingLevel::OK;
    m_LastTime            = CTimer::GetTimeInMS();
    m_NumFramesThisSec    = 0;
    m_FPS                 = 0;
    m_bEnableAmbientCrime = true;

    rng::fill(m_iMaxCycles, 0);

    rng::fill(m_fAveragedCyclesThisSecond, 0.f);

    m_fPeakLevels[+eLoadType::PED_AI]                 = 15.0f;
    m_fPeakLevels[+eLoadType::COLLISION]              = 20.0f;
    m_fPeakLevels[+eLoadType::NUM_STREAMING_REQUESTS] = 10.0f;
    m_fPeakLevels[+eLoadType::MOVE_SPEED]             = 50.0f;

    m_DisplayType = eLoadMonitorDisplay::NONE;
    m_VarConsoleDisplayType = eLoadMonitorDisplay::NONE;
}

// 0x53D030
void CLoadMonitor::BeginFrame() {
    rng::fill(m_iCyclesThisFrame, 0);
    m_bInFrame = 1;
}

// 0x53D0B0
void CLoadMonitor::EndFrame() {
    if (auto* ped = FindPlayerPed()) {
        const auto movevec = ped->IsInVehicle()
            ? ped->GetVehicleIfInOne()->GetMoveSpeed()
            : ped->GetMoveSpeed();

        m_iCyclesThisFrame[+eLoadType::MOVE_SPEED] = (uint32)(movevec.Magnitude() * 50.f);
    }

    // 0x53D143 - 0x53D1D3
    for (auto t = 0u; t < +eLoadType::NUM_LOAD_TYPES; t++) {
        if (m_iCyclesThisFrame[t] > m_iMaxCycles[t]) {
            m_iMaxCycles[t] = m_iCyclesThisFrame[t];
        }

        m_fAveragedCyclesThisSecond[t] += (float)m_iCyclesThisFrame[t];
    }

    auto mstime = CTimer::GetTimeInMS();
    auto delta  = mstime - m_LastTime;

    if (delta > 2'000) {
        m_LastTime = mstime;
        m_NumFramesThisSec = 0;
        return;
    }

    if (delta >= 1'000) {
        m_FPS = m_NumFramesThisSec;
        m_LastTime = mstime;

        if (m_NumFramesThisSec == 0) {
            m_NumFramesThisSec = 1;
        }

        const float recip = 1.f / (float)m_NumFramesThisSec;
        for (auto& avg : m_fAveragedCyclesThisSecond) {
            avg *= recip;
        }

        m_NumFramesThisSec = 0;

        for (auto t = 0u; t < +eLoadType::NUM_LOAD_TYPES; t++) {
            for (auto i = 7u; i > 0; i--) {
                m_iCyclesHistory[t][i] = m_iCyclesHistory[t][i - 1];
            }

            m_iCyclesHistory[t][0] = (uint32)m_fAveragedCyclesThisSecond[t];
            m_fAveragedCyclesThisSecond[t] = 0.f;
        }

        m_eProcLevel = eProcessingLevel::OK;

        const auto needsCyclesConversion = [](uint32 t) {
            return t != +eLoadType::NUM_STREAMING_REQUESTS
                && t != +eLoadType::MOVE_SPEED;
        };

        for (auto t = 0u; t < +eLoadType::NUM_LOAD_TYPES; t++) {
            float smoothed{};
            if (m_iMaxCycles[t] != 0) {
                float sum{};
                for (auto a = 0u; a < 8u; a++) {
                    sum += (float)m_iCyclesHistory[t][a];
                }

                smoothed = std::fminf(sum / 8.f, (float)m_iMaxCycles[t]);
            }
            m_fSmoothedValues[t] = smoothed;

            const float val = needsCyclesConversion(t) ? smoothed / (float)CTimer::GetCyclesPerMillisecond() : smoothed;
            const float normalized = val / m_fPeakLevels[t];

            if (normalized > (2.f / 3.f) && m_eProcLevel < eProcessingLevel::HIGH) {
                m_eProcLevel = eProcessingLevel::HIGH;
            } else if (normalized > (1.f / 3.f) && m_eProcLevel < eProcessingLevel::MED) {
                m_eProcLevel = eProcessingLevel::MED;
            }

            m_fNormalizedPeakRangeValues[t] = normalized;
            m_fSmoothedValues[t] = smoothed / (float)m_iMaxCycles[t];
        }
    } else {
        m_NumFramesThisSec++;
    }

    if (IsForcingProcLevel()) {
        m_eProcLevel = m_eProcLevelToForce;
    }
}

// 0x53D050
void CLoadMonitor::StartTimer(eLoadType timerIndex) {
    m_iStartTimes[+timerIndex] = CTimer::GetCurrentTimeInCycles();
}

// 0x53D070
void CLoadMonitor::EndTimer(eLoadType timerIndex) {
    m_iCyclesThisFrame[+timerIndex] += CTimer::GetCurrentTimeInCycles() - m_iStartTimes[+timerIndex];
}

// debug function?
// 0x53D480
void CLoadMonitor::Render() {
    // NOP
}
