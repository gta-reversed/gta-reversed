#pragma once

#include "EventEditableResponse.h"
#include "Vector.h"
#include "Enums/eMoveState.h"

class CPed;

class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoPed : public CEventEditableResponse {
public:
    CVector    m_targetPoint;
    CPed*      m_ped;
    eMoveState m_moveState;

public:
    CEventPotentialWalkIntoPed(CPed* ped, const CVector& targetPoint, eMoveState moveState);
    ~CEventPotentialWalkIntoPed() override;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_PED; }
    int32 GetEventPriority() const override { return 28; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEntity* GetSourceEntity() const override { return m_ped; }
    bool TakesPriorityOver(const CEvent& refEvent) override;
    CEventPotentialWalkIntoPed* CloneEditable() override { return new CEventPotentialWalkIntoPed(m_ped, m_targetPoint, m_moveState); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPotentialWalkIntoPed* Constructor(CPed* ped, const CVector& targetPoint, eMoveState moveState) {
        this->CEventPotentialWalkIntoPed::CEventPotentialWalkIntoPed(ped, targetPoint, moveState);
        return this;
    }

};
VALIDATE_SIZE(CEventPotentialWalkIntoPed, 0x28);
