/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Physical.h"
#include "ObjectData.h"

enum eObjectType {
    OBJECT_UNKNOWN         = 0,
    OBJECT_GAME            = 1,
    OBJECT_MISSION         = 2,
    OBJECT_TEMPORARY       = 3, // AKA OBJECT_TYPE_FLYING_COMPONENT
    OBJECT_TYPE_CUTSCENE   = 4,
    OBJECT_TYPE_DECORATION = 5, // Hand object, projectiles, escalator step, water creatures, no clue what this enum value should be called
    OBJECT_MISSION2        = 6
};

class CDummyObject;
class CFire;

class NOTSA_EXPORT_VTABLE CObject : public CPhysical {
public:
    CPtrNodeDoubleLink<CObject*>* m_pControlCodeList;
    uint8                         m_nObjectType; // see enum eObjectType
    uint8                         m_nBonusValue;
    uint16                        m_wCostValue;
    union {
        struct {
            uint32 bIsPickup : 1;               // 0x1
            uint32 b0x02 : 1;                   // 0x2 - collision related
            uint32 bPickupPropertyForSale : 1;  // 0x4
            uint32 bPickupInShopOutOfStock : 1; // 0x8
            uint32 bHasBrokenGlass : 1;         // 0x10
            uint32 bGlassBrokenAltogether : 1;  // 0x20
            uint32 bIsExploded : 1;             // 0x40
            uint32 bChangesVehColor : 1;        // 0x80

            uint32 bIsLampPost : 1;
            uint32 bIsTargetable : 1;
            uint32 bIsBroken : 1;
            uint32 bTrainCrossEnabled : 1;
            uint32 bIsPhotographed : 1;
            uint32 bIsLiftable : 1;
            uint32 bIsDoorMoving : 1;
            uint32 bIsDoorOpen : 1;

            uint32 bHasNoModel : 1;
            uint32 bIsScaled : 1;
            uint32 bCanBeAttachedToMagnet : 1;
            uint32 bDamaged : 1;
            uint32 b0x100000_0x200000 : 2; // something something scripts for brains
            uint32 bFadingIn : 1; // works only for objects with type 2 (OBJECT_MISSION)
            uint32 bAffectedByColBrightness : 1;

            uint32 bEnableDisabledAttractors : 1;
            uint32 bDoNotRender : 1;
            uint32 bFadingIn2 : 1;
            uint32 b0x08000000 : 1;
            uint32 b0x10000000 : 1;
            uint32 b0x20000000 : 1;
            uint32 b0x40000000 : 1;
            uint32 b0x80000000 : 1;
        } objectFlags;
        uint32 m_nObjectFlags;
    };
    uint8         m_nColDamageEffect;        // see eObjectColDamageEffect
    uint8         m_nSpecialColResponseCase; // see eObjectSpecialColResponseCases
    char          field_146;
    int8          m_nGarageDoorGarageIndex;
    uint8         m_nLastWeaponDamage;
    tColLighting  m_nColLighting;
    int16         m_nRefModelIndex;
    uint8         m_nCarColor[4];  // this is used for detached car parts
    uint32        m_nRemovalTime;  // time when this object must be deleted
    float         m_fHealth;
    float         m_fDoorStartAngle; // this is used for door objects
    float         m_fScale;
    CObjectData*  m_pObjectInfo;
    CFire*        m_pFire;
    int16         m_nStreamedScriptBrainToLoad;
    int16         m_wRemapTxd;     // this is used for detached car parts
    RwTexture*    m_pRemapTexture; // this is used for detached car parts
    CDummyObject* m_pDummyObject;  // used for dynamic objects like garage doors, train crossings etc.
    uint32        m_nBurnTime;     // time when particles must be stopped
    float         m_fBurnDamage;

    static uint16& nNoTempObjects;
    static float&  fDistToNearestTree;
    static bool&   bAircraftCarrierSamSiteDisabled;
    static bool&   bArea51SamSiteDisabled;

public:
    CObject();
    CObject(int32 modelId, bool bCreate);
    explicit CObject(CDummyObject* dummyObj);
    ~CObject() override;

    static void* operator new(size_t size);
    static void* operator new(size_t size, int32 poolRef);
    static void operator delete(void* obj);
    static void operator delete(void* obj, int32 poolRef);

    void  SetIsStatic(bool isStatic) override;
    void  CreateRwObject() override;
    void  ProcessControl() override;
    void  Teleport(CVector destination, bool resetRotation) override;
    void  SpecialEntityPreCollisionStuff(CPhysical* colPhysical, bool bIgnoreStuckCheck, bool& bCollisionDisabled, bool& bCollidedEntityCollisionIgnored, bool& bCollidedEntityUnableToMove, bool& bThisOrCollidedEntityStuck) override;
    uint8 SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2) override;
    void  PreRender() override;
    void  Render() override;
    bool  SetupLighting() override;
    void  RemoveLighting(bool bRemove) override;

    bool Load();
    bool Save();

    void     ProcessGarageDoorBehaviour();
    [[nodiscard]] bool CanBeDeleted() const;
    void     SetRelatedDummy(CDummyObject* relatedDummy);
    bool     TryToExplode();
    void     SetObjectTargettable(bool targetable);
    [[nodiscard]] bool CanBeTargetted() const;
    void     RefModelInfo(int32 modelIndex);
    void     SetRemapTexture(RwTexture* remapTexture, int16 txdIndex);
    float    GetRopeHeight();
    void     SetRopeHeight(float height);
    CEntity* GetObjectCarriedWithRope();
    void     ReleaseObjectCarriedWithRope();
    void     AddToControlCodeList();
    void     RemoveFromControlCodeList();
    void     ResetDoorAngle();
    void     LockDoor();
    void     Init();
    void     DoBurnEffect() const;
    void     GetLightingFromCollisionBelow();
    void     ProcessSamSiteBehaviour();
    void     ProcessTrainCrossingBehaviour();
    void     ObjectDamage(float damage, const CVector* fxOrigin, const CVector* fxDirection, CEntity* damager, eWeaponType weaponType);
    void     Explode();
    void     ObjectFireDamage(float damage, CEntity* damager);

    void GrabObjectToCarryWithRope(CPhysical* attachTo);
    bool CanBeUsedToTakeCoverBehind();
    void ProcessControlLogic();

    static CObject* Create(int32 modelIndex, bool bUnused);
    static CObject* Create(CDummyObject* dummyObject);

    static void SetMatrixForTrainCrossing(CMatrix* matrix, float fAngle);
    static void TryToFreeUpTempObjects(int32 numObjects);
    static void DeleteAllTempObjects();
    static void DeleteAllMissionObjects();
    static void DeleteAllTempObjectsInArea(CVector point, float radius);

    // Helpers
    [[nodiscard]] bool IsTemporary() const     { return m_nObjectType == OBJECT_TEMPORARY; }
    [[nodiscard]] bool IsMissionObject() const { return m_nObjectType == OBJECT_MISSION || m_nObjectType == OBJECT_MISSION2; }
    [[nodiscard]] bool IsCraneMovingPart() const;
    [[nodiscard]] bool IsFallenLampPost() const { return objectFlags.bIsLampPost && m_matrix->GetUp().z < 0.66F; }
    [[nodiscard]] bool IsExploded() const       { return objectFlags.bIsExploded; }
    [[nodiscard]] bool CanBeSmashed() const     { return m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};
VALIDATE_SIZE(CObject, 0x17C);

bool IsObjectPointerValid_NotInWorld(CObject* object);
bool IsObjectPointerValid(CObject* object);
