/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <extensions/EntityRef.hpp>

#include "Physical.h"
#include "AEPedAudioEntity.h"
#include "AEPedSpeechAudioEntity.h"
#include "AEPedWeaponAudioEntity.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "AnimBlendFrameData.h"
#include "Acquaintance.h"
#include "PedIK.h"
#include "Weapon.h"
#include "Vector2D.h"
#include "AnimBlendAssociation.h"
#include "Fire.h"
#include "PedGroups.h"

#include <Audio/Enums/PedSpeechContexts.h>
#include "AnimationEnums.h"
#include "eWeaponType.h"
#include "eWeaponSkill.h"
#include "ePedState.h"
#include "ePedStats.h"
#include "ePedType.h"
#include "eMoveState.h"

class CPedGroup;
class CCivilianPed;
class CEmergencyPed;
class CCoverPoint;
class CEntryExit;
class CAnimBlendClumpData;
struct RpHAnimHierarchy;

enum ePedNode : int32 {
    PED_NODE_NULL            = 0,
    PED_NODE_UPPER_TORSO     = 1,
    PED_NODE_HEAD            = 2,
    PED_NODE_LEFT_ARM        = 3,
    PED_NODE_RIGHT_ARM       = 4,
    PED_NODE_LEFT_HAND       = 5,
    PED_NODE_RIGHT_HAND      = 6,
    PED_NODE_LEFT_LEG        = 7,
    PED_NODE_RIGHT_LEG       = 8,
    PED_NODE_LEFT_FOOT       = 9,
    PED_NODE_RIGHT_FOOT      = 10,
    PED_NODE_RIGHT_LOWER_LEG = 11,
    PED_NODE_LEFT_LOWER_LEG  = 12,
    PED_NODE_LEFT_LOWER_ARM  = 13,
    PED_NODE_RIGHT_LOWER_ARM = 14,
    PED_NODE_LEFT_CLAVICLE   = 15,
    PED_NODE_RIGHT_CLAVICLE  = 16,
    PED_NODE_NECK            = 17,
    PED_NODE_JAW             = 18,

    TOTAL_PED_NODES
};

enum ePedPieceTypes {
    PED_COL_SPHERE_LEG  = 0,
    PED_COL_SPHERE_MID  = 1,
    PED_COL_SPHERE_HEAD = 2,
    PED_PIECE_TORSO     = 3, // AKA CHEST 
    PED_PIECE_ASS       = 4, // AKA MIDSECTION 
    PED_PIECE_LEFT_ARM  = 5, // AKA UPPERARM_L 
    PED_PIECE_RIGHT_ARM = 6, // AKA UPPERARM_R 
    PED_PIECE_LEFT_LEG  = 7, // AKA LEG_L 
    PED_PIECE_RIGHT_LEG = 8, // AKA LEG_R 
    PED_PIECE_HEAD      = 9 
};

enum ePedCreatedBy : uint8 {
    PED_UNKNOWN = 0,
    PED_GAME = 1,
    PED_MISSION = 2,
    PED_GAME_MISSION = 3, // used for the playbacked peds on replay
};

enum eFightingStyle : int8 {
    STYLE_STANDARD = 4,
    STYLE_BOXING,
    STYLE_KUNG_FU,
    STYLE_KNEE_HEAD,
    // various melee weapon styles
    STYLE_GRAB_KICK = 15,
    STYLE_ELBOWS = 16,
};

class CObject;
class CVehicle;
class CPedStat;
class CPedStats;

class NOTSA_EXPORT_VTABLE CPed : public CPhysical {
public:
    using Ref = notsa::EntityRef<CPed>;

    static inline int16 m_sGunFlashBlendStart = 10'000; // 0x8D1370

protected: // Use accessors
    CAEPedAudioEntity       m_pedAudio;
    CAEPedSpeechAudioEntity m_pedSpeech;
    CAEPedWeaponAudioEntity m_weaponAudio;
public:
    char                    field_43C[36];
    CPed*                   m_roadRageWith;
    char                    field_464[4];
    int32                   field_468;

    /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CPedSA.h */
    struct {
        // 1st byte starts here (m_nPedFlags)
        bool bIsStanding : 1 = false;            // 0 is ped standing on something
        bool bWasStanding : 1 = false;           // 1 was ped standing on something
        bool bIsLooking : 1 = false;             // 2 is ped looking at something or in a direction
        bool bIsRestoringLook : 1 = false;       // 3 is ped restoring head position from a look
        bool bIsAimingGun : 1 = false;           // 4 is ped aiming gun
        bool bIsRestoringGun : 1 = false;        // 5 is ped moving gun back to default posn
        bool bCanPointGunAtTarget : 1 = false;   // 6 can ped point gun at target
        bool bIsTalking : 1 = false;             // 7 is ped talking(see Chat())

        bool bInVehicle : 1 = false;             // 8  is in a vehicle [Sometimes accessed as `(ped->m_nPedFlags >> 8) & 1`]
        bool bIsInTheAir : 1 = false;            // 9  is in the air
        bool bIsLanding : 1 = false;             // 10 is landing after being in the air
        bool bHitSomethingLastFrame : 1 = false; // 11 has been in a collision last frame
        bool bIsNearCar : 1 = false;             // 12 has been in a collision last frame
        bool bRenderPedInCar : 1 = true;         // 13 has been in a collision last frame
        bool bUpdateAnimHeading : 1 = false;     // 14 update ped heading due to heading change during anim sequence
        bool bRemoveHead : 1 = false;            // 15 waiting on AntiSpazTimer to remove head - TODO: See `RemoveBodyPart` - The name seems to be incorrect. It should be like `bHasBodyPartToRemove`.

        bool bFiringWeapon : 1 = false;         // 16 is pulling trigger
        bool bHasACamera : 1;                   // 17 does ped possess a camera to document accidents
        bool bPedIsBleeding : 1 = false;        // 18 Ped loses a lot of blood if true
        bool bStopAndShoot : 1 = false;         // 19 Ped cannot reach target to attack with fist, need to use gun
        bool bIsPedDieAnimPlaying : 1 = false;  // 20 is ped die animation finished so can dead now
        bool bStayInSamePlace : 1 = false;      // 21 when set, ped stays put
        bool bKindaStayInSamePlace : 1 = false; // 22 when set, ped doesn't seek out opponent or cover large distances. Will still shuffle and look for cover
        bool bBeingChasedByPolice : 1 = false;  // 23 use nodes for route find

        bool bNotAllowedToDuck : 1 = false;     // 24 Is this ped allowed to duck at all?
        bool bCrouchWhenShooting : 1 = false;   // 25 duck behind cars etc
        bool bIsDucking : 1 = false;            // 26 duck behind cars etc
        bool bGetUpAnimStarted : 1 = false;     // 27 don't want to play getup anim if under something
        bool bDoBloodyFootprints : 1 = false;   // 28 bIsLeader
        bool bDontDragMeOutCar : 1 = false;     // 29
        bool bStillOnValidPoly : 1 = false;     // 30 set if the polygon the ped is on is still valid for collision
        bool bAllowMedicsToReviveMe : 1 = true; // 31

        // 5th byte starts here (m_nSecondPedFlags)
        bool bResetWalkAnims : 1 = false;
        bool bOnBoat : 1 = false;               // flee but only using nodes
        bool bBusJacked : 1 = false;            // flee but only using nodes
        bool bFadeOut : 1 = false;              // set if you want ped to fade out
        bool bKnockedUpIntoAir : 1 = false;     // has ped been knocked up into the air by a car collision
        bool bHitSteepSlope : 1 = false;        // has ped collided/is standing on a steep slope (surface type)
        bool bCullExtraFarAway : 1 = false;     // special ped only gets culled if it's extra far away (for roadblocks)
        bool bTryingToReachDryLand : 1 = false; // has ped just exited boat and trying to get to dry land

        bool bCollidedWithMyVehicle : 1 = false;
        bool bRichFromMugging : 1 = false;        // ped has lots of cash cause they've been mugging people
        bool bChrisCriminal : 1 = false;          // Is a criminal as killed during Chris' police mission (should be counted as such)
        bool bShakeFist : 1 = false;              // test shake hand at look entity
        bool bNoCriticalHits : 1 = false;         // ped cannot be killed by a single bullet
        bool bHasAlreadyBeenRecorded : 1 = false; // Used for replays
        bool bUpdateMatricesRequired : 1 = false; // if PedIK has altered bones so matrices need updated this frame
        bool bFleeWhenStanding : 1 = false;       //

        bool bMiamiViceCop : 1 = false;
        bool bMoneyHasBeenGivenByScript : 1 = false;
        bool bHasBeenPhotographed : 1 = false;
        bool bIsDrowning : 1 = false;
        bool bDrownsInWater : 1 = true;
        bool bHeadStuckInCollision : 1 = false;
        bool bDeadPedInFrontOfCar : 1 = false;
        bool bStayInCarOnJack : 1 = false;

        bool bDontFight : 1 = false;
        bool bDoomAim : 1 = true;
        bool bCanBeShotInVehicle : 1 = true;
        bool bPushedAlongByCar : 1 = false; // ped is getting pushed along by car collision (so don't take damage from horz velocity)
        bool bNeverEverTargetThisPed : 1 = false;
        bool bThisPedIsATargetPriority : 1 = false;
        bool bCrouchWhenScared : 1 = false;
        bool bKnockedOffBike : 1 = false; // TODO: Maybe rename to `bIsJumpingOut` or something similar, see x-refs

        // 9th byte starts here (m_nThirdPedFlags)
        bool bDonePositionOutOfCollision : 1 = false;
        bool bDontRender : 1 = false;
        bool bHasBeenAddedToPopulation : 1 = false;
        bool bHasJustLeftCar : 1 = false;
        bool bIsInDisguise : 1 = false;
        bool bDoesntListenToPlayerGroupCommands : 1 = false;
        bool bIsBeingArrested : 1 = false;
        bool bHasJustSoughtCover : 1 = false;

        bool bKilledByStealth : 1 = false;
        bool bDoesntDropWeaponsWhenDead : 1 = false;
        bool bCalledPreRender : 1 = false;
        bool bBloodPuddleCreated : 1 = false; // Has a static puddle of blood been created yet
        bool bPartOfAttackWave : 1 = false;
        bool bClearRadarBlipOnDeath : 1 = false;
        bool bNeverLeavesGroup : 1 = false;        // flag that we want to test 3 extra spheres on col model
        bool bTestForBlockedPositions : 1 = false; // this sets these indicator flags for various positions on the front of the ped

        bool bRightArmBlocked : 1 = false;
        bool bLeftArmBlocked : 1 = false;
        bool bDuckRightArmBlocked : 1 = false;
        bool bMidriffBlockedForJump : 1 = false;
        bool bFallenDown : 1 = false;
        bool bUseAttractorInstantly : 1 = false;
        bool bDontAcceptIKLookAts : 1 = false;
        bool bHasAScriptBrain : 1 = false;

        bool bWaitingForScriptBrainToLoad : 1 = false;
        bool bHasGroupDriveTask : 1 = false;
        bool bCanExitCar : 1 = true;
        bool CantBeKnockedOffBike : 2 = false; // (harder for mission peds)   normal(also for mission peds)
        bool bHasBeenRendered : 1 = false;
        bool bIsCached : 1 = false;
        bool bPushOtherPeds : 1 = false;   // GETS RESET EVERY FRAME - SET IN TASK: want to push other peds around (eg. leader of a group or ped trying to get in a car)

        // 13th byte starts here (m_nFourthPedFlags)
        bool bHasBulletProofVest : 1 = false;
        bool bUsingMobilePhone : 1 = false;
        bool bUpperBodyDamageAnimsOnly : 1 = false;
        bool bStuckUnderCar : 1 = false;
        bool bKeepTasksAfterCleanUp : 1 = false; // If true ped will carry on with task even after cleanup
        bool bIsDyingStuck : 1 = false;
        bool bIgnoreHeightCheckOnGotoPointTask : 1 = false; // set when walking round buildings, reset when task quits
        bool bForceDieInCar : 1 = false;

        bool bCheckColAboveHead : 1 = false;
        bool bIgnoreWeaponRange : 1 = false;
        bool bDruggedUp : 1 = false;
        bool bWantedByPolice : 1 = false; // if this is set, the cops will always go after this ped when they are doing a KillCriminal task
        bool bSignalAfterKill : 1 = true;
        bool bCanClimbOntoBoat : 1 = false;
        bool bPedHitWallLastFrame : 1 = false; // useful to store this so that AI knows (normal will still be available)
        bool bIgnoreHeightDifferenceFollowingNodes : 1 = false;

        bool bMoveAnimSpeedHasBeenSetByTask : 1 = false;
        bool bGetOutUpsideDownCar : 1 = true;
        bool bJustGotOffTrain : 1 = false;
        bool bDeathPickupsPersist : 1 = false;
        bool bTestForShotInVehicle : 1 = false;
        bool bUsedForReplay : 1 = false; // This ped is controlled by replay and should be removed when replay is done.
    };
    CPedIntelligence*   m_pIntelligence;
    CPlayerPedData*     m_pPlayerData;
    ePedCreatedBy       m_nCreatedBy;
    std::array<AnimBlendFrameData*, TOTAL_PED_NODES> m_apBones; // for Index, see ePedNode - TODO: Name incorrect, should be `m_apNodes` instead.
    AssocGroupId        m_nAnimGroup;
    CVector2D           m_vecAnimMovingShiftLocal;
    CAcquaintance       m_acquaintance;

    RpClump*            m_pWeaponObject;
    RwFrame*            m_pGunflashObject; // A frame in the Clump `m_pWeaponObject`
    RpClump*            m_pGogglesObject;
    bool*               m_pGogglesState;           // Stores a pointer to either `CPostEffects::m_bInfraredVision` or `m_bNightVision`, see \r PutOnGoggles and \r AddGogglesModel

    int16               m_nWeaponGunflashAlphaMP1; // AKA m_nWeaponGunflashStateRightHand
    int16               m_nWeaponGunFlashAlphaProgMP1;
    int16               m_nWeaponGunflashAlphaMP2; // AKA m_nWeaponGunflashStateLeftHand
    int16               m_nWeaponGunFlashAlphaProgMP2;

    CPedIK              m_pedIK;
    int32               field_52C;
    ePedState           m_nPedState;
    eMoveState          m_nMoveState;
    int32               m_nSwimmingMoveState; // type is eMoveState and used for swimming in CTaskSimpleSwim::ProcessPed
    int32               field_53C;
    float               m_fHealth;
    float               m_fMaxHealth;
    float               m_fArmour;
    uint32              m_nTimeTillWeNeedThisPed;
    CVector2D           m_vecAnimMovingShift;
    float               m_fCurrentRotation;
    float               m_fAimingRotation;
    float               m_fHeadingChangeRate;
    float               m_fMoveAnim; // not sure about the name here
    CEntity*            m_standingOnEntity;
    CVector             field_56C;
    CVector             field_578;
    CEntity*            m_pContactEntity;
    float               field_588;
    CVehicle*           m_pVehicle;         // Ped's vehicle - Only physically in it if `bInVehicle` is `true`.
    CVehicle*           m_VehDeadInFrontOf; // Set if `bDeadPedInFrontOfCar` 
    int32               field_594;
    ePedType            m_nPedType;
    CPedStat*           m_pStats;
    std::array<CWeapon, NUM_WEAPON_SLOTS> m_aWeapons;
    eWeaponType         m_nSavedWeapon;   // when we need to hide ped weapon, we save it temporary here
    eWeaponType         m_nDelayedWeapon; // 'delayed' weapon is like an additional weapon, f.e., simple cop has a nitestick as current and pistol as delayed weapons
    uint32              m_nDelayedWeaponAmmo;
    uint8               m_nActiveWeaponSlot;
    uint8               m_nWeaponShootingRate;
    uint8               m_nWeaponAccuracy;
    CEntity*            m_pTargetedObject;
    int32               field_720;
    int32               field_724;
    int32               field_728;
    eWeaponSkill        m_nWeaponSkill;
    eFightingStyle      m_nFightingStyle;
    char                m_nAllowedAttackMoves;
    uint8               field_72F; // taskId related? 0x4B5C47
    CFire*              m_pFire;
    float               m_fireDmgMult;
    CEntity*            m_pLookTarget;
    float               m_fLookDirection; // In RAD
    int32               m_nWeaponModelId;
    int32               field_744;
    uint32              m_nLookTime;
    int32               field_74C;
    int32               m_nDeathTimeMS; //< Death time in MS (CTimer::GetTimeMS())
    char                m_nBodypartToRemove;
    char                field_755;
    int16               m_nMoneyCount; // Used for money pickup when ped is killed
    float               m_Wobble;
    float               m_WobbleSpeed;
    char                m_nLastWeaponDamage; // See eWeaponType
    CEntity*            m_pLastEntityDamage;
    int32               field_768;

    // TODO: Not turret, but rather attached entity, see `AttachPedToEntity` and `AttachPedToBike`
    CVector             m_vecTurretOffset;
    uint16              m_fTurretAngleA;
    float               m_fTurretAngleB;
    float               m_nTurretPosnMode;
    int32               m_nTurretAmmo;
    // **

    CCoverPoint*        m_pCoverPoint;
    CEntryExit*         m_pEnex; // CEnEx *
    float               m_fRemovalDistMultiplier;
    int16               m_StreamedScriptBrainToLoad;
    int32               field_798;

public:
    void SetModelIndex(uint32 modelIndex) override;
    void DeleteRwObject() override;
    void ProcessControl() override;
    void Teleport(CVector destination, bool resetRotation) override;
    void SpecialEntityPreCollisionStuff(CPhysical* colPhysical, bool bIgnoreStuckCheck, bool& bCollisionDisabled, bool& bCollidedEntityCollisionIgnored, bool& bCollidedEntityUnableToMove, bool& bThisOrCollidedEntityStuck) override;
    uint8 SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2) override;
    void PreRender() override;
    void Render() override;
    bool SetupLighting() override;
    void RemoveLighting(bool bRemove) override;
    void FlagToDestroyWhenNextProcessed() override;
    int32 ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) override;

    // Process applied anim 0x86C3B4
    virtual void SetMoveAnim();
    // always returns true 0x86C3B8
    virtual bool Save();
    // always returns true 0x86C3BC
    virtual bool Load();

    // class functions
    static void InjectHooks();

    static void* operator new(unsigned size);
    static void* operator new(unsigned size, int32 poolRef);
    static void operator delete(void* data);
    static void operator delete(void* data, int poolRef);

    CPed(ePedType pedType);
    ~CPed();

    bool PedIsInvolvedInConversation();
    bool PedIsReadyForConversation(bool arg0);
    static bool PedCanPickUpPickUp();
    void CreateDeadPedMoney();
    void CreateDeadPedPickupCoors(float& outPickupX, float& outPickupY, float& outPickupZ);
    void CreateDeadPedWeaponPickups();
    static void Initialise();
    void SetPedStats(ePedStats statsType);
    void Update();
    void SetMoveState(eMoveState moveState);
    void SetMoveAnimSpeed(CAnimBlendAssociation* association);
    void StopNonPartialAnims();
    void RestartNonPartialAnims();
    bool CanUseTorsoWhenLooking() const;
    void SetLookFlag(float lookHeading, bool likeUnused, bool arg2);
    void SetLookFlag(CEntity* lookingTo, bool likeUnused, bool arg2);
    void SetAimFlag(CEntity* aimingTo);
    void ClearAimFlag();
    uint8 GetLocalDirection(const CVector2D& point) const;
    bool IsPedShootable() const;
    bool UseGroundColModel() const;
    bool CanPedReturnToState() const;
    bool CanSetPedState() const;
    bool CanBeArrested() const;
    bool CanStrafeOrMouseControl() const;
    bool CanBeDeleted();
    bool CanBeDeletedEvenInVehicle() const;
    void RemoveGogglesModel();
    int32 GetWeaponSlot(eWeaponType weaponType);
    void GrantAmmo(eWeaponType weaponType, uint32 ammo);
    void SetAmmo(eWeaponType weaponType, uint32 ammo);
    bool DoWeHaveWeaponAvailable(eWeaponType weaponType);
    bool DoGunFlash(int32 lifetime, bool bRightHand);
    void SetGunFlashAlpha(bool rightHand);
    void ResetGunFlashAlpha();
    float GetBikeRidingSkill() const;
    static void ShoulderBoneRotation(RpClump* clump);
    void SetLookTimer(uint32 time);
    bool IsPlayer() const;
    void SetPedPositionInCar();
    void RestoreHeadingRate();
    static void RestoreHeadingRateCB(CAnimBlendAssociation* association, void* data);
    void SetRadioStation();
    void PositionAttachedPed();
    void Undress(char* modelName);
    void Dress();
    bool IsAlive() const;
    void UpdateStatEnteringVehicle();
    void UpdateStatLeavingVehicle();
    void GetTransformedBonePosition(RwV3d& inOutPos, eBoneTag boneId, bool updateSkinBones = false);
    void ReleaseCoverPoint();
    CTaskSimpleHoldEntity* GetHoldingTask();
    CEntity* GetEntityThatThisPedIsHolding();
    void DropEntityThatThisPedIsHolding(bool bDeleteHeldEntity);
    bool CanThrowEntityThatThisPedIsHolding();
    bool IsPlayingHandSignal();
    void StopPlayingHandSignal();
    float GetWalkAnimSpeed();
    void SetPedDefaultDecisionMaker();
    // limitAngle in radians
    bool CanSeeEntity(CEntity* entity, float limitAngle);
    bool PositionPedOutOfCollision(int32 exitDoor, CVehicle* vehicle, bool findClosestNode);
    bool PositionAnyPedOutOfCollision();
    bool OurPedCanSeeThisEntity(CEntity* entity, bool isSpotted);
    void SortPeds(CPed** pedList, int32 arg1, int32 arg2);
    void ClearLookFlag();
    float WorkOutHeadingForMovingFirstPerson(float heading);
    void UpdatePosition();
    void ProcessBuoyancy();
    bool IsPedInControl() const;
    void RemoveWeaponModel(int32 modelIndex = MODEL_INVALID);
    void AddGogglesModel(int32 modelIndex, bool& inOutGogglesState);
    void PutOnGoggles();
    eWeaponSkill GetWeaponSkill(eWeaponType weaponType);
    void SetWeaponSkill(eWeaponType weaponType, eWeaponSkill skill);
    void ClearLook();
    bool TurnBody();
    bool IsPointerValid();
    CVector GetBonePosition(eBoneTag boneId, bool updateSkinBones = false);
    void GetBonePosition(CVector* outVec, eBoneTag bone, bool updateSkinBones);
    void GiveObjectToPedToHold(int32 modelIndex, uint8 replace);
    void SetPedState(ePedState pedState);
    ePedState GetPedState() const { return m_nPedState; }
    //1 = default, 2 = scm/mission script
    void SetCharCreatedBy(ePedCreatedBy createdBy);
    void CalculateNewVelocity();
    void CalculateNewOrientation();
    void ClearAll();
    void DoFootLanded(bool leftFoot, uint8 arg1);
    void PlayFootSteps();
    void AddWeaponModel(int32 modelIndex);
    void TakeOffGoggles();
    eWeaponSlot GiveWeapon(eWeaponType weaponType, uint32 ammo, bool likeUnused);
    void GiveWeaponSet1();
    void GiveWeaponSet2();
    void GiveWeaponSet3();
    void GiveWeaponSet4();
    void SetCurrentWeapon(int32 slot);
    void SetCurrentWeapon(eWeaponType weaponType);
    void ClearWeapon(eWeaponType weaponType);
    void ClearWeapons();
    void RemoveWeaponWhenEnteringVehicle(int32 arg0);
    void ReplaceWeaponWhenExitingVehicle();
    void ReplaceWeaponForScriptedCutscene();
    void RemoveWeaponForScriptedCutscene();
    eWeaponSkill GetWeaponSkill();
    void PreRenderAfterTest();
    void SetIdle();
    void SetLook(float heading);
    void SetLook(CEntity* entity);
    void Look();
    CEntity* AttachPedToEntity(CEntity* entity, CVector offset, uint16 arg2, float arg3, eWeaponType weaponType);
    void AttachPedToBike(CEntity* entity, CVector offset, uint16 arg2, float arg3, float arg4, eWeaponType weaponType);
    void DettachPedFromEntity();
    void SetAimFlag(float heading);
    bool CanWeRunAndFireWithWeapon();
    void RequestDelayedWeapon();
    void GiveDelayedWeapon(eWeaponType weaponType, uint32 ammo);
    void GiveWeaponAtStartOfFight();
    void GiveWeaponWhenJoiningGang();
    bool GetPedTalking();
    void DisablePedSpeech(bool stopCurrentSpeech);
    void EnablePedSpeech();
    void DisablePedSpeechForScriptSpeech(bool stopCurrentSpeech);
    void EnablePedSpeechForScriptSpeech();
    bool CanPedHoldConversation() const;
    void SayScript(eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd);
    int16 Say(eGlobalSpeechContext gCtx, uint32 startTimeDelay = 0, float probability = 1.f, bool overrideSilence = false, bool isForceAudible = false, bool isFrontEnd = false);
    void RemoveBodyPart(ePedNode pedNode, char localDir);
    void SpawnFlyingComponent(int32 arg0, char arg1);
    uint8 DoesLOSBulletHitPed(CColPoint& colPoint);
    void RemoveWeaponAnims(int32 likeUnused, float blendDelta);
    bool IsPedHeadAbovePos(float zPos);
    void KillPedWithCar(CVehicle* car, float fDamageIntensity, bool bPlayDeadAnimation);
    template<typename PtrListType>
    void MakeTyresMuddySectorList(PtrListType& ptrList);
    void DeadPedMakesTyresBloody();
    bool IsInVehicleThatHasADriver();
    void SetStayInSamePlace(bool enable) { bStayInSamePlace = enable; }
    bool IsWearingGoggles() const { return !!m_pGogglesObject; }

    // NOTSA helpers
    void SetArmour(float v) { m_fArmour = v; }
    void SetWeaponShootingRange(uint8 r) { m_nWeaponShootingRate = r; }
    void SetWeaponAccuracy(uint8 acc) { m_nWeaponAccuracy = acc; }

    CAcquaintance& GetAcquaintance() { return m_acquaintance; }
    CVehicle* GetVehicleIfInOne() const { return bInVehicle ? m_pVehicle : nullptr; }

    uint8 GetCreatedBy() const { return m_nCreatedBy; }
    void SetCreatedBy(ePedCreatedBy v) { m_nCreatedBy = v; }
    bool IsCreatedBy(ePedCreatedBy v) const noexcept { return v == m_nCreatedBy; }
    bool IsCreatedByMission() const noexcept { return IsCreatedBy(ePedCreatedBy::PED_MISSION); }

    CPedGroup* GetGroup() const { return CPedGroups::GetPedsGroup(this); }
    int32 GetGroupId();
    CPedClothesDesc* GetClothesDesc() { return m_pPlayerData->m_pPedClothesDesc; }

    CPedIntelligence* GetIntelligence() { return m_pIntelligence; }
    CPedIntelligence* GetIntelligence() const { return m_pIntelligence; }
    CTaskManager& GetTaskManager() { return m_pIntelligence->m_TaskMgr; }
    CTaskManager& GetTaskManager() const { return m_pIntelligence->m_TaskMgr; }
    CEventGroup& GetEventGroup() { return m_pIntelligence->m_eventGroup; }
    CEventHandler& GetEventHandler() { return m_pIntelligence->m_eventHandler; }
    CEventHandlerHistory& GetEventHandlerHistory() { return m_pIntelligence->m_eventHandler.GetHistory(); }
    CPedStuckChecker& GetStuckChecker() { return m_pIntelligence->m_pedStuckChecker; }

    CWeapon& GetWeaponInSlot(size_t slot) noexcept { return m_aWeapons[slot]; }
    CWeapon& GetWeaponInSlot(eWeaponSlot slot) noexcept { return m_aWeapons[(size_t)slot]; }
    CWeapon& GetActiveWeapon() noexcept { return GetWeaponInSlot(m_nActiveWeaponSlot); }
    CWeapon& GetWeapon(eWeaponType wt) noexcept { return GetWeaponInSlot(GetWeaponSlot(wt)); }

    void SetSavedWeapon(eWeaponType weapon) { m_nSavedWeapon = weapon; }
    bool IsStateDriving() const noexcept { return m_nPedState == PEDSTATE_DRIVING; }
    bool IsStateDead() const noexcept { return m_nPedState == PEDSTATE_DEAD; }
    bool IsStateDying() const noexcept { return m_nPedState == PEDSTATE_DEAD || m_nPedState == PEDSTATE_DIE; }
    bool IsStateDeadForScript()  const noexcept { return m_nPedState == PEDSTATE_DEAD || m_nPedState == PEDSTATE_DIE || m_nPedState == PEDSTATE_DIE_BY_STEALTH; }
    bool IsInVehicleAsPassenger() const noexcept;

    bool IsCop()      const noexcept { return m_nPedType == PED_TYPE_COP; }
    bool IsGangster() const noexcept { return IsPedTypeGang(m_nPedType); }
    bool IsCivilian() const noexcept { return m_nPedType == PED_TYPE_CIVMALE || m_nPedType == PED_TYPE_CIVFEMALE; }

    CCopPed*       AsCop()       { return reinterpret_cast<CCopPed*>(this); }
    CCivilianPed*  AsCivilian()  { return reinterpret_cast<CCivilianPed*>(this); }
    CEmergencyPed* AsEmergency() { return reinterpret_cast<CEmergencyPed*>(this); }
    CPlayerPed*    AsPlayer()    { return reinterpret_cast<CPlayerPed*>(this); }

    bool IsFollowerOfGroup(const CPedGroup& group) const;
    RwMatrix* GetBoneMatrix(eBoneTag bone) const;
    void CreateDeadPedPickupCoors(CVector& pickupPos);
    RpHAnimHierarchy& GetAnimHierarchy() const;
    CAnimBlendClumpData& GetAnimBlendData() const;
    bool IsInVehicle() const { return bInVehicle && m_pVehicle; }
    bool IsInVehicle(const CVehicle* veh) const { return bInVehicle && m_pVehicle == veh; }
    int32 GetPadNumber() const;
    bool IsCurrentlyUnarmed() { return GetActiveWeapon().m_Type == WEAPON_UNARMED; }

    auto&& GetAE(this auto&& self)    { return self.m_pedAudio; }
    auto&& GetSpeechAE(this auto&& self) { return self.m_pedSpeech; }
    auto&& GetWeaponAE(this auto&& self) { return self.m_weaponAudio; }

    /*!
     * @notsa
     * @brief Is the ped jogging, running or sprinting
     */
    bool IsJoggingOrFaster() const;

    /*!
     * @notsa
     * @brief Is the ped running or sprinting
     */
    bool IsRunningOrSprinting() const;

    /*!
     * @notsa
     * @brief Is the ped standing in place (might still be moving, but in place)
     */
    bool IsPedStandingInPlace() const;

    /*!
     * @notsa
     * @brief Is the ped's right arm blocked right now
     */
    bool IsRightArmBlockedNow() const;

    /*!
     * @notsa
     * @brief Give weapon according to given CWeapon struct.
     */
    eWeaponSlot GiveWeapon(const CWeapon& weapon, bool likeUnused) {
        return GiveWeapon(weapon.m_Type, weapon.m_TotalAmmo, likeUnused);
    }

    auto GetPedModelInfo() const { return reinterpret_cast<CPedModelInfo*>(GetModelInfo()); }

    /*!
     * @notsa
     * @brief Returns vehicle's position if ped is in one, ped's otherwise.
     */
    CVector GetRealPosition() const { return IsInVehicle() ? m_pVehicle->GetPosition() : GetPosition(); }

private:
    void RenderThinBody() const;
    void RenderBigHead() const;

private:
    // Virtual method wrappers
    auto Constructor(ePedType pt) { this->CPed::CPed(pt); return this; }
    auto Destructor() { this->CPed::~CPed(); return this; }
};
VALIDATE_SIZE(CPed, 0x79C);

RwObject* SetPedAtomicVisibilityCB(RwObject* rwObject, void* data);
bool IsPedPointerValid(CPed* ped);
bool SayJacked(CPed* jacked, CVehicle* vehicle, uint32 offset = 0);
bool SayJacking(CPed* jacker, CPed* jacked, CVehicle* vehicle, uint32 offset = 0);
