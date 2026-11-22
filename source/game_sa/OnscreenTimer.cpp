#include "StdInc.h"

#include "OnscreenTimer.h"
#include "OnscreenTimerEntry.h"
#include "Hud.h"

void COnscreenTimer::InjectHooks() {
    RH_ScopedClass(COnscreenTimer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddClock, 0x44CD50);
    RH_ScopedInstall(ClearClock, 0x44CE60);
    RH_ScopedInstall(SetClockBeepCountdownSecs, 0x44CEE0);
    RH_ScopedInstall(AddCounter, 0x44CDA0);
    RH_ScopedInstall(ClearCounter, 0x44CE80);
    RH_ScopedInstall(SetCounterFlashWhenFirstDisplayed, 0x44CEB0);
}

// 0x44CBC0
void COnscreenTimer::Init() {
    for (COnscreenCounterEntry& counter : m_aCounters) {
        counter.Init();
    }
    m_Clock.Init();
    m_bPaused = false;
}

#if ANDROID
void COnscreenTimer::Load() {

}

void COnscreenTimer::Save() {

}
#endif

// bool -> eTimerDirection
// 0x44CD50
void COnscreenTimer::AddClock(uint32 varId, char* gxt, eTimerDirection nTimerDirection) {
    if (m_Clock.m_nVarId != 0)
        return;

    m_Clock.m_nVarId = varId;
    m_Clock.m_nTimerDirection = nTimerDirection;
    if (gxt)
        strncpy_s(m_Clock.m_szDescriptionTextKey, gxt, sizeof(m_Clock.m_szDescriptionTextKey));
    else
        m_Clock.m_szDescriptionTextKey[0] = '\0';
}

// 0x44CE60
void COnscreenTimer::ClearClock(uint32 varId) {
    if (m_Clock.m_nVarId == varId) {
        m_Clock.m_nVarId = 0;
        m_Clock.m_szDescriptionTextKey[m_Clock.m_nVarId] = '\0';
        m_Clock.m_bEnabled = false;
        m_Clock.m_nTimerDirection = eTimerDirection::DECREASE;
    }
}

// 0x44CEE0
void COnscreenTimer::SetClockBeepCountdownSecs(uint32 varId, uint32 time) {
    for (COnscreenCounterEntry& counter : m_aCounters) {
        if (m_Clock.m_nVarId == varId) {
            m_Clock.m_nClockBeepCountdownSecs = time;
        }
    }
}

// uint16 -> eOnscreenCounter
// 0x44CDA0
void COnscreenTimer::AddCounter(uint32 varId, eOnscreenCounter type, char* gxt, uint16 counterIndex) {
    assert(counterIndex < COnscreenTimer::NUM_COUNTERS);

    COnscreenCounterEntry* counter = &m_aCounters[counterIndex];
    if (counter->m_nVarId != 0)
        return;

    counter->m_nVarId = varId;
    if (gxt)
        strncpy_s(counter->m_szDescriptionTextKey, gxt, sizeof(counter->m_szDescriptionTextKey));
    else
        counter->m_szDescriptionTextKey[0] = '\0';

    counter->m_nMaxVarValue = 0;
    counter->m_nType = type;
    counter->m_bFlashWhenFirstDisplayed = true;
    counter->m_nColourId = HUD_COLOUR_LIGHT_BLUE;
}

// unused
// 0x44CE00
void COnscreenTimer::AddCounterCounter(uint32 varId, uint32 maxValue, char* gxt, uint16 lineId) {
    COnscreenCounterEntry* counter = &m_aCounters[lineId];
    if (counter->m_nVarId != 0)
        return;

    counter->m_nVarId = varId;
    counter->m_nMaxVarValue = maxValue;
    if (gxt)
        strncpy_s(counter->m_szDescriptionTextKey, gxt, sizeof(counter->m_szDescriptionTextKey));
    else
        counter->m_szDescriptionTextKey[0] = '\0';

    counter->m_nType = eOnscreenCounter::DOUBLE;
    counter->m_bFlashWhenFirstDisplayed = true;
}

// 0x44CE80
void COnscreenTimer::ClearCounter(uint32 varId) {
    for (COnscreenCounterEntry& counter : m_aCounters) {
        if (counter.m_nVarId == varId) {
            counter.Clear();
        }
    }
}

// uint8 -> eHudColours
// 0x44CF10, unused
void COnscreenTimer::SetCounterColourID(uint32 varId, eHudColours colorId) {
    for (COnscreenCounterEntry& counter : m_aCounters) {
        if (counter.m_nVarId == varId) {
            counter.SetColourID(colorId);
        }
    }
}

// 0x44CEB0
void COnscreenTimer::SetCounterFlashWhenFirstDisplayed(uint32 varId, bool bFlashWhenFirstDisplayed) {
    for (COnscreenCounterEntry& counter : m_aCounters) {
        if (counter.m_nVarId == varId) {
            counter.m_bFlashWhenFirstDisplayed = bFlashWhenFirstDisplayed;
        }
    }
}

// 0x44CD30
void COnscreenTimer::Process() {
    if (CReplay::Mode == MODE_PLAYBACK || m_bPaused)
        return;

    m_Clock.Process();
}

// 0x44CC20
void COnscreenTimer::ProcessForDisplay() {
    if (!CHud::m_Wants_To_Draw_Hud)
        return;

    m_bDisplay = false;
    m_Clock.m_bEnabled = false;

    if (m_Clock.m_nVarId != 0) {
        m_Clock.ProcessForDisplayClock();
        m_Clock.m_bEnabled = true;
        m_bDisplay = true;
    }

    for (COnscreenCounterEntry& counter : m_aCounters) {
        counter.m_bEnabled = false;
        if (counter.m_nVarId != 0) {
            counter.ProcessForDisplayCounter(counter.m_nType);
            counter.m_bEnabled = true;
            m_bDisplay = true;
        }
    }
}
