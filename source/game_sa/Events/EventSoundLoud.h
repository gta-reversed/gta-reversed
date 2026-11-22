#pragma once

#include "EventEditableResponse.h"
#include <extensions/EntityRef.hpp>

class CEventSoundLoud : public CEventEditableResponse {
public:
    CEventSoundLoud(CEntity* entity);
    ~CEventSoundLoud() override = default;

    eEventType              GetEventType() const override { return EVENT_SOUND_LOUD; }
    int32                   GetEventPriority() const override { return 16; }
    int32                   GetLifeTime() override { return 0; }
    bool                    AffectsPed(CPed* ped) override;
    CEntity*                GetSourceEntity() const override { return m_Entity; } // ?
    float                   GetLocalSoundLevel() override { return 100.0f; }
    CEventEditableResponse* CloneEditable() const noexcept override { return nullptr; }

public:
    static void InjectHooks();

private:
    notsa::EntityRef<> m_Entity{};
};
