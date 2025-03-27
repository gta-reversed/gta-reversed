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


class NOTSA_EXPORT_VTABLE CEventPotentialWalkIntoVehicle : public CEventEditableResponse {
public:
    CVehicle* m_vehicle;
    int32     m_moveState;

public:
    CEventPotentialWalkIntoVehicle(CVehicle* vehicle, int32 moveState);
    ~CEventPotentialWalkIntoVehicle() override;

    eEventType GetEventType() const override { return EVENT_POTENTIAL_WALK_INTO_VEHICLE; }
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }
    CEventPotentialWalkIntoVehicle* CloneEditable() override { return new CEventPotentialWalkIntoVehicle(m_vehicle, m_moveState); }
    int32 GetEventPriority() const override { return 29; }
    int32 GetLifeTime() override { return 0; }
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventPotentialWalkIntoVehicle* Constructor(CVehicle* vehicle, int32 moveState);

};
VALIDATE_SIZE(CEventPotentialWalkIntoVehicle, 0x1C);
