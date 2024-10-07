#include "StdInc.h"
#include "Event.h"


void CEvent::InjectHooks() {
    RH_ScopedVirtualClass(CEvent, 0x85AB38, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ABFC0);
    RH_ScopedInstall(CalcSoundLevelIncrement, 0x4AC050);
    RH_ScopedInstall(GetSoundLevel, 0x4B2850);
}

// 0x4ABFC0
CEvent::CEvent() {
    m_nTimeActive = 0;
    m_bValid = false;
}

CEvent::~CEvent() {
    // nothing here
}

// 0x4ABFC0
CEvent* CEvent::Constructor() {
    this->CEvent::CEvent();
    return this;
}

// 0x4B2850
float CEvent::GetSoundLevel(const CEntity* entity, CVector& position) {
    if ((entity && entity != GetSourceEntity()) || GetLocalSoundLevel() == 0.0f)
        return 0.0f;

    float level = 1000.0f;
    if (GetEventType() == EVENT_SOUND_QUIET) {
        CVector& sourcePos = static_cast<CEventSoundQuiet*>(this)->m_position;
        level = std::min((position - sourcePos).Magnitude(), 1.0f);
    } else if (GetSourceEntity()) {
        CVector& sourcePos = GetSourceEntity()->GetPosition();
        level = std::min((position - sourcePos).Magnitude(), 1.0f);
    }
    return (GetLocalSoundLevel() - (std::log10f(level * level) * 10.0f)) - (level / 100.0f);
}

// 0x4AC050
float CEvent::CalcSoundLevelIncrement(float level1, float level2) {
    if (level2 == 0.0f)
        return 0.0f;

    if (level1 == 0.0f)
        return level2;

    level1 = std::powf(10.0f, level1 * 0.1f);
    level2 = std::powf(10.0f, level2 * 0.1f);
    return std::log10f((level1 + level2) / level1) * 10.0f;
}

