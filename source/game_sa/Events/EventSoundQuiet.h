#pragma once

#include "EventEditableResponse.h"
#include "Vector.h"

class NOTSA_EXPORT_VTABLE CEventSoundQuiet final : public CEventEditableResponse {
public:
    CEntity* m_entity;
    float    m_fLocalSoundLevel;
    uint32   m_startTimeInMs;
    CVector  m_position;

public:
    CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position);
    ~CEventSoundQuiet() override;

    eEventType GetEventType() const override { return EVENT_SOUND_QUIET; }
    int32 GetEventPriority() const override { return 8; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_entity; }
    float GetLocalSoundLevel() override { return m_fLocalSoundLevel; }
    bool CanBeInterruptedBySameEvent() override { return true; }
    CEventEditableResponse* CloneEditable() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventSoundQuiet* Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position);

};
