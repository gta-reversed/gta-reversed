#include "StdInc.h"

#include "EventKnockOffBike.h"

#include "TaskComplexEnterCar.h"
#include "TaskSimpleCarSetPedOut.h"

void CEventKnockOffBike::InjectHooks()
{
    RH_ScopedVirtualClass(CEventKnockOffBike, 0x85B2E0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedOverloadedInstall(Constructor, "first", 0x4AFCF0, CEventKnockOffBike*(CEventKnockOffBike::*)(CVehicle*, CVector*, CVector*, float, float, uint8, uint8, int32, CPed*, bool, bool));
    RH_ScopedOverloadedInstall(Constructor, "second", 0x4AFC70, CEventKnockOffBike*(CEventKnockOffBike::*)());
    RH_ScopedVMTInstall(AffectsPed, 0x4AFEE0);
    RH_ScopedVMTInstall(ReportCriminalEvent, 0x4B4E80);
    RH_ScopedInstall(From, 0x4AFDD0);
    RH_ScopedInstall(SetPedOutCar, 0x4AFF60);
    RH_ScopedInstall(CalcForcesAndAnims, 0x4B0020);
    RH_ScopedInstall(SetPedSafePosition, 0x4B4AC0);
}

CEventKnockOffBike::CEventKnockOffBike(CVehicle* vehicle, CVector* moveSpeed, CVector* collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff)
{
    m_moveSpeed = *moveSpeed;
    m_collisionImpactVelocity = *collisionImpactVelocity;
    m_time = time;
    m_damageIntensity = damageIntensity;
    field_28 = a6;
    m_ped = ped;
    m_knockOffDirection = knockOffDirection;
    m_isVictimDriver = isVictimDriver;
    m_forceKnockOff = forceKnockOff;
    m_knockOffType = knockOffType;
    m_exitDoor = 0;
    m_vehicle = vehicle;
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_ped);
}

CEventKnockOffBike::CEventKnockOffBike()
{
    m_damageIntensity = 0.0f;
    field_28 = 0.0f;
    m_time = 0;
    m_ped = nullptr;
    m_knockOffDirection = 0;
    m_isVictimDriver = false;
    m_forceKnockOff = true;
    m_knockOffType = 0;
    m_exitDoor = 0;
    m_vehicle = nullptr;
}

CEventKnockOffBike::~CEventKnockOffBike()
{
    CEntity::SafeCleanUpRef(m_vehicle);
    CEntity::SafeCleanUpRef(m_ped);
}

CEventKnockOffBike* CEventKnockOffBike::Constructor(CVehicle* vehicle, CVector* moveSpeed, CVector* collisionImpactVelocity, float damageIntensity, float a6, uint8 knockOffType, uint8 knockOffDirection, int32 time, CPed* ped, bool isVictimDriver, bool forceKnockOff)
{
    this->CEventKnockOffBike::CEventKnockOffBike(vehicle, moveSpeed, collisionImpactVelocity, damageIntensity, a6, knockOffType, knockOffDirection, time, ped, isVictimDriver, forceKnockOff);
    return this;
}

CEventKnockOffBike* CEventKnockOffBike::Constructor()
{
    this->CEventKnockOffBike::CEventKnockOffBike();
    return this;
}

// 0x4AFEE0
bool CEventKnockOffBike::AffectsPed(CPed* ped)
{
    if (ped->IsAlive()) {
        if (m_vehicle && m_vehicle->m_nStatus == STATUS_GHOST)
            return false;
        if (ped->CantBeKnockedOffBike && !ped->bHasBeenRendered && !m_forceKnockOff)
            return false;
        if (m_vehicle) {
            if (m_vehicle->m_pDriver != ped
                && m_vehicle->m_apPassengers[0] != ped
                && m_vehicle->m_apPassengers[1] != ped
                && m_vehicle->m_apPassengers[2] != ped)
            {
                return false;
            }
            if (m_vehicle->m_pDriver == ped)
                m_isVictimDriver = true;
            return true;
        }
    }
    return false;
}

// 0x4B4E80
void CEventKnockOffBike::ReportCriminalEvent(CPed* ped)
{
    if (IsCriminalEvent()) {
        if (ped->m_nPedType == PED_TYPE_COP && m_ped && m_ped->IsPlayer())
            FindPlayerWanted()->RegisterCrime(eCrimeType::CRIME_DAMAGE_COP_CAR, ped->GetPosition(), ped, false);
    }
}

// 0x4AFDD0
void CEventKnockOffBike::From(const CEventKnockOffBike& right)
{
    m_vehicle = right.m_vehicle;
    m_moveSpeed = right.m_moveSpeed;
    m_collisionImpactVelocity = right.m_collisionImpactVelocity;
    m_damageIntensity = right.m_damageIntensity;
    field_28 = right.field_28;
    m_knockOffType = right.m_knockOffType;
    m_knockOffDirection = right.m_knockOffDirection;
    m_time = right.m_time;
    m_ped = right.m_ped;
    m_isVictimDriver = right.m_isVictimDriver;
    m_forceKnockOff = right.m_forceKnockOff;
    m_exitDoor = right.m_exitDoor;
    CEntity::SafeRegisterRef(m_vehicle);
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AFF60
void CEventKnockOffBike::SetPedOutCar(CPed* ped)
{
    m_exitDoor = TARGET_DOOR_DRIVER;
    if (m_vehicle->m_pDriver == ped)
        m_exitDoor = TARGET_DOOR_DRIVER;
    else if (m_vehicle->IsBike())
        m_exitDoor = TARGET_DOOR_REAR_LEFT;
    else if (m_vehicle->m_apPassengers[0] == ped)
        m_exitDoor = TARGET_DOOR_FRONT_RIGHT;
    else if (m_vehicle->m_apPassengers[1] == ped)
        m_exitDoor = TARGET_DOOR_REAR_LEFT;
    else if (m_vehicle->m_apPassengers[2] == ped)
        m_exitDoor = TARGET_DOOR_REAR_RIGHT;
    CTaskSimpleCarSetPedOut taskCarSetPedOut(m_vehicle, (eTargetDoor)m_exitDoor, true);
    taskCarSetPedOut.m_bKnockedOffBike = true;
    taskCarSetPedOut.m_bSwitchOffEngine = false;
    taskCarSetPedOut.ProcessPed(ped);
    CCarEnterExit::RemoveCarSitAnim(ped);
}

// 0x4B0020
int32 CEventKnockOffBike::CalcForcesAndAnims(CPed* ped)
{
    uint8 numContactWheels = 0;
    float massRatio = ped->m_fMass / m_vehicle->m_fMass;
    if (m_vehicle->IsBike())
        numContactWheels = m_vehicle->AsBike()->m_nNoOfContactWheels;
    else if (m_vehicle->IsAutomobile())
        numContactWheels = m_vehicle->AsAutomobile()->m_nNumContactWheels;

    switch (m_knockOffType)
    {
    case KNOCK_OFF_TYPE_NONE:
    case KNOCK_OFF_TYPE_NONE_1:
        ped->m_vecMoveSpeed = m_moveSpeed;
        ped->m_pEntityIgnoredCollision = m_vehicle;
        return ANIM_ID_NO_ANIMATION_SET;
    default:
        switch (m_knockOffDirection)
        {
        case 0:
        {
            if (m_vehicle->m_vecMoveSpeed.SquaredMagnitude() < 0.09f) {
                CVector force = m_vehicle->GetForward() * -6.0f + m_vehicle->GetUp() * 5.0f;
                ped->ApplyMoveForce(force);
            }
            ped->m_pEntityIgnoredCollision = m_vehicle;
            return ANIM_ID_BIKE_FALLR;
        }
        case 1:
        case 2:
        {
            if (m_vehicle->m_vecMoveSpeed.SquaredMagnitude() <= 0.09f) {
                ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed;
                CVector force = m_vehicle->GetUp() * 4.0f + m_vehicle->GetRight() * 8.0f;
                ped->ApplyMoveForce(force);
                return ANIM_ID_KD_LEFT;
            }
            ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed * 0.3f;
            CVector force = m_vehicle->GetUp() * 5.0f + m_vehicle->GetRight() * 6.0f;
            ped->ApplyMoveForce(force);
            return ANIM_ID_KO_SPIN_R;
        }
        case 3:
        {
            if (m_vehicle->m_vecMoveSpeed.SquaredMagnitude() <= 0.09f) {
                ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed;
                CVector force = m_vehicle->GetUp() * 4.0f + m_vehicle->GetRight() * -8.0f;
                ped->ApplyMoveForce(force);
                return ANIM_ID_KD_RIGHT;
            }
            ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed * 0.3f;
            CVector force = m_vehicle->GetUp() * 5.0f + m_vehicle->GetRight() * -6.0f;
            ped->ApplyMoveForce(force);
            return ANIM_ID_KO_SPIN_L;
        }
        default:
            NOTSA_UNREACHABLE();
        }
        break;
    case KNOCK_OFF_TYPE_SKIDBACKFRONT:
    {
        ped->m_vecMoveSpeed = m_moveSpeed;
        float factor = -0.3f;
        if (m_vehicle->IsSubBMX())
            factor = -0.2f;
        CVector force = factor * m_collisionImpactVelocity * massRatio * m_damageIntensity;
        force.z = 0.0f;
        ped->ApplyMoveForce(force);
        float randomNum = 5.0f;
        if (m_vehicle->IsSubBMX())
            randomNum = 6.0f;
        ped->m_vecMoveSpeed.z += CGeneral::GetRandomNumberInRange(3.0f, randomNum) / 70.0f;
        ped->m_pEntityIgnoredCollision = m_vehicle;
        int32 animId = ANIM_ID_KO_SKID_BACK;
        switch (m_knockOffDirection)
        {
        case 0:
            animId = ANIM_ID_KO_SKID_BACK;
            break;
        case 1:
            animId = ANIM_ID_KD_RIGHT;
            break;
        case 2:
            animId = ANIM_ID_KO_SKID_FRONT;
            break;
        case 3:
            animId = ANIM_ID_KD_LEFT;
            break;
        }
        ped->bKnockedOffBike = true;
        ped->IsPlayer(); // leftover?
        return animId;
    }
    case KNOCK_OFF_TYPE_FALL:
    {
        ped->m_vecMoveSpeed = m_vehicle->m_vecMoveSpeed * 0.2f;
        ped->m_vecMoveSpeed.z = 0.0f;
        ped->m_pEntityIgnoredCollision = m_vehicle;
        return ANIM_ID_FALL_FALL;
    }
    case KNOCK_OFF_TYPE_SKIDBACK_FALLR:
    {
        ped->m_vecMoveSpeed = m_moveSpeed;
        CVector force = -0.3f * m_collisionImpactVelocity * massRatio * m_damageIntensity;
        force.z = CGeneral::GetRandomNumberInRange(3.0f, 5.0f);
        ped->ApplyMoveForce(force);
        ped->m_pEntityIgnoredCollision = m_vehicle;
        int32 animId = ANIM_ID_KO_SKID_BACK;
        switch (m_knockOffDirection)
        {
        case 0:
            animId = ANIM_ID_KO_SKID_BACK;
            break;
        case 1:
            animId = ANIM_ID_KD_RIGHT;
            break;
        case 2:
            animId = ANIM_ID_BIKE_FALLR;
            break;
        case 3:
            animId = ANIM_ID_KD_LEFT;
            break;
        }
        if (!numContactWheels)
            ped->bKnockedOffBike = true;
        return animId;
    }
    }
    return ANIM_ID_NO_ANIMATION_SET;
}

// 0x4B4AC0
bool CEventKnockOffBike::SetPedSafePosition(CPed* ped)
{
    if (m_vehicle->IsBike()) {
        CBike* bike = m_vehicle->AsBike();
        bike->m_RideAnimData.m_fAnimLean = 0.0f;
        bike->m_bLeanMatrixCalculated = false;
        ped->SetPedPositionInCar();
    }

    if (m_vehicle->GetUp().z >= 0.0f)
        ped->m_fAimingRotation = m_vehicle->GetHeading();
    else
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(m_vehicle->GetHeading() + PI);

    ped->m_fCurrentRotation = ped->m_fAimingRotation;
    ped->SetHeading(ped->m_fAimingRotation);

    if (m_vehicle->IsBike() && !m_isVictimDriver) {
        float forwardDistance = (1.0f - fabs(DotProduct(m_vehicle->GetForward(), ped->GetForward()))) * 0.8f;
        CVector distance = ped->GetPosition() - (forwardDistance * ped->GetForward());
        ped->SetPosn(distance);
    }

    bool isSafe = true;
    bool findClosestNode = false;
    if (ped->m_fHealth <= 0.0f || m_ped) {
        findClosestNode = true;
    }

    CWorld::pIgnoreEntity = m_vehicle;
    ped->m_pEntityIgnoredCollision = m_vehicle;
    CVector savedPedPos = ped->GetPosition();
    bool knockPedOffBike = false;
    if (!ped->TestCollision(false) &&
        CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), savedPedPos, true, false, false, true, false, false, false)) {
        if (m_knockOffType == KNOCK_OFF_TYPE_SKIDBACKFRONT) {
            ped->GetPosition().z += 0.5f;
            if (ped->TestCollision(false))
                ped->SetPosn(savedPedPos);
            if (!CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false))
                ped->SetPosn(savedPedPos);
        }
        knockPedOffBike = true;
    }
    else {
        ped->GetPosition().z += 0.6f;
        if (!ped->TestCollision(false) &&
            CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false)) {
            ped->m_pEntityIgnoredCollision = m_vehicle;
            knockPedOffBike = true;
        }
        else {
            ped->SetPosn(savedPedPos);
            ped->GetPosition().z -= 0.6f;
            if (!ped->TestCollision(false) &&
                CWorld::GetIsLineOfSightClear(m_vehicle->GetPosition(), ped->GetPosition(), true, false, false, true, false, false, false)) {
                ped->m_pEntityIgnoredCollision = m_vehicle;
                knockPedOffBike = true;
            }
            else {
                isSafe = ped->PositionPedOutOfCollision(m_exitDoor, m_vehicle, findClosestNode);
                if (isSafe) {
                    ped->m_pEntityIgnoredCollision = m_vehicle;
                    knockPedOffBike = true;
                }
            }
        }
    }

    if (knockPedOffBike) {
        ped->bKnockedOffBike = true;
        CEntity::ClearReference(ped->m_standingOnEntity);
        ped->bWasStanding = false;
        ped->bIsStanding = false;
    }

    CWorld::pIgnoreEntity = nullptr;
    return isSafe;
}
