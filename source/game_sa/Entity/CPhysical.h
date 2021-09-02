/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "CEntity.h"
#include "CColPoint.h"
#include "CVector.h"
#include "CQuaternion.h"
#include "CRealTimeShadow.h"
#include "CRepeatSector.h"
#include "eWeaponType.h"
#include "CEntryInfoList.h"

enum ePhysicalFlags {
    PHYSICAL_b01 = 0x1,
    PHYSICAL_APPLY_GRAVITY = 0x2,
    PHYSICAL_DISABLE_COLLISION_FORCE = 0x4,
    PHYSICAL_COLLIDABLE = 0x8, 
    PHYSICAL_DISABLE_TURN_FORCE = 0x10,
    PHYSICAL_DISABLE_MOVE_FORCE = 0x20,
    PHYSICAL_INFINITE_MASS = 0x40,
    PHYSICAL_DISABLE_Z = 0x80,

    PHYSICAL_SUBMERGED_IN_WATER = 0x100,
    PHYSICAL_ON_SOLID_SURFACE = 0x200,
    PHYSICAL_BROKEN = 0x400,
    PHYSICAL_b12 = 0x800,
    PHYSICAL_b13 = 0x1000,
    PHYSICAL_DONT_APPLY_SPEED = 0x2000,
    PHYSICAL_b15 = 0x4000,
    PHYSICAL_b16 = 0x8000,

    PHYSICAL_17 = 0x10000,
    PHYSICAL_18 = 0x20000,
    PHYSICAL_BULLETPROOF = 0x40000,
    PHYSICAL_FIREPROOF = 0x80000,
    PHYSICAL_COLLISIONPROOF = 0x100000,
    PHYSICAL_MEELEPROOF = 0x200000,
    PHYSICAL_INVULNERABLE = 0x400000,
    PHYSICAL_EXPLOSIONPROOF = 0x800000,

    PHYSICAL_25 = 0x1000000,
    PHYSICAL_ATTACHEDTOENTITY = 0x2000000,
    PHYSICAL_27 = 0x4000000,
    PHYSICAL_TOUCHINGWATER = 0x8000000,
    PHYSICAL_CANBECOLLIDEDWITH = 0x10000000,
    PHYSICAL_DESTROYED = 0x20000000,
    PHYSICAL_31 = 0x40000000,
    PHYSICAL_32 = 0x80000000,
};

enum eEntityAltCollision : uint16 {
    ALT_ENITY_COL_DEFAULT = 0,
    ALT_ENITY_COL_OBJECT,
    ALT_ENITY_COL_VEHICLE,
    ALT_ENITY_COL_BIKE_WRECKED,
    ALT_ENITY_COL_BOAT,
};

class CPhysical : public CEntity {
public:
    CPhysical();
    ~CPhysical() override;
public:
    float  field_38;
    uint32 m_nLastCollisionTime;
    union {
        struct {
            uint32 bMakeMassTwiceAsBig : 1;
            uint32 bApplyGravity : 1;
            uint32 bDisableCollisionForce : 1;
            uint32 bCollidable : 1;
            uint32 bDisableTurnForce : 1;
            uint32 bDisableMoveForce : 1;
            uint32 bInfiniteMass : 1;
            uint32 bDisableZ : 1;

            uint32 bSubmergedInWater : 1;
            uint32 bOnSolidSurface : 1;
            uint32 bBroken : 1;
            uint32 bProcessCollisionEvenIfStationary : 1; // ref @ 0x6F5CF0
            uint32 b13 : 1;                               // only used for peds
            uint32 bDontApplySpeed : 1;
            uint32 b15 : 1;
            uint32 bProcessingShift : 1;

            uint32 b17 : 1;
            uint32 bDisableSimpleCollision : 1; // ref @ CPhysical::ProcessCollision
            uint32 bBulletProof : 1;
            uint32 bFireProof : 1;
            uint32 bCollisionProof : 1;
            uint32 bMeeleProof : 1;
            uint32 bInvulnerable : 1;
            uint32 bExplosionProof : 1;

            uint32 bDontCollideWithFlyers : 1;
            uint32 bAttachedToEntity : 1;
            uint32 bAddMovingCollisionSpeed : 1;
            uint32 bTouchingWater : 1;
            uint32 bCanBeCollidedWith : 1;
            uint32 bDestroyed : 1;
            uint32 b31 : 1;
            uint32 b32 : 1;
        } physicalFlags;
        uint32 m_nPhysicalFlags;
    };
    CVector             m_vecMoveSpeed;
    CVector             m_vecTurnSpeed;
    CVector             m_vecFrictionMoveSpeed;
    CVector             m_vecFrictionTurnSpeed;
    CVector             m_vecForce;
    CVector             m_vecTorque;
    float               m_fMass;
    float               m_fTurnMass;
    float               m_fVelocityFrequency;
    float               m_fAirResistance;
    float               m_fElasticity;
    float               m_fBuoyancyConstant;
    CVector             m_vecCentreOfMass;
    CEntryInfoList      m_pCollisionList;
    CPtrNodeDoubleLink* m_pMovingList;
    uint8               m_nFakePhysics;
    uint8               m_nNumEntitiesCollided;
    uint8               m_nContactSurface;
    char                field_BB;
    CEntity*            m_apCollidedEntities[6];
    float               m_fMovingSpeed; // ref @ CTheScripts::IsVehicleStopped
    float               m_fDamageIntensity;
    CEntity*            m_pDamageEntity;
    CVector             m_vecLastCollisionImpactVelocity;
    CVector             m_vecLastCollisionPosn;
    uint16              m_nPieceType;
    int16               field_FA;
    class CPhysical*    m_pAttachedTo;
    CVector             m_vecAttachOffset;
    CVector             m_vecAttachedEntityRotation;
    CQuaternion         m_qAttachedEntityRotation;
    CEntity*            m_pEntityIgnoredCollision;
    float               m_fContactSurfaceBrightness;
    float               m_fDynamicLighting;
    CRealTimeShadow*    m_pShadowData;

    static float& DAMPING_LIMIT_IN_FRAME;
    static float& DAMPING_LIMIT_OF_SPRING_FORCE;
    static float& PHYSICAL_SHIFT_SPEED_DAMP;
    static float& SOFTCOL_SPEED_MULT;
    static float& SOFTCOL_SPEED_MULT2;
    static float& SOFTCOL_DEPTH_MIN;
    static float& SOFTCOL_DEPTH_MULT;
    static float& SOFTCOL_CARLINE_SPEED_MULT;
    static float& TEST_ADD_AMBIENT_LIGHT_FRAC;
    static float& HIGHSPEED_ELASTICITY_MULT_COPCAR;
    static CVector& fxDirection;

    static void InjectHooks();

    // originally virtual functions
    void Add() override;
    void Remove() override;
    CRect* GetBoundRect(CRect* pRect) override;
    void ProcessControl() override;
    void ProcessCollision() override;
    void ProcessShift() override;
    bool TestCollision(bool bApplySpeed) override;
    virtual int32 ProcessEntityCollision(CPhysical* entity, CColPoint* colpoint);
private:
    void Add_Reversed();
    void Remove_Reversed();
    CRect* GetBoundRect_Reversed(CRect* pRect);
    void ProcessControl_Reversed();
    void ProcessCollision_Reversed();
    void ProcessShift_Reversed();
    bool TestCollision_Reversed(bool bApplySpeed);
    int32 ProcessEntityCollision_Reversed(CPhysical* entity, CColPoint* colpoint);
public:
    void RemoveAndAdd();
    void AddToMovingList();
    void RemoveFromMovingList();
    void SetDamagedPieceRecord(float fDamageIntensity, CEntity* entity, CColPoint* colPoint, float fDistanceMult);
    void ApplyMoveForce(float x, float y, float z);
    void ApplyMoveForce(CVector force);
    void ApplyTurnForce(CVector force, CVector point);
    void ApplyForce(CVector vecMoveSpeed, CVector point, bool bUpdateTurnSpeed);
    CVector GetSpeed( CVector point);
    void ApplyMoveSpeed();
    void ApplyTurnSpeed();
    void ApplyGravity();
    void ApplyFrictionMoveForce(CVector moveForce);
    void ApplyFrictionTurnForce(CVector posn, CVector velocity);
    void ApplyFrictionForce(CVector vecMoveForce, CVector point);
    void SkipPhysics();
    void AddCollisionRecord(CEntity* collidedEntity);
    bool GetHasCollidedWith(CEntity* entity);
    bool GetHasCollidedWithAnyObject();
    bool ApplyCollision(CEntity* pEntity, CColPoint* pColPoint, float* pDamageIntensity);
    bool ApplySoftCollision(CEntity* pEntity, CColPoint* pColPoint, float* pDamageIntensity);
    bool ApplySpringCollision(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, float& fSpringForceDampingLimit);
    bool ApplySpringCollisionAlt(float fSuspensionForceLevel, CVector& direction, CVector& collisionPoint, float fSpringLength, float fSuspensionBias, CVector& normal, float& fSpringForceDampingLimit);
    bool ApplySpringDampening(float fDampingForce, float fSpringForceDampingLimit, CVector& direction, CVector& collisionPoint, CVector& collisionPos);
    bool ApplySpringDampeningOld(float arg0, float arg1, CVector& arg2, CVector& arg3, CVector& arg4);
    void RemoveRefsToEntity(CEntity* entity);
    void DettachEntityFromEntity(float x, float y, float z, bool bApplyTurnForce);
    void DettachAutoAttachedEntity();
    float GetLightingFromCol(bool bInteriorLighting);
    float GetLightingTotal();
    bool CanPhysicalBeDamaged(eWeaponType weapon, bool* bDamagedDueToFireOrExplosionOrBullet);
    void ApplyAirResistance();
    bool ApplyCollisionAlt(CPhysical* pEntity, CColPoint* pColPoint, float* pDamageIntensity, CVector* pVecMoveSpeed, CVector* pVecTurnSpeed);
    bool ApplyFriction(float fFriction, CColPoint* pColPoint);
    bool ApplyFriction(CPhysical* pEntity, float fFriction, CColPoint* pColPoint);
    bool ProcessShiftSectorList(int32 sectorX, int32 sectorY);
    static void PlacePhysicalRelativeToOtherPhysical(CPhysical* relativeToPhysical, CPhysical* physicalToPlace, CVector offset);
    float ApplyScriptCollision(CVector arg0, float arg1, float arg2, CVector* arg3);
    void PositionAttachedEntity();
    void ApplySpeed();
    void UnsetIsInSafePosition();
    void ApplyFriction();
    bool ApplyCollision(CEntity* pTheEntity, CColPoint* pColPoint, float* pThisDamageIntensity, float* pEntityDamageIntensity);
    bool ApplySoftCollision(CPhysical* pEntity, CColPoint* pColPoint, float* pThisDamageIntensity, float* pEntityDamageIntensity);
    bool ProcessCollisionSectorList(int32 sectorX, int32 sectorY);
    bool ProcessCollisionSectorList_SimpleCar(CRepeatSector* pRepeatSector);
    void AttachEntityToEntity(CPhysical* entity, CVector offset, CVector rotation);
    void AttachEntityToEntity(CPhysical* pEntityAttachTo, CVector* vecAttachOffset, CQuaternion* attachRotation);
    bool CheckCollision();
    bool CheckCollision_SimpleCar();

    void ResetMoveSpeed() { m_vecMoveSpeed = CVector(); }
    void ResetTurnSpeed() { m_vecTurnSpeed = CVector(); }
    void ResetFrictionMoveSpeed() { m_vecFrictionMoveSpeed = CVector(); }
    void ResetFrictionTurnSpeed() { m_vecFrictionTurnSpeed = CVector(); }

    float GetMass(const CVector& pos, const CVector& dir) {
        return 1.0f / (CrossProduct(pos, dir).SquaredMagnitude() / m_fTurnMass + 1.0f / m_fMass);
    }

// HELPERS
public:
    inline bool IsImmovable()const { return physicalFlags.bDisableZ || physicalFlags.bInfiniteMass || physicalFlags.bDisableMoveForce; }
};

VALIDATE_SIZE(CPhysical, 0x138);
