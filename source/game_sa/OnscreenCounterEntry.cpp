#include "StdInc.h"
#include "OnscreenCounterEntry.h"
#include "TheScripts.h"

void COnscreenCounterEntry::InjectHooks() {
    RH_ScopedClass(COnscreenCounterEntry);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ProcessForDisplayCounter, 0x44CA90);
    RH_ScopedInstall(SetColourID, 0x44CB00);
}

void COnscreenCounterEntry::Init() {
    Clear();

    m_bFlashWhenFirstDisplayed = false;
    m_nColourId                = HUD_COLOUR_RED;
    memset(&m_szDisplayedText, 0, sizeof(m_szDisplayedText));           // original initialization
}

void COnscreenCounterEntry::Clear() {
    m_nVarId                  = 0;
    m_nMaxVarValue            = 0;
    m_nType                   = eOnscreenCounter::SIMPLE;
    m_bEnabled                = false;
    memset(&m_szDescriptionTextKey, 0, sizeof(m_szDescriptionTextKey)); // original initialization
}

// 0x44CA90
void COnscreenCounterEntry::ProcessForDisplayCounter(eOnscreenCounter type) {
    const auto t2 = *CTheScripts::GetPointerToScriptVariable(m_nVarId);
    const auto t1 = std::max<int32>(0, t2);

    switch (type) {
    case eOnscreenCounter::SIMPLE:
    case eOnscreenCounter::LINE:
        sprintf_s(m_szDisplayedText, "%d", t1);
        break;
    case eOnscreenCounter::DOUBLE:
        sprintf_s(m_szDisplayedText, "%d / %d", t1, t2);
        break;
    }
}

// 0x44CB00
void COnscreenCounterEntry::SetColourID(eHudColours colorId) {
    m_nColourId = colorId;
}
