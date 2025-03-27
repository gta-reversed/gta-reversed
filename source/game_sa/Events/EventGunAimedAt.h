#pragma once

#include "Event.h"
#include "Ped.h"

class CPed;

class NOTSA_EXPORT_VTABLE CEventGunAimedAt : public CEventEditableResponse {
public:
    CPed* m_AimedBy;

public:
    static constexpr auto Type = EVENT_GUN_AIMED_AT;

    explicit CEventGunAimedAt(CPed* ped);
    ~CEventGunAimedAt() override;

    eEventType GetEventType() const override { return Type; }
    int32 GetEventPriority() const override { return 50; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    bool IsCriminalEvent() override { return m_AimedBy && m_AimedBy->IsPlayer(); }
    void ReportCriminalEvent(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_AimedBy; }
    bool TakesPriorityOver(const CEvent& refEvent) override;
    CEventEditableResponse* CloneEditable() override;
};
VALIDATE_SIZE(CEventGunAimedAt, 0x18);
