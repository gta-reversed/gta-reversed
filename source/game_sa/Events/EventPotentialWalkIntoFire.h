#pragma once

#include "EventEditableResponse.h"

#include "EventEditableResponse.h"
#include "Vector.h"
#include "Enums/eMoveState.h"
#include "Events/EventPotentialWalkIntoPed.h"
#include "Events/EventPotentialWalkIntoFire.h"
#include "Events/EventPotentialWalkIntoObject.h"
#include "Events/EventPotentialWalkIntoVehicle.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskSimpleGoTo.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexWalkRoundObject.h"
#include "TaskComplexFollowPedFootsteps.h"
#include "TaskComplexKillPedOnFoot.h"


class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoFire : public CEventEditableResponse {
public:
    CVector m_firePos;
    float m_fireSize;
    float m_radius;
    eMoveState m_moveState;

public:
    CEventPotentialWalkIntoFire(const CVector& firePos, float fireSize, eMoveState moveState);
    ~CEventPotentialWalkIntoFire() override = default;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_FIRE; }
    int32 GetEventPriority() const override { return 31; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;
    CEventPotentialWalkIntoFire* CloneEditable() const noexcept override { return new CEventPotentialWalkIntoFire(m_firePos, m_fireSize, m_moveState); }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPotentialWalkIntoFire* Constructor(const CVector& firePos, float fireSize, eMoveState moveState);

};
VALIDATE_SIZE(CEventPotentialWalkIntoFire, 0x2C);
