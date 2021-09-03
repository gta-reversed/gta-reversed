#pragma once

#include "CEvent.h"

class CVehicle;
class CVector;

enum eVehicleEvadeType {
    VEHICLE_EVADE_NONE = 0,
    VEHICLE_EVADE_BY_HITSIDE = 1
};

class CEventVehicleCollision : public CEvent {
public:
    int16     m_pieceType;
    int16     m_evadeType;
    float     m_fDamageIntensity;
    CVehicle* m_vehicle;
    CVector   m_collisionImpactVelocity;
    CVector   m_collisionPosition;
    int8      m_moveState;
    int8      field_31;
    int8      field_32[2];

    static void InjectHooks();

    CEventVehicleCollision(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
    ~CEventVehicleCollision();
private:
    CEventVehicleCollision* Constructor(int16 pieceType, float damageIntensity, CVehicle* vehicle, const CVector& collisionImpactVelocity, const CVector& collisionPosition, int8 moveState, int16 evadeType);
public:
    eEventType GetEventType() const override { return EVENT_VEHICLE_COLLISION; }
    int32 GetEventPriority() const override { return 58; }
    int32 GetLifeTime() override { return 0; }
    CEvent* Clone() override;
    bool AffectsPed(CPed* ped) override;
    bool TakesPriorityOver(const CEvent& refEvent) override { return true; }

    CEvent* Clone_Reversed();
    bool AffectsPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CEventVehicleCollision, 0x34);
