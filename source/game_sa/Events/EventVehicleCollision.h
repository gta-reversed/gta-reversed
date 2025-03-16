#pragma once

#include "Event.h"

#include "Event.h"
#include "TaskSimpleGoTo.h"
#include "TaskComplexWalkRoundCar.h"
#include "TaskComplexHitPedWithCar.h"

enum eVehicleEvadeType {
    VEHICLE_EVADE_NONE = 0,
    VEHICLE_EVADE_BY_HITSIDE = 1
};

class NOTSA_EXPORT_VTABLE CEventVehicleCollision : public CEvent {
public:
    int16     m_pieceType;
    int16     m_evadeType;
    float     m_fDamageIntensity;
    CVehicle* m_vehicle;
    CVector   m_impactNormal;
    CVector   m_impactPos;
    int8      m_moveState;
    uint8     m_DirectionToWalkRoundCar;

public:
    static void InjectHooks();

    CEventVehicleCollision(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
    ~CEventVehicleCollision() override;

    eEventType GetEventType() const override { return EVENT_VEHICLE_COLLISION; }
    int32 GetEventPriority() const override { return 58; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }

private:
    CEventVehicleCollision* Constructor(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
};
VALIDATE_SIZE(CEventVehicleCollision, 0x34);
