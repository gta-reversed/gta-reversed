#include "StdInc.h"

#include "EventSoundQuiet.h"

void CEventSoundQuiet::InjectHooks()
{
    RH_ScopedClass(CEventSoundQuiet);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5E05B0);
    RH_ScopedInstall(AffectsPed_Reversed, 0x4B5240);
    RH_ScopedInstall(CloneEditable_Reversed, 0x5E0670);
}

// 0x5E05B0
CEventSoundQuiet::CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, CVector& position)
{
    m_fLocalSoundLevel = fLocalSoundLevel;
    m_startTimeInMs = startTime;
    m_entity = entity;
    m_position = position;
    if (m_entity)
        m_entity->RegisterReference(&m_entity);
    if (m_startTimeInMs != -1)
        return;
    m_startTimeInMs = CTimer::GetTimeInMS();
    m_position = m_entity->GetPosition();
}

CEventSoundQuiet::~CEventSoundQuiet()
{
    if (m_entity)
        m_entity->CleanUpOldReference(&m_entity);
}

CEventSoundQuiet* CEventSoundQuiet::Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, CVector& position)
{
    this->CEventSoundQuiet::CEventSoundQuiet(entity, fLocalSoundLevel, startTime, position);
    return this;
}

// 0x4B5240
bool CEventSoundQuiet::AffectsPed(CPed* ped)
{
    return CEventSoundQuiet::AffectsPed_Reversed(ped);
}

// 0x5E0670
CEventEditableResponse* CEventSoundQuiet::CloneEditable()
{
    return CEventSoundQuiet::CloneEditable_Reversed();
}

bool CEventSoundQuiet::AffectsPed_Reversed(CPed* ped)
{
    if (ped->IsPlayer() || !ped->IsAlive()|| !GetSourceEntity())
        return false;

    float soundLevel = GetSoundLevel(nullptr, ped->GetPosition());
    if (soundLevel < 30.0f)
        return false;

    auto currentEvent = static_cast<CEventSoundQuiet*>(ped->GetEventHandlerHistory().GetCurrentEvent());
    if (currentEvent &&
        currentEvent->GetEventType() == EVENT_SOUND_QUIET && currentEvent->GetSourceEntity() == GetSourceEntity()) {
        CVector distance = m_position - currentEvent->m_position;
        if (m_startTimeInMs - currentEvent->m_startTimeInMs >= 2000 && distance.SquaredMagnitude() >= 1.0f)
            return true;
        if (soundLevel - currentEvent->GetSoundLevel(nullptr, ped->GetPosition()) < 3.0f)
            return false;
    }
    return true;
}

CEventEditableResponse* CEventSoundQuiet::CloneEditable_Reversed()
{
    return new CEventSoundQuiet(m_entity, m_fLocalSoundLevel, m_startTimeInMs, m_position);
}
