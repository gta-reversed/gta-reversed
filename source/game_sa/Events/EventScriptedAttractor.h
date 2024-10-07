#pragma once

#include "EventAttractor.h"

#include "EventEditableResponse.h"
#include "EventAttractor.h"
#include "IKChainManager_c.h"


class NOTSA_EXPORT_VTABLE CEventScriptedAttractor : public CEventAttractor {
public:
    static void InjectHooks();

    CEventScriptedAttractor(C2dEffect* the2dEffect, CEntity* entity, bool bAvoidLookingAtAttractor);
    ~CEventScriptedAttractor() override = default;

    CEventScriptedAttractor* Constructor(C2dEffect* the2dEffect, CEntity* entity, bool bAvoidLookingAtAttractor);
    eEventType GetEventType() const override { return EVENT_SCRIPTED_ATTRACTOR; }
    CEventScriptedAttractor* CloneEditable() override { return new CEventScriptedAttractor(m_2dEffect, m_entity, false); }
};
VALIDATE_SIZE(CEventScriptedAttractor, 0x20);
