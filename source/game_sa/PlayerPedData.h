/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"
#include "Vector2D.h"
#include <Ped.h>

class CEntity;
class CCopPed;
class CPedClothesDesc;
class CWanted;

class CPlayerPedData {
public:
    CWanted*         m_pWanted;
    CPedClothesDesc* m_pPedClothesDesc;
    CCopPed*         m_pArrestingCop;
    CVector2D        m_vecFightMovement;
    float            m_fMoveBlendRatio;
    float            m_fTimeCanRun;
    float            m_fMoveSpeed;
    uint8            m_nChosenWeapon;
    uint8            m_nCarDangerCounter;
    uint32           m_nStandStillTimer;
    uint32           m_nHitAnimDelayTimer;
    float            m_fAttackButtonCounter;
    void*            m_pDangerCar;
    union {
        struct {
            uint32 m_bStoppedMoving : 1;
            uint32 m_bAdrenaline : 1;
            uint32 m_bHaveTargetSelected : 1;             // Needed to work out whether we lost target this frame
            uint32 m_bFreeAiming : 1;
            uint32 m_bCanBeDamaged : 1;
            uint32 m_bAllMeleeAttackPtsBlocked : 1;       // if all of m_pMeleeAttackers[] is blocked by collision, just attack straight ahead
            uint32 m_bJustBeenSnacking : 1;               // If this bit is true we have just bought something from a vending machine
            uint32 m_bRequireHandleBreath : 1;

            uint32 m_bGroupStuffDisabled : 1;             // if this is true the player can't recrout or give his group commands.
            uint32 m_bGroupAlwaysFollow : 1;              // The group is told to always follow the player (used for girlfriend missions)
            uint32 m_bGroupNeverFollow : 1;               // The group is told to always follow the player (used for girlfriend missions)
            uint32 m_bInVehicleDontAllowWeaponChange : 1; // stop weapon change once driveby weapon has been given
            uint32 m_bRenderWeapon : 1;                   // set to false during cutscenes so that knuckledusters are not rendered
        };

        struct {
            uint8 m_bPlayerFlagsByte1;
            uint8 m_bPlayerFlagsByte2;
            uint8 m_bPlayerFlagsByte3;
            uint8 m_bPlayerFlagsByte4;
        };
        uint32 m_nPlayerFlags;
    };
    uint32    m_nPlayerGroup;
    uint32    m_nAdrenalineEndTime;
    uint8     m_nDrunkenness;
    uint8     m_nFadeDrunkenness;
    uint8     m_nDrugLevel;
    uint8     m_nScriptLimitToGangSize;
    float     m_fBreath;
    uint32    m_nMeleeWeaponAnimReferenced;
    uint32    m_nMeleeWeaponAnimReferencedExtra;
    float     m_fFPSMoveHeading;
    float     m_fLookPitch;
    float     m_fSkateBoardSpeed;
    float     m_fSkateBoardLean;
    RpAtomic* m_pSpecialAtomic;
    float     m_fGunSpinSpeed;
    float     m_fGunSpinAngle;
    uint32    m_nLastTimeFiring;
    uint32    m_nTargetBone;
    CVector   m_vecTargetBoneOffset;
    uint32    m_nBusFaresCollected;
    bool      m_bPlayerSprintDisabled;
    bool      m_bDontAllowWeaponChange;
    bool      m_bForceInteriorLighting;
    uint16    m_nPadDownPressedInMilliseconds;
    uint16    m_nPadUpPressedInMilliseconds;
    uint8     m_nWetness;
    bool      m_bPlayersGangActive;
    uint8     m_nWaterCoverPerc;
    float     m_fWaterHeight;
    uint32    m_nFireHSMissilePressedTime;
    CEntity*  m_LastHSMissileTarget;
    uint32    m_nModelIndexOfLastBuildingShot;
    uint32    m_nLastHSMissileLOSTime : 31;
    uint32    m_bLastHSMissileLOS : 1;
    CPed::Ref m_CurrProstitute;
    CPed::Ref m_LastProstitute;

public:
    static void InjectHooks();

    CPlayerPedData();
    CPlayerPedData * Constructor();

    ~CPlayerPedData();
    CPlayerPedData * Destructor();

    void AllocateData();
    void DeAllocateData();
    void SetInitialState();
};

VALIDATE_SIZE(CPlayerPedData, 0xAC);
