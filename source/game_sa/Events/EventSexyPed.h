#pragma once

#include "EventEditableResponse.h"

#include <Enums/eTaskType.h>
#include "./EventEditableResponse.h"
#include "IKChainManager_c.h"


class NOTSA_EXPORT_VTABLE CEventSexyPed : public CEventEditableResponse {
public:
    CPed* m_SexyPed;

public:
    CEventSexyPed(CPed* ped);
    CEventSexyPed(CPed* sexyPed, eTaskType taskType);
    ~CEventSexyPed() override;

    eEventType GetEventType() const override { return EVENT_SEXY_PED; }
    int32 GetEventPriority() const override { return 4; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_SexyPed; }
    CEventEditableResponse* CloneEditable() override { return new CEventSexyPed(m_SexyPed); }
};
VALIDATE_SIZE(CEventSexyPed, 0x18);
