/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Vector.h"

class FxSystem_c;

enum eObjectColDamageEffect : uint8 {
    COL_DAMAGE_EFFECT_NONE = 0,
    COL_DAMAGE_EFFECT_CHANGE_MODEL = 1,
    COL_DAMAGE_EFFECT_SMASH_COMPLETELY = 20,
    COL_DAMAGE_EFFECT_CHANGE_THEN_SMASH = 21,
    COL_DAMAGE_EFFECT_BREAKABLE = 200,
    COL_DAMAGE_EFFECT_BREAKABLE_REMOVED = 202 // (i.e. never regenerated after destroyed)
};

enum eObjectSpecialColResponseCases : uint8 {
    COL_SPECIAL_RESPONSE_NONE = 0,
    COL_SPECIAL_RESPONSE_LAMPOST,
    COL_SPECIAL_RESPONSE_SMALLBOX,
    COL_SPECIAL_RESPONSE_BIGBOX,
    COL_SPECIAL_RESPONSE_FENCEPART,
    COL_SPECIAL_RESPONSE_GRENADE,
    COL_SPECIAL_RESPONSE_SWINGDOOR,
    COL_SPECIAL_RESPONSE_LOCKDOOR,
    COL_SPECIAL_RESPONSE_HANGING,
    COL_SPECIAL_RESPONSE_OB_COL_POOLBALL,
};

enum eObjectFxType : uint8 {
    NO_FX,
    PLAY_ON_HIT,
    PLAY_ON_DESTROYED,
    PLAY_ON_HIT_DESTROYED
};

enum eObjectBreakMode : uint32 {
    NOT_BY_GUN,
    BY_GUN,
    SMASHABLE,
};

enum eObjectCameraAvoidType : uint8 {
    CAMERA_IGNORE = 0,
    CAMERA_COLLIDE = 1,
    CAMERA_UNKNOWN = 2,
};

class CObjectData {
public:
    float         m_fMass;
    float         m_fTurnMass;
    float         m_fAirResistance;
    float         m_fElasticity;
    float         m_fBuoyancyConstant;
    float         m_fUprootLimit;
    float         m_fColDamageMultiplier;
    uint8         m_nColDamageEffect;        // see eObjectColDamageEffect
    uint8         m_nSpecialColResponseCase; // see eObjectSpecialColResponseCases
    uint8         m_nCameraAvoidObject;      // see eObjectCameraAvoidType
    uint8         m_bCausesExplosion;
    uint8         m_nFxType; // see eObjectFxType
    CVector       m_vFxOffset;
    FxSystemBP_c* m_pFxSystemBP;
    float         m_fSmashMultiplier;
    CVector       m_vecBreakVelocity;
    float         m_fBreakVelocityRand;
    uint32        m_nGunBreakMode; // see eObjectBreakMode
    uint32        m_nSparksOnImpact;

public:
    static constexpr int32 NUM_OBJECT_INFOS = 160;
    static inline auto (&ms_aObjectInfo)[NUM_OBJECT_INFOS] = StaticRef<CObjectData[NUM_OBJECT_INFOS]>(0xBB4A90);

public:
    static void InjectHooks();

    static void Initialise(const char* fileName);
    static void SetObjectData(int32 dwModelIndex, CObject& obj);

    bool operator==(const CObjectData& right) const;

public:
    inline static CObjectData& GetAtIndex(int32 index) { return ms_aObjectInfo[index]; }
    inline static constexpr CObjectData& GetDefault() { return ms_aObjectInfo[0]; }
};

VALIDATE_SIZE(CObjectData, 0x50);
