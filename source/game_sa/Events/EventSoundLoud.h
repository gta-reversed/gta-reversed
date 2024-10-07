#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"


class CEventSoundLoud : public CEventEditableResponse {
public:
    CEntity* m_Entity;

public:
    CEventSoundLoud(CEntity* entity);
    ~CEventSoundLoud() override;

    eEventType GetEventType() const override { return EVENT_SOUND_LOUD; }
    int32 GetEventPriority() const override { return 16; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_Entity; } // ?
    float GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() override { return nullptr; }
};

