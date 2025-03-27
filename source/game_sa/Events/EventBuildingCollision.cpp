#include "StdInc.h"
#include "EventBuildingCollision.h"


void CEventBuildingCollision::InjectHooks()
{
    RH_ScopedVirtualClass(CEventBuildingCollision, 0x85ACE8, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4ACF00);
    RH_ScopedVMTInstall(AffectsPed, 0x4AD070);
    RH_ScopedInstall(IsHeadOnCollision, 0x4AD1E0);
    RH_ScopedInstall(CanTreatBuildingAsObject, 0x4B3120);
}

// 0x4ACF00
CEventBuildingCollision::CEventBuildingCollision(int16 pieceType, float damageIntensity, CBuilding* building, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState)
{
    m_pieceType = pieceType;
    m_moveState = moveState;
    m_damageIntensity = damageIntensity;
    m_building = building;
    m_impactNormal = *collisionImpactVelocity;
    m_impactPos = *collisionPos;
    CEntity::SafeRegisterRef(m_building);
}

CEventBuildingCollision::~CEventBuildingCollision()
{
    CEntity::SafeCleanUpRef(m_building);
}

// 0x4ACF00
CEventBuildingCollision* CEventBuildingCollision::Constructor(int16 pieceType, float damageIntensity, CBuilding* building, CVector* collisionImpactVelocity, CVector* collisionPos, int16 moveState)
{
    this->CEventBuildingCollision::CEventBuildingCollision(pieceType, damageIntensity, building, collisionImpactVelocity, collisionPos, moveState);
    return this;
}

// 0x4AD070
bool CEventBuildingCollision::AffectsPed(CPed* ped)
{
    if (!ped->IsPlayer()
        && ped->IsAlive()
        && m_moveState != PEDMOVE_STILL
        && (m_impactNormal.z <= 0.707f || m_building->m_bIsTempBuilding)
        && !ped->m_pAttachedTo)
    {
        CVector direction(m_impactNormal.x, m_impactNormal.y, 0.0f);
        direction.Normalise();
        float dotProduct = DotProduct(direction, ped->GetForward());
        if (dotProduct <= -0.422f) {
            if (ped->GetEventHandler().GetCurrentEventType() == EVENT_BUILDING_COLLISION) {
                auto buildingColEvent = static_cast<CEventBuildingCollision*>(ped->GetEventHandlerHistory().GetCurrentEvent());
                if (buildingColEvent->m_building == m_building && dotProduct > -0.5f) 
                    return false;
            }
            // CEventPotentialWalkIntoBuilding doesn't even exist on PC
            assert (ped->GetEventHandler().GetCurrentEventType() != EVENT_POTENTIAL_WALK_INTO_BUILDING);
            if (dotProduct <= -0.5f && !ped->GetTaskManager().Find<TASK_COMPLEX_CLIMB>())
                return ped->GetEventHandler().GetCurrentEventType() != EVENT_GUN_AIMED_AT;
        }
    }
    return false;
}

// 0x4AD1E0
bool CEventBuildingCollision::IsHeadOnCollision(CPed* ped)
{
    CVector velocity = m_impactNormal;
    velocity.z = 0.0f;
    velocity.Normalise();
    return -DotProduct(velocity, ped->GetForward()) > 0.866f;
}

// 0x4B3120
bool CEventBuildingCollision::CanTreatBuildingAsObject(CBuilding* building)
{
    if (building->m_bIsTempBuilding)
        return true;

    CColModel* colModel = CModelInfo::GetModelInfo(building->m_nModelIndex)->GetColModel();
    CVector& boundMax = colModel->m_boundBox.m_vecMax;
    CVector& boundMin = colModel->m_boundBox.m_vecMin;
    float absoluteLengthX = fabs(boundMax.x - boundMin.x);
    float absoluteLengthY = fabs(boundMax.y - boundMin.y);
    float lengthZ = boundMax.z - boundMin.z;
    if (lengthZ < 2.0f && (absoluteLengthX < 6.0f || absoluteLengthY < 6.0f)
        || absoluteLengthX < 6.0f && absoluteLengthY < 6.0f)
    {
        return true;
    }
    return false;
}

