/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Ped.h"

#include "PedType.h"
#include "Buoyancy.h"
#include "TaskSimpleSwim.h"
#include "PedStats.h"
#include "Conversations.h"
#include "TaskSimpleLand.h"
#include "AEAudioUtility.h"
#include "PedClothesDesc.h"
#include "EventAcquaintancePedHate.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexGoPickUpEntity.h"
#include "Radar.h"
#include "PostEffects.h"
#include "PedStdBonePositions.h"
#include "TaskSimpleJetPack.h"
#include "PedSaveStructure.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFacial.h"
#include "WeaponInfo.h"
#include "Shadows.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "RealTimeShadowManager.h"
#include "WindModifiers.h"

void CPed::InjectHooks() {
    RH_ScopedVirtualClass(CPed, 0x86C358, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Constructor, 0x5E8030);
    RH_ScopedInstall(Destructor, 0x5E8620);

    RH_ScopedInstall(RequestDelayedWeapon, 0x5E8910);
    RH_ScopedInstall(DettachPedFromEntity, 0x5E7EC0);
    RH_ScopedInstall(AttachPedToBike, 0x5E7E60);
    RH_ScopedInstall(AttachPedToEntity, 0x5E7CB0);
    RH_ScopedInstall(OurPedCanSeeThisEntity, 0x5E1660);
    RH_ScopedOverloadedInstall(operator delete, "anon", 0x5E4760, void (*)(void*));
    RH_ScopedOverloadedInstall(operator new, "anon", 0x5E4720, void* (*)(unsigned));
    RH_ScopedOverloadedInstall(operator new, "poolIndexed", 0x5E4730, void* (*)(unsigned, int32));
    RH_ScopedInstall(SpawnFlyingComponent, 0x5F0190);
    RH_ScopedInstall(PedCanPickUpPickUp, 0x455560);
    RH_ScopedInstall(Update, 0x5DEBE0);
    RH_ScopedInstall(Initialise, 0x5DEBB0);
    RH_ScopedInstall(UpdateStatLeavingVehicle, 0x5E01B0);
    RH_ScopedInstall(UpdateStatEnteringVehicle, 0x5E01A0);
    RH_ScopedInstall(ShoulderBoneRotation, 0x5DF560);
    RH_ScopedInstall(RestoreHeadingRateCB, 0x5DFD70);
    RH_ScopedInstall(PedIsInvolvedInConversation, 0x43AB90);
    RH_ScopedInstall(ClearWeapons, 0x5E6320);
    RH_ScopedInstall(ClearWeapon, 0x5E62B0);
    RH_ScopedOverloadedInstall(SetCurrentWeapon, "WepType", 0x5E6280, void(CPed::*)(eWeaponType));
    RH_ScopedOverloadedInstall(SetCurrentWeapon, "Slot", 0x5E61F0, void(CPed::*)(int32));
    RH_ScopedOverloadedInstall(GiveWeapon, "", 0x5E6080, eWeaponSlot(CPed::*)(eWeaponType, uint32, bool));
    RH_ScopedInstall(TakeOffGoggles, 0x5E6010);
    RH_ScopedInstall(AddWeaponModel, 0x5E5ED0);
    RH_ScopedInstall(PlayFootSteps, 0x5E57F0, { .reversed = false });
    RH_ScopedInstall(DoFootLanded, 0x5E5380, { .reversed = false });
    RH_ScopedInstall(ClearAll, 0x5E5320);
    RH_ScopedInstall(CalculateNewOrientation, 0x5E52E0);
    RH_ScopedInstall(CalculateNewVelocity, 0x5E4C50, { .reversed = false });
    RH_ScopedInstall(SetCharCreatedBy, 0x5E47E0);
    RH_ScopedInstall(SetPedState, 0x5E4500);
    RH_ScopedInstall(GiveObjectToPedToHold, 0x5E4390);
    RH_ScopedInstall(ClearLookFlag, 0x5E1950);
    RH_ScopedInstall(WorkOutHeadingForMovingFirstPerson, 0x5E1A00);
    RH_ScopedInstall(UpdatePosition, 0x5E1B10, { .reversed = false });
    RH_ScopedInstall(MakeTyresMuddySectorList<CPtrListSingleLink>, 0x6AE0D0, { .reversed = false });
    RH_ScopedInstall(IsPedInControl, 0x5E3960);
    RH_ScopedInstall(RemoveWeaponModel, 0x5E3990);
    RH_ScopedInstall(RemoveWeaponWhenEnteringVehicle, 0x5E6370);
    RH_ScopedInstall(AddGogglesModel, 0x5E3A90);
    RH_ScopedInstall(SetWeaponSkill, 0x5E3C10);
    RH_ScopedInstall(ClearLook, 0x5E3FF0);
    RH_ScopedInstall(TurnBody, 0x5E4000);
    RH_ScopedInstall(IsPointerValid, 0x5E4220);
    RH_ScopedOverloadedInstall(GetBonePosition, "Original", 0x5E4280, void(CPed::*)(CVector*, eBoneTag, bool));
    RH_ScopedInstall(PutOnGoggles, 0x5E3AE0);
    RH_ScopedInstall(ReplaceWeaponWhenExitingVehicle, 0x5E6490);
    RH_ScopedInstall(KillPedWithCar, 0x5F0360, { .reversed = false });
    RH_ScopedInstall(IsPedHeadAbovePos, 0x5F02C0);
    RH_ScopedInstall(RemoveWeaponAnims, 0x5F0250);
    RH_ScopedInstall(DoesLOSBulletHitPed, 0x5F01A0);
    RH_ScopedInstall(RemoveBodyPart, 0x5F0140);
    RH_ScopedInstall(Say, 0x5EFFE0);
    RH_ScopedInstall(SayScript, 0x5EFFB0);
    RH_ScopedInstall(CanPedHoldConversation, 0x5EFFA0);
    RH_ScopedInstall(EnablePedSpeechForScriptSpeech, 0x5EFF90);
    RH_ScopedInstall(DisablePedSpeechForScriptSpeech, 0x5EFF80);
    RH_ScopedInstall(EnablePedSpeech, 0x5EFF70);
    RH_ScopedInstall(DisablePedSpeech, 0x5EFF60);
    RH_ScopedInstall(GetPedTalking, 0x5EFF50);
    RH_ScopedInstall(GiveWeaponWhenJoiningGang, 0x5E8BE0);
    RH_ScopedInstall(GiveDelayedWeapon, 0x5E89B0);
    RH_ScopedOverloadedInstall(GetWeaponSkill, "Current", 0x5E6580, eWeaponSkill(CPed::*)());
    RH_ScopedOverloadedInstall(GetWeaponSkill, "WeaponType", 0x5E3B60, eWeaponSkill(CPed::*)(eWeaponType));
    RH_ScopedInstall(PreRenderAfterTest, 0x5E65A0);
    RH_ScopedInstall(SetIdle, 0x5E7980);
    RH_ScopedOverloadedInstall(SetLook, "Heading", 0x5E79B0, void(CPed::*)(float));
    RH_ScopedOverloadedInstall(SetLook, "Entity", 0x5E7A60, void(CPed::*)(CEntity *));
    RH_ScopedInstall(Look, 0x5E7B20);
    RH_ScopedInstall(ReplaceWeaponForScriptedCutscene, 0x5E6530);
    RH_ScopedInstall(RemoveWeaponForScriptedCutscene, 0x5E6550);
    RH_ScopedInstall(GiveWeaponAtStartOfFight, 0x5E8AB0);
    RH_ScopedInstall(ProcessBuoyancy, 0x5E1FA0);
    RH_ScopedInstall(PositionPedOutOfCollision, 0x5E0820, { .reversed = false });
    RH_ScopedInstall(GrantAmmo, 0x5DF220);
    RH_ScopedInstall(GetWeaponSlot, 0x5DF200);
    RH_ScopedInstall(PositionAnyPedOutOfCollision, 0x5E13C0);
    RH_ScopedInstall(CanBeDeletedEvenInVehicle, 0x5DF150);
    RH_ScopedInstall(CanBeDeleted, 0x5DF100);
    RH_ScopedInstall(CanStrafeOrMouseControl, 0x5DF090);
    RH_ScopedInstall(CanBeArrested, 0x5DF060);
    RH_ScopedInstall(CanSetPedState, 0x5DF030);
    RH_ScopedInstall(CanPedReturnToState, 0x5DF000);
    RH_ScopedInstall(UseGroundColModel, 0x5DEFE0);
    RH_ScopedInstall(IsPedShootable, 0x5DEFD0);
    RH_ScopedInstall(GetLocalDirection, 0x5DEF60);
    RH_ScopedInstall(ClearAimFlag, 0x5DEF20);
    RH_ScopedOverloadedInstall(SetAimFlag, "Entity", 0x5DEED0, void(CPed::*)(CEntity *));
    RH_ScopedOverloadedInstall(SetAimFlag, "Heading", 0x5E8830, void(CPed::*)(float));
    RH_ScopedOverloadedInstall(SetLookFlag, "Entity", 0x5DEE40, void(CPed::*)(CEntity *, bool, bool));
    RH_ScopedOverloadedInstall(SetLookFlag, "Heading", 0x5DEDC0, void(CPed::*)(float, bool, bool));
    RH_ScopedInstall(CanUseTorsoWhenLooking, 0x5DED90);
    RH_ScopedInstall(PedIsReadyForConversation, 0x43ABA0);
    RH_ScopedInstall(CreateDeadPedMoney, 0x4590F0);
    RH_ScopedOverloadedInstall(CreateDeadPedPickupCoors, "", 0x459180, void(CPed::*)(float&, float&, float&));
    RH_ScopedInstall(CreateDeadPedWeaponPickups, 0x4591D0);
    RH_ScopedInstall(IsWearingGoggles, 0x479D10);
    RH_ScopedInstall(SetAmmo, 0x5DF290);
    RH_ScopedInstall(SetStayInSamePlace, 0x481090);
    RH_ScopedInstall(SetPedStats, 0x5DEBC0);
    RH_ScopedInstall(SetMoveState, 0x5DEC00);
    RH_ScopedInstall(SetMoveAnimSpeed, 0x5DEC10);
    RH_ScopedInstall(StopNonPartialAnims, 0x5DED10);
    RH_ScopedInstall(RestartNonPartialAnims, 0x5DED50);
    RH_ScopedInstall(DoWeHaveWeaponAvailable, 0x5DF300);
    RH_ScopedInstall(RemoveGogglesModel, 0x5DF170);
    RH_ScopedInstall(SetGunFlashAlpha, 0x5DF400);
    RH_ScopedInstall(CanSeeEntity, 0x5E0730);
    RH_ScopedInstall(SetPedDefaultDecisionMaker, 0x5E06E0);
    RH_ScopedInstall(GetWalkAnimSpeed, 0x5E04B0);
    RH_ScopedInstall(StopPlayingHandSignal, 0x5E0480);
    RH_ScopedInstall(IsPlayingHandSignal, 0x5E0460);
    RH_ScopedInstall(CanThrowEntityThatThisPedIsHolding, 0x5E0400);
    RH_ScopedInstall(DropEntityThatThisPedIsHolding, 0x5E0360);
    RH_ScopedInstall(GetEntityThatThisPedIsHolding, 0x5E02E0);
    RH_ScopedInstall(GetHoldingTask, 0x5E0290);
    RH_ScopedInstall(ReleaseCoverPoint, 0x5E0270);
    RH_ScopedInstall(DoGunFlash, 0x5DF340);
    RH_ScopedInstall(GetTransformedBonePosition, 0x5E01C0);
    RH_ScopedInstall(IsAlive, 0x5E0170);
    RH_ScopedInstall(DeadPedMakesTyresBloody, 0x6B4200);
    RH_ScopedInstall(Undress, 0x5E00F0);
    RH_ScopedInstall(SetLookTimer, 0x5DF8D0);
    RH_ScopedInstall(RestoreHeadingRate, 0x5DFD60);
    RH_ScopedInstall(Dress, 0x5E0130);
    RH_ScopedInstall(IsPlayer, 0x5DF8F0);
    RH_ScopedInstall(GetBikeRidingSkill, 0x5DF510);
    RH_ScopedInstall(SetPedPositionInCar, 0x5DF910, { .reversed = false });
    RH_ScopedInstall(SetRadioStation, 0x5DFD90);
    RH_ScopedInstall(PositionAttachedPed, 0x5DFDF0, { .reversed = false });
    RH_ScopedInstall(ResetGunFlashAlpha, 0x5DF4E0);

    RH_ScopedVMTInstall(SetModelIndex, 0x5E4880);
    RH_ScopedVMTInstall(DeleteRwObject, 0x5DEBF0);
    //RH_ScopedVirtualInstall(ProcessControl, 0x5E8CD0, { .reversed = false });
    RH_ScopedVMTInstall(Teleport, 0x5E4110);
    //RH_ScopedVirtualInstall(SpecialEntityPreCollisionStuff, 0x5E3C30, { .reversed = false });
    //RH_ScopedVirtualInstall(SpecialEntityCalcCollisionSteps, 0x5E3E90, { .reversed = false });
    RH_ScopedVMTInstall(PreRender, 0x5E8A20);
    RH_ScopedVMTInstall(Render, 0x5E7680);
    RH_ScopedVMTInstall(SetupLighting, 0x553F00);
    RH_ScopedVMTInstall(RemoveLighting, 0x5533B0);
    RH_ScopedVMTInstall(FlagToDestroyWhenNextProcessed, 0x5E7B70);
    //RH_ScopedVirtualInstall(ProcessEntityCollision, 0x5E2530, { .reversed = false });
    RH_ScopedVMTInstall(SetMoveAnim, 0x5E4A00);
    RH_ScopedVMTInstall(Save, 0x5D5730);
    RH_ScopedVMTInstall(Load, 0x5D4640);

    RH_ScopedGlobalInstall(SetPedAtomicVisibilityCB, 0x5F0060);
}

// 0x5E8030
CPed::CPed(ePedType pedType) : CPhysical(), m_pedIK{CPedIK(this)} {
    m_vecAnimMovingShiftLocal = CVector2D();

    m_fHealth = 100.0f;
    m_fMaxHealth = 100.0f;
    m_fArmour = 0.0f;

    m_nPedType = pedType;
    m_nType = ENTITY_TYPE_PED;

    // 0x5E8196
    physicalFlags.bCanBeCollidedWith = true;
    physicalFlags.bDisableTurnForce = true;

    m_nCreatedBy = PED_GAME;
    m_pVehicle = nullptr;
    field_52C = 0;
    field_744 = 0;
    field_74C = 0;
    m_nLookTime = 0;
    m_nDeathTimeMS = 0;

    m_vecAnimMovingShift = CVector2D();
    field_56C = CVector();
    field_578 = CVector(0.0f, 0.0f, 1.0f);

    m_nPedState = PEDSTATE_IDLE;
    m_nMoveState = PEDMOVE_STILL;
    m_fCurrentRotation = 0.0f;
    m_fHeadingChangeRate = 15.0f;
    m_fMoveAnim = 0.1f;
    m_fAimingRotation = 0.0f;
    m_standingOnEntity = nullptr;
    m_nWeaponShootingRate = 40;
    field_594 = 0;
    m_pEntityIgnoredCollision = nullptr;
    m_nSwimmingMoveState = 0;
    m_pFire = nullptr;
    m_fireDmgMult = 1.0f;
    m_pTargetedObject = nullptr;
    m_pLookTarget = nullptr;
    m_fLookDirection = 0.0f;
    m_pContactEntity = nullptr;
    field_588 = 99999.992f;
    m_fMass = 70.0f;
    m_fTurnMass = 100.0f;
    m_fAirResistance = 1.f / 175.f;
    m_fElasticity = 0.05f;
    m_nBodypartToRemove = -1;
    bHasACamera = CGeneral::GetRandomNumber() % 4 != 0;

    m_weaponAudio.Initialise(this);
    m_pedAudio.Initialise(this);

    m_acquaintance = CPedType::GetPedTypeAcquaintances(m_nPedType);
    m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    m_nDelayedWeapon = WEAPON_UNIDENTIFIED;
    m_nActiveWeaponSlot = 0;

    for (auto& weapon : m_aWeapons ) {
        weapon.m_Type = WEAPON_UNARMED;
        weapon.m_State = WEAPONSTATE_READY;
        weapon.m_AmmoInClip = 0;
        weapon.m_TotalAmmo = 0;
        weapon.m_TimeForNextShotMs = 0;
    }

    m_nWeaponSkill = eWeaponSkill::STD;
    m_nFightingStyle = STYLE_STANDARD;
    m_nAllowedAttackMoves = 0;

    GiveWeapon(WEAPON_UNARMED, 0, true);

    m_nWeaponAccuracy = 60;
    m_nLastWeaponDamage = -1;
    m_pLastEntityDamage = nullptr;
    field_768 = 0;
    m_pAttachedTo = nullptr;
    m_nTurretAmmo = 0;
    m_roadRageWith = nullptr;
    field_468 = 0;
    m_nWeaponModelId = -1;
    m_nMoneyCount = 0;
    field_72F = 0;
    m_nTimeTillWeNeedThisPed = 0;
    m_VehDeadInFrontOf = nullptr;

    m_pWeaponObject = nullptr;
    m_pGunflashObject = nullptr;
    m_pGogglesObject = nullptr;
    m_pGogglesState = nullptr;

    m_nWeaponGunflashAlphaMP1 = 0;
    m_nWeaponGunFlashAlphaProgMP1 = 0;
    m_nWeaponGunflashAlphaMP2 = 0;
    m_nWeaponGunFlashAlphaProgMP2 = 0;

    m_pCoverPoint = nullptr;
    m_pEnex = nullptr;
    field_798 = -1;

    m_pIntelligence = new CPedIntelligence(this);
    m_pPlayerData = nullptr;

    if (!IsPlayer()) {
        GetTaskManager().SetTaskSecondary(new CTaskComplexFacial{}, TASK_SECONDARY_FACIAL_COMPLEX);
    }
    GetTaskManager().SetTask(new CTaskSimpleStandStill{ 0, true, false, 8.0 }, TASK_PRIMARY_DEFAULT, false);

    m_Wobble = 0.0f;
    m_fRemovalDistMultiplier = 1.0f;
    m_StreamedScriptBrainToLoad = -1;

    CPopulation::UpdatePedCount(this, 0);

    if (CCheat::IsActive(CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD)) {
        if (!IsPlayer()) {
            GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(ePedType::PED_TYPE_PLAYER1));
            GetEventGroup().Add(CEventAcquaintancePedHate{FindPlayerPed()});
        }
    }
}

/*!
* @addr 0x5E8620
 */
CPed::~CPed() {
    CReplay::RecordPedDeleted(this);

    // Remove script brain
    if (bWaitingForScriptBrainToLoad) {
        CStreaming::SetMissionDoesntRequireModel(SCMToModelId(CTheScripts::ScriptsForBrains.m_aScriptForBrains[m_StreamedScriptBrainToLoad].m_StreamedScriptIndex));
        bWaitingForScriptBrainToLoad = false;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(this, m_StreamedScriptBrainToLoad);
        m_StreamedScriptBrainToLoad = -1;
    }

    CWorld::Remove(this);
    CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(this));
    CConversations::RemoveConversationForPed(this);

    ClearReference(m_pVehicle);

    if (m_pFire) {
        m_pFire->Extinguish();
    }

    ReleaseCoverPoint();
    ClearWeapons();

    if (bMiamiViceCop) {
        CPopulation::NumMiamiViceCops--;
    }

    CPopulation::UpdatePedCount(this, 1);

    m_pedSpeech.Terminate();
    m_weaponAudio.Terminate();
    m_pedAudio.Terminate();

    delete m_pIntelligence;

    ClearReference(m_pLookTarget);
}

/*!
* @addr 0x5E4720
*/
void* CPed::operator new(unsigned size) {
    return GetPedPool()->New();
}

/*!
* @addr 0x5E4730
*/
void* CPed::operator new(unsigned size, int32 poolRef) {
    return GetPedPool()->NewAt(poolRef);
}

/*!
* @addr 0x5E4760
*/
void CPed::operator delete(void* data) {
    GetPedPool()->Delete((CPed*)data);
}

// NOTSA
void CPed::operator delete(void* data, int poolRef) {
    GetPedPool()->Delete((CPed*)data);
}

/*!
* @addr 0x5E4A00
*/
void CPed::SetMoveAnim() {
    if (!IsAlive() || bIsDucking || m_pAttachedTo) {
        return;
    }

    const auto DoUpdateMoveAnim = [this](auto* assoc) {
        if (!bMoveAnimSpeedHasBeenSetByTask) {
            SetMoveAnimSpeed(assoc);
        }
    };

    if (m_nSwimmingMoveState == m_nMoveState) {
        switch (m_nMoveState) {
        case PEDMOVE_WALK:
        case PEDMOVE_JOG:
        case PEDMOVE_RUN:
        case PEDMOVE_SPRINT: {
            const auto GetAnimId = [this] {
                switch (m_nMoveState) {
                case PEDMOVE_RUN:
                    return ANIM_ID_RUN;
                case PEDMOVE_SPRINT:
                    return ANIM_ID_SPRINT;
                }
                return ANIM_ID_WALK;
            };

            if (const auto assoc = RpAnimBlendClumpGetAssociation(m_pRwClump, GetAnimId())) {
                DoUpdateMoveAnim(assoc);
            }

            break;
        }
        }
    } else if (m_nMoveState != PEDMOVE_NONE) {
        m_nSwimmingMoveState = m_nMoveState;

        switch (m_nMoveState) { // TODO: What's happening here?
        case PEDMOVE_WALK:
        case PEDMOVE_RUN:
        case PEDMOVE_SPRINT: {
            for (auto assoc = RpAnimBlendClumpGetFirstAssociation(m_pRwClump, ANIMATION_IS_PARTIAL); assoc; assoc = RpAnimBlendGetNextAssociation(assoc, ANIMATION_IS_PARTIAL)) {
                if ((assoc->m_Flags & ANIMATION_IS_FINISH_AUTO_REMOVE) == 0 && (assoc->m_Flags & ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) == 0) {
                    assoc->m_BlendDelta = -2.f;
                    assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
                }
            }

            ClearAimFlag();
            ClearLookFlag();

            break;
        }
        }

        // Do BlendAnimation and call `DoUpdateMoveAnim` afterwards
        const auto DoBlendAnim = [&, this](AssocGroupId grp, AnimationId animId, float blendDelta) {
            if (const auto assoc = CAnimManager::BlendAnimation(m_pRwClump, grp, animId, blendDelta)) {
                DoUpdateMoveAnim(assoc);
            }
        };

        switch (m_nMoveState) {
        case PEDMOVE_STILL:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_IDLE, 4.f);
            return;

        case PEDMOVE_TURN_L:
            DoBlendAnim(ANIM_GROUP_DEFAULT, ANIM_ID_TURN_L, 16.f);
            return;

        case PEDMOVE_TURN_R:
            DoBlendAnim(ANIM_GROUP_DEFAULT, ANIM_ID_TURN_R, 16.f);
            return;

        case PEDMOVE_WALK:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_WALK, 1.f);
            return;

        case PEDMOVE_RUN:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_RUN, m_nPedState == PEDSTATE_FLEE_ENTITY ? 3.f : 1.f);
            return;

        case PEDMOVE_SPRINT: {
            // If we're in a group, and our leader is sprinting as well sprinting should be played with a different anim group
            if (CPedGroups::IsInPlayersGroup(this)) {
                if (const auto leader = CPedGroups::GetPedsGroup(this)->GetMembership().GetLeader()) {
                    switch (leader->m_nMoveState) {
                    case PEDMOVE_RUN:
                    case PEDMOVE_SPRINT: {
                        DoBlendAnim(ANIM_GROUP_PLAYER, ANIM_ID_SPRINT, 1.f);
                        return;
                    }
                    }
                }
            }
            DoBlendAnim(m_nAnimGroup, ANIM_ID_SPRINT, 1.f);
            return;
        }
        }
    } else {
        m_nSwimmingMoveState = PEDMOVE_NONE;
    }
}

/*!
* @addr 0x5D4640
 */
bool CPed::Load() {
    auto size = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();

    // TODO: Can't do `auto save = CGenericGameStorage::LoadDataFromWorkBuffer<CPedSaveStructure>()` dure to deleted copy constructor in CWanted which is used somehow inside.
    // Would be nice if someone with more knowledge of templates and shit can fix that
    CPedSaveStructure save;
    CGenericGameStorage::LoadDataFromWorkBuffer(save);
    assert(size == sizeof(save));

    save.Extract(this);
    return true;
}

/*!
* @addr 0x5D5730
*/
bool CPed::Save() {
    CPedSaveStructure save;
    save.Construct(this);

    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(save));
    CGenericGameStorage::SaveDataToWorkBuffer(save);

    return true;
}

/*!
* @addr 0x43AB90
*/
bool CPed::PedIsInvolvedInConversation() {
    return this == CPedToPlayerConversations::m_pPed;
}

/*!
* @addr 0x43ABA0
*/
bool CPed::PedIsReadyForConversation(bool checkLocalPlayerWantedLevel) {
    // We don't talk when we're behind the wheel! (Nor when we're fighting...)
    if (bInVehicle || GetIntelligence()->GetTaskFighting()) {
        return false;
    }

    if (checkLocalPlayerWantedLevel && FindPlayerPed()->GetWanted()->m_nWantedLevel) {
        return false;
    }

    // If we're doing any of these we don't have the mental power to chat...
    switch (m_nMoveState) {
    case PEDMOVE_JOG:
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return false;
    }

    if (!IsCreatedByMission()) { // Don't check if we've a chatting task/event if we're a mission ped
        if (GetIntelligence()->FindTaskByType(TASK_COMPLEX_PARTNER_CHAT)) {
            return false;
        }

        if (GetEventGroup().GetEventOfType(eEventType::EVENT_CHAT_PARTNER)) {
            return false;
        }
    }

    return true;
}

/*!
* @addr 0x455560
*/
bool CPed::PedCanPickUpPickUp() {
    return FindPlayerPed(0)->GetTaskManager().FindActiveTaskFromList({ TASK_COMPLEX_ENTER_CAR_AS_DRIVER, TASK_COMPLEX_USE_MOBILE_PHONE });
}

/*!
* @addr 0x4590F0
*/
void CPed::CreateDeadPedMoney() {
    if (!CLocalisation::StealFromDeadPed()) {
        return;
    }

    switch (m_nPedType) {
    case PED_TYPE_COP:
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
        return;
    }

    if (IsCreatedByMission() && !bMoneyHasBeenGivenByScript) {
        return;
    }

    if (bInVehicle) {
        return;
    }

    if (m_nMoneyCount > 10) {
        CPickups::CreateSomeMoney(GetPosition(), m_nMoneyCount);
        m_nMoneyCount = 0;
    }
}

/*!
* @addr 0x459180
* @brief Created a pickup close to the ped's position (Using \r CPickups::CreatePickupCoorsCloseToCoors)
* @param [out] outPickupX, outPickupY, outPickupZ Position of the created pickup.
*/
void CPed::CreateDeadPedPickupCoors(float& outPickupX, float& outPickupY, float& outPickupZ) {
    CPickups::CreatePickupCoorsCloseToCoors(GetPosition(), outPickupX, outPickupY, outPickupZ);
}

/*!
* @address notsa
* @copybrief CPed::CreateDeadPedPickupCoors
* @param [out] pickupPos Position of the created pickup.
*/
void CPed::CreateDeadPedPickupCoors(CVector& pickupPos) {
    return CreateDeadPedPickupCoors(pickupPos.x, pickupPos.y, pickupPos.z);
}

/*!
* @notsa
*/
RpHAnimHierarchy& CPed::GetAnimHierarchy() const {
    return *GetAnimHierarchyFromSkinClump(m_pRwClump);
}

CAnimBlendClumpData& CPed::GetAnimBlendData() const {
    return *RpAnimBlendClumpGetData(m_pRwClump);
}

/*!
* @addr 0x4591D0
* @brief Create weapon/ammo pickups for dead ped
*/
void CPed::CreateDeadPedWeaponPickups() {
    if (bInVehicle || bDoesntDropWeaponsWhenDead) {
        return;
    }

    for (auto& wep : m_aWeapons) {
        switch (wep.m_Type) {
        case WEAPON_UNARMED:
        case WEAPON_DETONATOR:
            continue;
        }

        if (!wep.m_TotalAmmo && !wep.IsTypeMelee()) {
            continue; // Has no ammo, but isn't a melee weapon.. so it's a weapon with no ammo :D
        }

        // Now, create a pickup at close to our position
        CVector pickupPos{};
        CreateDeadPedPickupCoors(pickupPos);
        pickupPos.z += 0.3f;

        // No. of ammo the pickups will contain
        const auto pickupAmmo{ std::min(wep.m_TotalAmmo, (uint32)AmmoForWeapon_OnStreet[(size_t)wep.m_Type] * 2) };

        if (!CPickups::TryToMerge_WeaponType(
            pickupPos,
            wep.m_Type,
            ePickupType::PICKUP_ONCE_TIMEOUT,
            pickupAmmo,
            false
        )) {
            CPickups::GenerateNewOne_WeaponType(
                pickupPos,
                wep.m_Type,
                bDeathPickupsPersist ? ePickupType::PICKUP_ONCE_FOR_MISSION : ePickupType::PICKUP_ONCE_TIMEOUT,
                pickupAmmo,
                false,
                nullptr
            );
        }
    }
}

/*!
* @addr 0x5DEBB0
*/
void CPed::Initialise() {
    CPedType::Initialise();
    CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle();
}

/*!
* @addr 0x5DEBC0
* @unused
*/
void CPed::SetPedStats(ePedStats statsType) {
    m_pStats = &CPedStats::ms_apPedStats[(size_t)statsType];
}

/*!
* @addr 0x5DEBE0
*/
void CPed::Update()
{
    // NOP
}

/*!
* @addr 0x5DEC00
*/
void CPed::SetMoveState(eMoveState moveState) {
    m_nMoveState = moveState;
}

/*!
* @addr 0x5DEC10
*/
void CPed::SetMoveAnimSpeed(CAnimBlendAssociation* association) {
    const auto pitchFactor = std::clamp(m_pedIK.m_fSlopePitch, -0.3f, 0.3f);
    if (IsCreatedByMission()) {
        association->m_Speed = pitchFactor + 1.f;
    } else {
        association->m_Speed = pitchFactor + 1.2f - (float)m_nRandomSeed * RAND_MAX_FLOAT_RECIPROCAL * 0.4f; // todo: use GetRandom from CGeneral::
    }
}

/*!
* @addr 0x5DED10
*/
void CPed::StopNonPartialAnims() {
    RpAnimBlendClumpForEachAssociation(m_pRwClump, [](CAnimBlendAssociation* a) {
        if (!(a->m_Flags & ANIMATION_IS_PARTIAL)) {
            a->SetFlag(ANIMATION_IS_PLAYING, false);
        }
    });
}

/*!
* @addr 0x5DED50
*/
void CPed::RestartNonPartialAnims() {
    RpAnimBlendClumpForEachAssociation(m_pRwClump, [](CAnimBlendAssociation* a) {
        if (!(a->m_Flags & ANIMATION_IS_PARTIAL)) {
            a->SetFlag(ANIMATION_IS_PLAYING, true);
        }
    });
}

/*!
* @addr 0x5DED90
*/
bool CPed::CanUseTorsoWhenLooking() const {
    switch (m_nPedState) {
    case PEDSTATE_DRIVING:
    case PEDSTATE_DRAGGED_FROM_CAR:
        return false;
    }

    if (bIsDucking) {
        return false;
    }

    return true;
}

/*!
* @addr 0x5DEDC0
*/
void CPed::SetLookFlag(float lookHeading, bool unused, bool ignoreLookTime) {
    UNUSED(unused);

    if (m_nLookTime >= CTimer::GetTimeInMS() && !ignoreLookTime) {
        return;
    }

    bIsLooking = true;
    m_fLookDirection = lookHeading;
    m_nLookTime = 0;

    ClearReference(m_pLookTarget);

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5DEE40
* @brief Start looking at entity \a lookingTo
*/
void CPed::SetLookFlag(CEntity* lookingTo, bool unused, bool ignoreLookTime) {
    UNUSED(unused);

    if (m_nLookTime >= CTimer::GetTimeInMS() && !ignoreLookTime) {
        return;
    }

    bIsRestoringLook = false;
    bIsLooking = true;

    ChangeEntityReference(m_pLookTarget, lookingTo);

    m_fLookDirection = 999'999.f;
    m_nLookTime = 0;

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5DEED0
*/
void CPed::SetAimFlag(CEntity* aimingTo) {
    bIsAimingGun = true;
    bIsRestoringGun = false;
    ChangeEntityReference(m_pLookTarget, aimingTo);
    m_nLookTime = 0;
}

/*!
* @addr 0x5DEF20
* @brief Clear gun aiming flag
*/
void CPed::ClearAimFlag() {
    if (bIsAimingGun) {
        bIsAimingGun = false;
        bIsRestoringGun = true;
        m_pedIK.bUseArm = false;
        m_nLookTime = 0;
    }

    if (m_pPlayerData) {
        m_pPlayerData->m_fLookPitch = 0.f;
    }
}

/*!
* @addr 0x5DEF60
* @returns Which quadrant a given point is in relative to the ped's rotation. (Google: "Angle quadrants" - https://www.mathstips.com/wp-content/uploads/2014/03/unit-circle.png)
* @param point Point should be relative to the ped's position. Eg.: point = actualPoint - ped.GetPostion2D()
*/
uint8 CPed::GetLocalDirection(const CVector2D& point) const {
    float angle;
    for (angle = point.Heading() - m_fCurrentRotation + RadiansToDegrees(45.0f); angle < 0.0f; angle += TWO_PI); // TODO: This is quite stupid as well..
    return ((uint8)RadiansToDegrees(angle) / 90) % 4; // See original code below:

    // Original R* code - Kinda stupid, we just use modulo instead.
    // int32 dir;
    //for (dir = (int)RWRAD2DEG(angle) / 90; angle > 3; angle -= 4);
    // 0-forward, 1-left, 2-backward, 3-right.
    //return angle;
}

/*!
* @addr 0x5DEFD0
*/
bool CPed::IsPedShootable() const {
    // Not sure if they used a switch case or `<= PEDSTATE_STATES_CAN_SHOOT` originally, but I'll use a switch case.
    switch (m_nPedState) {
    case PEDSTATE_NONE:
    case PEDSTATE_IDLE:
    case PEDSTATE_LOOK_ENTITY:
    case PEDSTATE_LOOK_HEADING:
    case PEDSTATE_WANDER_RANGE:
    case PEDSTATE_WANDER_PATH:
    case PEDSTATE_SEEK_POSITION:
    case PEDSTATE_SEEK_ENTITY:
    case PEDSTATE_FLEE_POSITION:
    case PEDSTATE_FLEE_ENTITY:
    case PEDSTATE_PURSUE:
    case PEDSTATE_FOLLOW_PATH:
    case PEDSTATE_SNIPER_MODE:
    case PEDSTATE_ROCKETLAUNCHER_MODE:
    case PEDSTATE_DUMMY:
    case PEDSTATE_PAUSE:
    case PEDSTATE_ATTACK:
    case PEDSTATE_FIGHT:
    case PEDSTATE_FACE_PHONE:
    case PEDSTATE_MAKE_PHONECALL:
    case PEDSTATE_CHAT:
    case PEDSTATE_MUG:
    case PEDSTATE_AIMGUN:
    case PEDSTATE_AI_CONTROL:
    case PEDSTATE_SEEK_CAR:
    case PEDSTATE_SEEK_BOAT_POSITION:
    case PEDSTATE_FOLLOW_ROUTE:
    case PEDSTATE_CPR:
    case PEDSTATE_SOLICIT:
    case PEDSTATE_BUY_ICE_CREAM:
    case PEDSTATE_INVESTIGATE_EVENT:
    case PEDSTATE_EVADE_STEP:
    case PEDSTATE_ON_FIRE:
    case PEDSTATE_SUNBATHE:
    case PEDSTATE_FLASH:
    case PEDSTATE_JOG:
    case PEDSTATE_ANSWER_MOBILE:
    case PEDSTATE_HANG_OUT:
    case PEDSTATE_STATES_NO_AI:
    case PEDSTATE_ABSEIL_FROM_HELI:
    case PEDSTATE_SIT:
    case PEDSTATE_JUMP:
    case PEDSTATE_FALL:
    case PEDSTATE_GETUP:
    case PEDSTATE_STAGGER:
    case PEDSTATE_EVADE_DIVE:
    case PEDSTATE_STATES_CAN_SHOOT:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DEFE0
*/
bool CPed::UseGroundColModel() const {
    switch (m_nPedState) {
    case PEDSTATE_FALL:
    case PEDSTATE_EVADE_DIVE:
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF000
*/
bool CPed::CanPedReturnToState() const {
    switch (m_nPedState) {
    case PEDSTATE_NONE:
    case PEDSTATE_IDLE:
    case PEDSTATE_LOOK_HEADING:
    case PEDSTATE_WANDER_RANGE:
    case PEDSTATE_WANDER_PATH:
    case PEDSTATE_SEEK_POSITION:
    case PEDSTATE_SEEK_ENTITY:
    case PEDSTATE_FLEE_POSITION:
    case PEDSTATE_FLEE_ENTITY:
    case PEDSTATE_PURSUE:
    case PEDSTATE_FOLLOW_PATH:
    case PEDSTATE_ROCKETLAUNCHER_MODE:
    case PEDSTATE_DUMMY:
    case PEDSTATE_PAUSE:
    case PEDSTATE_FACE_PHONE:
    case PEDSTATE_MAKE_PHONECALL:
    case PEDSTATE_CHAT:
    case PEDSTATE_MUG:
    case PEDSTATE_AI_CONTROL:
    case PEDSTATE_SEEK_CAR:
    case PEDSTATE_SEEK_BOAT_POSITION:
    case PEDSTATE_FOLLOW_ROUTE:
    case PEDSTATE_CPR:
    case PEDSTATE_SOLICIT:
    case PEDSTATE_BUY_ICE_CREAM:
    case PEDSTATE_INVESTIGATE_EVENT:
    case PEDSTATE_ON_FIRE:
    case PEDSTATE_SUNBATHE:
    case PEDSTATE_FLASH:
    case PEDSTATE_JOG:
    case PEDSTATE_ANSWER_MOBILE:
    case PEDSTATE_HANG_OUT:
    case PEDSTATE_STATES_NO_AI:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF030
*/
bool CPed::CanSetPedState() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
    case PEDSTATE_ARRESTED:
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_STEAL_CAR:
        return false;
    }
    return true;
}

/*!
* @addr 0x5DF060
*/
bool CPed::CanBeArrested() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
    case PEDSTATE_ARRESTED:
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_EXIT_CAR:
        return false;
    }
    return true;
}

/*!
* @addr 5DF090
*/
bool CPed::CanStrafeOrMouseControl() const {
    switch (m_nPedState) {
    case PEDSTATE_IDLE:
    case PEDSTATE_FLEE_ENTITY:
    case PEDSTATE_FLEE_POSITION:
    case PEDSTATE_NONE:
    case PEDSTATE_AIMGUN:
    case PEDSTATE_ATTACK:
    case PEDSTATE_FIGHT:
    case PEDSTATE_JUMP:
    case PEDSTATE_ANSWER_MOBILE:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF100
* @brief Check if ped can be deleted
* @returns Always false if ped is in vehicle or is follower of player's group.
*/
bool CPed::CanBeDeleted() {
    return !bInVehicle && !IsFollowerOfGroup(FindPlayerGroup()) && CanBeDeletedEvenInVehicle();
}

/*!
* @addr 0x5DF100
* @brief Check if ped can be deleted even if it's in a vehicle.
* @returns False only if created by PED_UNKNOWN or PED_MISSION, true otherwise.
*/
bool CPed::CanBeDeletedEvenInVehicle() const {
    switch (m_nCreatedBy) {
    case ePedCreatedBy::PED_MISSION:
    case ePedCreatedBy::PED_UNKNOWN:
        return false;
    }
    return true;
}

/*!
* @addr 0x5DF170
* @brief Remove goggles model, also disabled related PostFX.
*/
void CPed::RemoveGogglesModel() {
    if (!m_pGogglesObject) {
        return;
    }

    // Release model info
    CVisibilityPlugins::GetClumpModelInfo(m_pGogglesObject)->RemoveRef();

#ifdef SA_SKINNED_PEDS
    // Remove skin anim
    if (IsClumpSkinned(m_pGogglesObject)) {
        RpClumpForAllAtomics(m_pGogglesObject, AtomicRemoveAnimFromSkinCB, nullptr);
    }
#endif

    // Destroy clump
    RpClumpDestroy(m_pGogglesObject);
    m_pGogglesObject = nullptr;

    // Disable FX's of the goggles. (See mem. var. `m_pGogglesState` in the header)
    if (m_pGogglesState) {
        *m_pGogglesState = false;
        m_pGogglesState = nullptr;
    }
}

/*!
* @addr   0x5DF200
* @return \a weaponType weapon's slot (\r CWeaponInfo::GetWeaponInfo()->slot)
*/
int32 CPed::GetWeaponSlot(eWeaponType weaponType)
{
    return CWeaponInfo::GetWeaponInfo(weaponType)->m_nSlot;
}

/*!
* @addr 0x5DF220
* @brief Set \a weaponType's slot totalAmmo to \a ammo. Also changes the gun's state to `READY`
*/
void CPed::GrantAmmo(eWeaponType weaponType, uint32 ammo) {
    const auto wepSlot = GetWeaponSlot(weaponType);
    if (wepSlot != -1) {
        auto& wepInSlot = GetWeaponInSlot(wepSlot);

        wepInSlot.m_TotalAmmo = std::min(wepInSlot.m_TotalAmmo + ammo, 99'999u); // Clamp upper

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (wepInSlot.m_TotalAmmo > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }
}

/*!
* @addr 0x5DF290
* @brief Im lazy to write it :D Similar to \r CPed::GrantAmmo
*/
void CPed::SetAmmo(eWeaponType weaponType, uint32 ammo) {
    const auto wepSlot = GetWeaponSlot(weaponType);
    if (wepSlot != -1) {
        auto& wepInSlot = GetWeaponInSlot(wepSlot);

        wepInSlot.m_TotalAmmo = std::min(ammo, 99'999u);
        wepInSlot.m_AmmoInClip = std::max(wepInSlot.m_TotalAmmo, wepInSlot.m_AmmoInClip);

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (wepInSlot.m_TotalAmmo > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }
}

/*!
* @addr 0x5DF300
* @brief Check if ped has a weapon of type \a weaponType
*/
bool CPed::DoWeHaveWeaponAvailable(eWeaponType weaponType) {
    const auto slot = GetWeaponSlot(weaponType);
    return slot != -1 && GetWeaponInSlot(slot).m_Type == weaponType;
}

/*!
* @addr 0x5DF340
* @brief Do gun flash by resetting it's alpha to max
*/
bool CPed::DoGunFlash(int32 duration, bool isLeftHand) {
    if (!m_pGunflashObject || !m_pWeaponObject) {
        return false;
    }

    // Really elegant.. ;D
    if (isLeftHand) {
        m_nWeaponGunflashAlphaMP2     = m_sGunFlashBlendStart;
        m_nWeaponGunFlashAlphaProgMP2 = (uint16)m_sGunFlashBlendStart / duration;
    } else {
        m_nWeaponGunflashAlphaMP1     = m_sGunFlashBlendStart;
        m_nWeaponGunFlashAlphaProgMP1 = (uint16)m_sGunFlashBlendStart / duration;
    }

    const auto angle = CGeneral::GetRandomNumberInRange(-360.0f, 360.0f);
    RwMatrixRotate(RwFrameGetMatrix(m_pGunflashObject), &CPedIK::XaxisIK, angle, rwCOMBINEPRECONCAT);

    return true;
}

/*!
* @addr 0x5DF400
* @brief Set alpha of gun flash object
*/
void CPed::SetGunFlashAlpha(bool rightHand) {
    if (!m_pGunflashObject) {
        return;
    }

    if (m_nWeaponGunflashAlphaMP1 < 0 && m_nWeaponGunflashAlphaMP2 < 0) { // Reordered a little.
        return;
    }

    auto& gunFlashAlphaInHand = rightHand ? m_nWeaponGunflashAlphaMP2 : m_nWeaponGunflashAlphaMP1;

    if (auto atomic = (RpAtomic*)GetFirstObject(m_pGunflashObject)) {
        // They used a clever trick to not have to convert to float..
        // Then they converted to a float to check if the number is higher than 255.. XDDD
        if (gunFlashAlphaInHand <= 0) {
            CVehicle::SetComponentAtomicAlpha(atomic, 0);
        } else {
            CVehicle::SetComponentAtomicAlpha(atomic, std::min(255, 350 * gunFlashAlphaInHand / m_sGunFlashBlendStart));
        }
        RpAtomicSetFlags(atomic, rpATOMICRENDER);
    }

    if (!gunFlashAlphaInHand) {
        gunFlashAlphaInHand = (uint16)-1;
    }
}

/*!
* @addr 0x5DF4E0
* @brief Reset alpha of gun flash object
*/
void CPed::ResetGunFlashAlpha() {
    if (m_pGunflashObject) {
        if (auto atomic = (RpAtomic*)GetFirstObject(m_pGunflashObject)) {
            RpAtomicSetFlags(atomic, 0);
            CVehicle::SetComponentAtomicAlpha(atomic, 0);
        }
    }
}

/*!
* @addr 0x5DF510
* @returns If ped is a player returns stat value BIKE_SKILL, otherwise 1 for mission peds and 0 for all others.
*/
float CPed::GetBikeRidingSkill() const {
    if (m_pPlayerData) {
        return std::min(1000.f, CStats::GetStatValue(eStats::STAT_BIKE_SKILL) / 1000.f);
    }
    return IsCreatedByMission() ? 1.f : 0.f;
}

/*!
* @addr 0x5DF560
* @brief Deal with shoulder bone (clavicle) rotation based on arm and breast rotation
*/
void CPed::ShoulderBoneRotation(RpClump* clump) {
    const auto GetMatrixOf = [hier = GetAnimHierarchyFromClump(clump)](eBoneTag bone) {
        return RpHAnimHierarchyGetNodeMatrix(hier, bone);
    };
    const auto DoUpdate = [&](eBoneTag breast, eBoneTag upperArm, eBoneTag clavicle) {
        auto* const breastRwMat = GetMatrixOf(breast);

        // Make the breast's matrix same as the upper arm's
        RwMatrixCopy(breastRwMat, GetMatrixOf(upperArm));

        CMatrix breastMat{ breastRwMat };
        CMatrix clavicleMat{ GetMatrixOf(clavicle) };

        // Calculate breast to clavicle transformation matrix (and store it in breastMat)
        breastMat = Invert(clavicleMat) * breastMat;

        // Half it's X rotation
        float rx, ry, rz;
        breastMat.ConvertToEulerAngles(&rx, &ry, &rz, ORDER_ZYX | SWAP_XZ);
        // Originally there is an `if` check of a static bool value, which is always true.
        rx /= 2.f;
        breastMat.ConvertFromEulerAngles(rx, ry, rz, ORDER_ZYX | SWAP_XZ);

        // Transform it back into it's own space
        breastMat = clavicleMat * breastMat;

        // Finally, update it's RW associated matrix
        breastMat.UpdateRW();
    };
    DoUpdate(eBoneTag::BONE_L_BREAST, eBoneTag::BONE_L_UPPER_ARM, eBoneTag::BONE_L_CLAVICLE);
    DoUpdate(eBoneTag::BONE_R_BREAST, eBoneTag::BONE_R_UPPER_ARM, eBoneTag::BONE_R_CLAVICLE);
}

/*!
* @addr 0x5DF8D0
* @brief Set look timer relative to now, but only if it has expired.
* @param time Time the timer ends relative to now
*/
void CPed::SetLookTimer(uint32 time) {
    if (CTimer::GetTimeInMS() > m_nLookTime) {
        m_nLookTime = CTimer::GetTimeInMS() + time;
    }
}

/*!
* @addr 0x5DF8F0
*/
bool CPed::IsPlayer() const
{
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1:
    case PED_TYPE_PLAYER2:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF910
*/
void CPed::SetPedPositionInCar() {
    ((void(__thiscall *)(CPed*))0x5DF910)(this);
}

/*!
* @addr 0x5DFD60
* @brief Set head changing rate to value stored in m_pStats
*/
void CPed::RestoreHeadingRate() {
    m_fHeadingChangeRate = m_pStats->m_fHeadingChangeRate;
}

/*!
* @addr 0x5DFD70
*/
void CPed::RestoreHeadingRateCB(CAnimBlendAssociation* assoc, void* data) {
    UNUSED(assoc);

    auto& ped = *((CPed*)data);
    ped.m_fHeadingChangeRate = ped.m_pStats->m_fHeadingChangeRate;
}

/*!
* @addr 0x5DFD90
* @brief Set random radio station if ped is in car. The station is chosen randomly, and is either `m_nRadio1` or `m_nRadio2` from the ped's \r CPedModelInfo
*/
void CPed::SetRadioStation()
{
    if (IsPlayer() || !m_pVehicle)
        return;

    if (m_pVehicle->m_pDriver == this) {
        const auto& mi = *(CPedModelInfo*)GetModelInfo();
        m_pVehicle->m_vehicleAudio.m_Settings.m_nRadioID = (CGeneral::GetRandomNumber() <= RAND_MAX / 2) ? mi.m_nRadio1 : mi.m_nRadio2;
    }
}

/*!
* @addr 0x5DFDF0
*/
void CPed::PositionAttachedPed()
{
    ((void(__thiscall *)(CPed*))0x5DFDF0)(this);
}

/*!
* @addr 0x5E00F0
* @brief Remove ped from the world, and request special model for it.
*/
void CPed::Undress(char* modelName) {
    DeleteRwObject();
    CStreaming::RequestSpecialModel(IsPlayer() ? 0 : m_nModelIndex, modelName, STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CWorld::Remove(this);
}

/*!
* @addr 0x5E0130
* @brief Re-add ped to the world
*/
void CPed::Dress() {
    SetModelIndex(m_nModelIndex);
    if (m_nPedState != PEDSTATE_DRIVING) {
        SetPedState(PEDSTATE_IDLE);
    }
    CWorld::Add(this);
    RestoreHeadingRate();
}

/*!
* @addr 0x5E0170
* @brief Checks if the Pedestrian is still alive.
*/
bool CPed::IsAlive() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
        return false;
    }
    return true;
}

/*!
* @addr 0x5E01A0
*/
void CPed::UpdateStatEnteringVehicle()
{
    // NOP
}

/*!
* @addr 0x5E01B0
*/
void CPed::UpdateStatLeavingVehicle()
{
    // NOP
}

/*!
* @addr 0x5E0270
* @brief Release current cover point
*/
void CPed::ReleaseCoverPoint() {
    if (m_pCoverPoint) {
        m_pCoverPoint->ReleaseCoverPointForPed(this);
        m_pCoverPoint = nullptr;
    }
}

/*!
* @addr 0x5E0290
* @returns Any active task of type HOLD_ENTITY, PICKUP_ENTITY, PUTDOWN_ENTITY.
*/
CTaskSimpleHoldEntity* CPed::GetHoldingTask() {
    // Man programming in C++03 must've been a pain.. if, if, if, if, if, if... IF.
    if (const auto task = GetTaskManager().FindActiveTaskFromList({ TASK_SIMPLE_HOLD_ENTITY, TASK_SIMPLE_PICKUP_ENTITY, TASK_SIMPLE_PUTDOWN_ENTITY })) {
        return static_cast<CTaskSimpleHoldEntity*>(task);
    }
    return nullptr;
}

/*!
* @addr 0x5E02E0
*/
CEntity* CPed::GetEntityThatThisPedIsHolding()
{
    if (const auto task = GetHoldingTask()) {
        return task->m_pEntityToHold;
    }

    if (const auto task = GetTaskManager().Find<CTaskComplexGoPickUpEntity>()) {
        return task->m_pEntity;
    }

    return nullptr;
}

/*!
* @addr  0x5E0360
* @brief Drop held entity, possibly deleting it.
*/
void CPed::DropEntityThatThisPedIsHolding(bool bDeleteHeldEntity) {
    if (const auto task = GetHoldingTask()) {
        // Drop the entity
        task->DropEntity(this, true);

        // Delete held entity (If any)
        if (bDeleteHeldEntity) {
            if (const auto heldEntity = task->m_pEntityToHold) {
                if (!heldEntity->IsObject() || !heldEntity->AsObject()->IsMissionObject()) {
                    heldEntity->DeleteRwObject(); // TODO; Are these 3 lines inlined?
                    CWorld::Remove(heldEntity);
                    delete heldEntity;
                }
            }
        }
    }
}

/*!
* @addr 0x5E0400
* @returns If there's a holding task and the held entity can be thrown.
*/
bool CPed::CanThrowEntityThatThisPedIsHolding() {
    if (const auto task = GetHoldingTask()) {
        return task->CanThrowEntity();
    }
    return false;
}

/*!
* @addr 0x5E0460
* @returns If there's a HANDSIGNAL task
*/
bool CPed::IsPlayingHandSignal() {
    return GetTaskManager().HasAnyOf<TASK_COMPLEX_HANDSIGNAL_ANIM>();
}

/*!
* @addr 0x5E0480
* @brief Stop the HANDSINGAL task
*/
void CPed::StopPlayingHandSignal() {
    if (const auto task = GetTaskManager().Find<TASK_COMPLEX_HANDSIGNAL_ANIM>()) {
        task->MakeAbortable(this);
    }
}

/*!
* @addr 0x5E04B0
* @returns Get walk speed in units/s based on the ped's anim group's WALK anim.
*/
float CPed::GetWalkAnimSpeed() {
    auto hier = CAnimManager::GetAnimAssociation(m_nAnimGroup, ANIM_ID_WALK)->m_BlendHier;

    CAnimManager::UncompressAnimation(hier);
    auto& firstSequence = hier->m_pSequences[ANIM_ID_WALK];

    if (!firstSequence.m_FramesNum) {
        return 0.f; // No frames
    }

    // NOTE: This is quite garbage, based on at least 5 assumptions, more of a hack than a solution from R*'s side.
    //       It won't work correctly if first frame is not a root frame, nor if the animation happens on any other axis than Y, etc..

    const auto lastFrame = firstSequence.GetUKeyFrame(firstSequence.m_FramesNum - 1);
    const auto lastFrameY = firstSequence.m_bHasTranslation
                                ? lastFrame->Trans.y
                                : ((KeyFrame*)lastFrame)->Rot.imag.y;

    return (lastFrameY - firstSequence.GetUKeyFrame(0)->Trans.y) / hier->m_fTotalTime;
}

/*!
* @addr 0x5E06E0
*/
void CPed::SetPedDefaultDecisionMaker() {
    if (!IsPlayer()) {
        GetIntelligence()->SetPedDecisionMakerType(IsCreatedByMission() ? -1 : m_pStats->m_nDefaultDecisionMaker);
    } else {
        GetIntelligence()->SetPedDecisionMakerType(-2);
    }
}

/*!
* @addr 0x5E0730
* @returns If entity is a given range angle relative to our current rotation given by \r limitAngle [-limitAngle, limitAngle]
*/
bool CPed::CanSeeEntity(CEntity* entity, float limitAngle) {

    // TODO: Inlined? 0x5E0780, 0x5E07BB
    const auto FixRadianAngle = [](float angle) {
        if (angle < TWO_PI) {
            if (angle < 0.f) {
                return angle + TWO_PI;
            }
        } else {
            return angle - TWO_PI;
        }
        return angle;
    };

    // R* used the degree returning function, and converted to radians, we just use the radian version directly
    const auto pointAngle = FixRadianAngle(CGeneral::GetRadianAngleBetweenPoints(entity->GetPosition2D(), GetPosition2D()));

    const auto delta = std::abs(m_fCurrentRotation - pointAngle);
    return delta < limitAngle || delta > TWO_PI - limitAngle;
}

/*!
* @addr 0x5E0820
*/
bool CPed::PositionPedOutOfCollision(int32 exitDoor, CVehicle* vehicle, bool findClosestNode)
{
    return ((bool(__thiscall *)(CPed*, int32, CVehicle*, bool))0x5E0820)(this, exitDoor, vehicle, findClosestNode);
}

/*!
* @addr     0x5E13C0
* @brief    Teleport ped to the furthest point from here that isn't colliding with a building or ped.
* @returns  If there was at lest one such point.
*/
bool CPed::PositionAnyPedOutOfCollision() {
    struct Point {
        CVector pos{};
        bool    found{};
        float   distSq{};
    };
    Point vehiclePoint{}, noCollPoint{};

    // Find 2 points furthest away from us:
    // - One that is colliding with a vehicle, (vehiclePoint)
    // - One that isn't colliding with one. (noCollPoint)
    // Neither point should be colliding with a building or ped.
    auto testPoint{ GetPosition() };
    for (auto y = 0; y < 15; y++) {
        testPoint.y -= 3.5f;
        for (auto x = 0; x < 15; x++) {
            testPoint.x -= 3.5f;

            // If we collide with a building or ped skip
            if (!CWorld::TestSphereAgainstWorld(testPoint, 0.6f, this, /*buildings: */true, false, false, true, false, false)) {
                continue;
            }

            const auto PossiblyUpdatePoint = [&, this](Point& p) {
                const auto distSq{ (testPoint - GetPosition()).SquaredMagnitude() };
                if (distSq < p.distSq) {
                    p.pos = testPoint;
                    p.distSq = distSq;
                    p.found = true;
                }
            };

            // Check for collision with vehicles
            if (CWorld::TestSphereAgainstWorld(testPoint, 0.6f, this, false, /*vehicles: */true, false, false, false, false)) {
                PossiblyUpdatePoint(vehiclePoint);
            } else { // Not collided with anything
                PossiblyUpdatePoint(noCollPoint);
            }
        }
    }

    // Set our position to one of the point's
    if (noCollPoint.found) { // If there was a point not colliding with a vehicle use it
        SetPosn(noCollPoint.pos);
    } else if (vehiclePoint.found) { // Otherwise use the other one collided with one (if any)
        SetPosn(vehiclePoint.pos + CVector{ 0.f, 0.f, GetBoundingBox().m_vecMax.z });
    }

    return noCollPoint.found || vehiclePoint.found;
}

/*!
* @addr 0x5E1660
*/
bool CPed::OurPedCanSeeThisEntity(CEntity* entity, bool isSpotted) {
    if (!isSpotted) {
        const auto dir2D{ entity->GetPosition2D() - GetPosition2D() };
        if (   DotProduct2D(dir2D, m_matrix->GetForward()) < 0.f // Is behind us
            || dir2D.SquaredMagnitude() >= 40.f * 40.f           // NOTSA: Using SqMag instead of Mag
        ) {
            return false;
        }
    }

    auto target{entity->GetPosition()};
    if (entity->IsPed()) {
        target.z += 1.f; // Adjust for head pos?
    }

    // Seems like they explicitly use this one instead of `IsLineOfSightClear` because of the `shootThru` check.
    CColPoint cp{};
    CEntity* hitEntity{};
    return !CWorld::ProcessLineOfSight(GetPosition(), target, cp, hitEntity, true, false, false, isSpotted, false, false, false, isSpotted);
}

/*!
* @addr 0x5E17E0
* @unused
*/
void CPed::SortPeds(CPed** pedList, int32 arg1, int32 arg2)
{
    ((void(__thiscall *)(CPed*, CPed**, int32, int32))0x5E17E0)(this, pedList, arg1, arg2);
}

/*!
* @addr 0x5E1A00
* @brief Update `m_fFPSMoveHeading` depending on the ped's Up/Down, Left/Right control states.
*/
float CPed::WorkOutHeadingForMovingFirstPerson(float heading) {
    if (!IsPlayer() || !m_pPlayerData) {
        return 0.f; // Probably shouldn't ever happen, but okay
    }

    const auto walkUpDown = (float)CPad::GetPad()->GetPedWalkUpDown();
    const auto walkLeftRight = (float)CPad::GetPad()->GetPedWalkLeftRight();
    if (walkUpDown == 0.f) {
        if (walkLeftRight != 0.f) {
            m_pPlayerData->m_fFPSMoveHeading = walkLeftRight < 0.f ? HALF_PI : -HALF_PI;
        }
    } else {
        m_pPlayerData->m_fFPSMoveHeading = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkLeftRight, walkUpDown);
    }

    return CGeneral::LimitRadianAngle(heading + m_pPlayerData->m_fFPSMoveHeading);
}

/*!
* @addr 0x5E1B10
*/
void CPed::UpdatePosition()
{
    ((void(__thiscall *)(CPed*))0x5E1B10)(this);
}

/*!
* @addr 0x5E1FA0
*/
void CPed::ProcessBuoyancy()
{
    if (bInVehicle)
        return;

    float fBuoyancyMult = 1.1F;
    if (!IsAlive())
        fBuoyancyMult = 1.8F;

    float fBuoyancy = fBuoyancyMult * m_fMass / 125.0F;
    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, fBuoyancy, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        physicalFlags.bTouchingWater = false;
        auto swimTask = GetIntelligence()->GetTaskSwim();
        if (swimTask)
            swimTask->m_fSwimStopTime = 1000.0F;

        return;
    }

    if (bIsStanding) {
        auto& standingOnEntity = m_pContactEntity;
        if (standingOnEntity && standingOnEntity->IsVehicle()) {
            auto pStandingOnVehicle = standingOnEntity->AsVehicle();
            if (pStandingOnVehicle->IsBoat() && !pStandingOnVehicle->physicalFlags.bRenderScorched) {
                physicalFlags.bSubmergedInWater = false;
                auto swimTask = GetIntelligence()->GetTaskSwim();
                if (!swimTask)
                    return;

                swimTask->m_fSwimStopTime += CTimer::GetTimeStep();
                return;
            }
        }
    }

    if (m_pPlayerData) {
        const auto& vecPedPos = GetPosition();
        float fCheckZ = vecPedPos.z - 3.0F;
        CColPoint lineColPoint;
        CEntity* colEntity;
        if (CWorld::ProcessVerticalLine(vecPedPos, fCheckZ, lineColPoint, colEntity, false, true, false, false, false, false, nullptr)) {
            if (colEntity->IsVehicle()) {
                auto colVehicle = colEntity->AsVehicle();
                if (colVehicle->IsBoat()
                    && !colVehicle->physicalFlags.bRenderScorched
                    && colVehicle->GetMatrix().GetUp().z > 0.0F) {

                    physicalFlags.bSubmergedInWater = false;
                    return;
                }
            }
        }
    }

    // Those 4 are called for some reason
    /*
    CTimeCycle::GetAmbientRed();
    CTimeCycle::GetAmbientGreen();
    CTimeCycle::GetAmbientBlue();
    CGeneral::GetRandomNumber();
    */

    // Add splash particle if it's the first frame we're touching water, and
    // the movement of ped is downward, preventing particles from being created
    // if ped is standing still and water wave touches him
    if (!physicalFlags.bTouchingWater && m_vecMoveSpeed.z < -0.01F) {
        auto vecMoveDir = m_vecMoveSpeed * CTimer::GetTimeStep() * 4.0F;
        auto vecSplashPos = GetPosition() + vecMoveDir;
        float fWaterZ;
        if (CWaterLevel::GetWaterLevel(vecSplashPos, fWaterZ, true, nullptr)) {
            vecSplashPos.z = fWaterZ;
            g_fx.TriggerWaterSplash(vecSplashPos);
            AudioEngine.ReportWaterSplash(this, -100.0F, true);
        }
    }

    physicalFlags.bTouchingWater = true;
    physicalFlags.bSubmergedInWater = true;
    ApplyMoveForce(vecBuoyancyForce);

    if (CTimer::GetTimeStep() / 125.0F < vecBuoyancyForce.z / m_fMass
        || GetPosition().z + 0.6F < mod_Buoyancy.m_fWaterLevel) {

        bIsStanding = false;
        bIsDrowning = true;

        bool bPlayerSwimmingOrClimbing = false;
        if (!IsPlayer()) {
            CEventInWater cEvent(0.75F);
            GetEventGroup().Add(&cEvent, false);
        }
        else {
            auto swimTask = GetIntelligence()->GetTaskSwim();
            if (swimTask) {
                swimTask->m_fSwimStopTime = 0.0F;
                bPlayerSwimmingOrClimbing = true;
            }
            else if (GetIntelligence()->GetTaskClimb()) {
                bPlayerSwimmingOrClimbing = true;
            }
            else {
                auto fAcceleration = vecBuoyancyForce.z / (CTimer::GetTimeStep() * m_fMass / 125.0F);
                CEventInWater cEvent(fAcceleration);
                GetEventGroup().Add(&cEvent, false);
            }

            if (bPlayerSwimmingOrClimbing)
                return;
        }

        float fTimeStep = pow(0.9F, CTimer::GetTimeStep());
        m_vecMoveSpeed.x *= fTimeStep;
        m_vecMoveSpeed.y *= fTimeStep;
        if (m_vecMoveSpeed.z < 0.0F)
            m_vecMoveSpeed.z *= fTimeStep;

        return;
    }

    auto swimTask = GetIntelligence()->GetTaskSwim();
    if (bIsStanding && swimTask)
    {
        swimTask->m_fSwimStopTime += CTimer::GetTimeStep();
        return;
    }

    if (m_pPlayerData) {
        CVector vecHeadPos(0.0F, 0.0F, 0.1F);
        GetTransformedBonePosition(vecHeadPos, eBoneTag::BONE_HEAD, false);
        if (vecHeadPos.z < mod_Buoyancy.m_fWaterLevel) {
            AsPlayer()->HandlePlayerBreath(true, 1.0F);
        }
    }
}

/*!
* @addr 0x5E3960
* @returns If ped can be moved, that is: not in the air or landing, is not arrested/dead/dying.
*/
bool CPed::IsPedInControl() const
{
    return !bIsLanding && !bIsInTheAir && IsAlive() && m_nPedState != PEDSTATE_ARRESTED;
}

/*!
* @addr 0x5E3990
* @brief Remove current weapon's object model. \r modelIndex should be the same as with which the object was created with.
*/
void CPed::RemoveWeaponModel(int32 modelIndex) {

    // For players remove any attached FX (Created in `AddWeaponModel` for molotov)
    if (IsPlayer()) {
        auto& activeWep = GetActiveWeapon();
        if (activeWep.m_FxSystem) {
            g_fxMan.DestroyFxSystem(activeWep.m_FxSystem);
            activeWep.m_FxSystem = nullptr;
        }
    }

    // Deal with weapon's loaded clump (if any)
    if (m_pWeaponObject) {
        if (   modelIndex == MODEL_INVALID
            || CModelInfo::GetModelInfo(modelIndex) == CVisibilityPlugins::GetClumpModelInfo(m_pWeaponObject)
        ) {
            // Release model info
            CVisibilityPlugins::GetClumpModelInfo(m_pWeaponObject)->RemoveRef();

#ifdef SA_SKINNED_PEDS
            // Remove skin anim
            if (IsClumpSkinned(m_pWeaponObject)) {
                RpClumpForAllAtomics(m_pWeaponObject, AtomicRemoveAnimFromSkinCB, nullptr);
            }
#endif

            // Destroy clump
            RpClumpDestroy(m_pWeaponObject);
            m_pWeaponObject = nullptr;
            m_pGunflashObject = nullptr;
        }
    }

    m_nWeaponGunflashAlphaMP1 = 0;
    m_nWeaponGunflashAlphaMP2 = 0;
    m_nWeaponModelId = -1;
}

/*!
* @addr 0x5E3A90
* @brief Creates goggles model for current infrared/night vision. See \r PutOnGoggles.
*/
void CPed::AddGogglesModel(int32 modelIndex, bool& inOutGogglesState) {
    assert(!m_pGogglesObject); // Make sure it's not created already

    if (modelIndex != MODEL_INVALID) {
        m_pGogglesObject = reinterpret_cast<RpClump*>(CModelInfo::GetModelInfo(modelIndex)->CreateInstanceAddRef());

        m_pGogglesState = &inOutGogglesState;
        inOutGogglesState = true;
    }
}

/*!
* @addr 0x5E3AE0
* @brief Puts on goggles if current weapon is infrared/night vision. (Also removes weapon model from hand and enabled corresponding PostFX)
*/
void CPed::PutOnGoggles() {
    auto& wepInSlot = GetWeaponInSlot(GetWeaponSlot(WEAPON_INFRARED));

    // Game checks if wepInSlot.m_nType != UNARMED here, not sure why? Probably compiler mistake on switch case codegen..

    switch (wepInSlot.m_Type) {
    case WEAPON_INFRARED:
    case WEAPON_NIGHTVISION: {

        // Add(load) googles model and enable PostFX
        const auto DoAddGogglesModel = [&, this](bool& state) {
            AddGogglesModel(wepInSlot.GetWeaponInfo().m_nModelId1, state);
        };

        switch (wepInSlot.m_Type) {
        case WEAPON_INFRARED:
            DoAddGogglesModel(CPostEffects::m_bInfraredVision);
            break;
        case WEAPON_NIGHTVISION:
            DoAddGogglesModel(CPostEffects::m_bNightVision);
            break;
        }

        // Make sure weapon model doesn't get loaded (Because we've put the them on)
        wepInSlot.m_DontPlaceInHand = true;

        // If it was the active weapon: unload it's weapon model
        if (&wepInSlot == &GetActiveWeapon()) {
            RemoveWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);
        }

        break;
    }
    }
}

/*!
* @addr    0x5E6580
* @returns Weapon skill with current weapon
*/
eWeaponSkill CPed::GetWeaponSkill() {
    return GetWeaponSkill(GetActiveWeapon().m_Type);
}

/*!
* @addr     0x5E3B60
* @returns Skill with \a weaponType. In case we're a player it's based on our current stat level with this weapon type, otherwise the mem. var. `m_nWeaponSkill`.
*/
eWeaponSkill CPed::GetWeaponSkill(eWeaponType weaponType)
{
    if (!CWeaponInfo::TypeHasSkillStats(weaponType)) {
        return eWeaponSkill::STD;
    }

    if (IsPlayer())
    {
        const auto GetReqStatLevelWith = [this, weaponType](eWeaponSkill skill) {
            return (float)CWeaponInfo::GetWeaponInfo(weaponType, skill)->m_nReqStatLevel;
        };

        const auto statValue = CStats::GetStatValue((eStats)CWeaponInfo::GetSkillStatIndex(weaponType));
        if (statValue >= GetReqStatLevelWith(eWeaponSkill::PRO)) {
            return eWeaponSkill::PRO;
        } else if (statValue <= GetReqStatLevelWith(eWeaponSkill::POOR)) {
            return eWeaponSkill::POOR;
        } else {
            return eWeaponSkill::STD; // Somewhere in-between poor and pro stat levels
        }
    } else {
        if (weaponType == WEAPON_PISTOL && m_nPedType == PED_TYPE_COP)
            return eWeaponSkill::COP;
        return m_nWeaponSkill;
    }
}

/*!
* @addr 0x5E3C10
* @brief Set weapon skill, unless we're a player ped (\r IsPlayer)
*/
void CPed::SetWeaponSkill(eWeaponType weaponType, eWeaponSkill skill)
{
    if (!IsPlayer()) {
        m_nWeaponSkill = skill;
    }
}

/*!
* @addr 0x5E1950
* @brief Clear ped look, and start restoring it
*/
void CPed::ClearLookFlag() {
    if (!bIsLooking) {
        return;
    }

    // Originally there's a do-while loop, but it will never iterate more than once, so I won't add it.
    // do { ...

    bIsLooking = false;
    bIsDrowning = false;
    bIsRestoringLook = true;

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }

    m_nLookTime = CTimer::GetTimeInMS() + (IsPlayer() ? 4000 : 2000);

    // .. } while ((PEDSTATE_LOOK_HEADING || PEDSTATE_LOOK_ENTITY) && bIsLooking), but `bIsLooking` will never be true at this point.
}

/*!
* @addr 0x5E3FF0
* @brief Just calls \r ClearLookFlag
*/
void CPed::ClearLook() {
    ClearLookFlag();
}

/*!
* @addr 0x5E4000
*/
/*!
* @addr    0x5E4000
* @brief   Turns ped to look at `m_pLookTarget`.
* @returns If `m_fCurrentRotation` changed.
*/
bool CPed::TurnBody() {
    if (m_pLookTarget) {
        m_fLookDirection = CGeneral::GetRadianAngleBetweenPoints(m_pLookTarget->GetPosition2D(), GetPosition2D());
    }

    m_fLookDirection = CGeneral::LimitRadianAngle(m_fLookDirection);

    // Some logic to make sure `m_fCurrentRotation` is always in the range [-PI, PI] or [0, 2PI] ? Not sure.. TODO.
    if (m_fCurrentRotation + PI >= m_fLookDirection) {
        if (m_fCurrentRotation - PI > m_fLookDirection) {
            m_fCurrentRotation += PI;
        }
    } else {
        m_fCurrentRotation -= PI;
    }

    m_fAimingRotation = m_fLookDirection;

    if (std::abs(m_fCurrentRotation - m_fLookDirection) <= 0.05f) {
        return true;
    } else {
        m_fCurrentRotation -= (m_fCurrentRotation - m_fLookDirection) / 5.f;
        return false;
    }
}

/*!
* @addr 0x5E4220
* @brief Check if `this` is valid. Probably used by scripts?
*/
bool CPed::IsPointerValid() {
    return GetPedPool()->IsObjectValid(this) && (!m_pCollisionList.IsEmpty() || this == FindPlayerPed());
}

/*!
* @addr 0x5E4280
* @brief Retrieve object-space position of the given \a bone.
* @param updateSkinBones if not already called `UpdateRpHAnim` will be called. If this param is not set, and the latter function wasn't yet called a default position will be returned.
*/
CVector CPed::GetBonePosition(eBoneTag bone, bool updateSkinBones) {
    if (!bCalledPreRender) {
        if (!updateSkinBones) {
            return m_matrix->TransformPoint(GetPedBoneStdPosition(bone));
        }
        UpdateRpHAnim();
        bCalledPreRender = true;
    }
    if (const auto* const m = GetBoneMatrix(bone)) {
        return *RwMatrixGetPos(m);
    }
    return GetPosition();
}

/*
* @addr 0x5E4280
* @brief Added for compatiblity reasons for hooking - use the version returning CVector where possible in code.
*/
void CPed::GetBonePosition(CVector* outVec, eBoneTag bone, bool updateSkinBones) {
    *outVec = CPed::GetBonePosition(bone, updateSkinBones);
}

/*!
* @addr 0x5E01C0
* @brief Transform \r inOutPos into the given \a bone's space
*
* @param [in,out] inOutPos The position to be transformed in-place.
* @param          updateSkinBones If `UpdateRpHAnim` should be called
*/
void CPed::GetTransformedBonePosition(RwV3d& inOutPos, eBoneTag bone, bool updateSkinBones) { // todo: fix this too!!!
    // Pretty much the same as GetBonePosition..
    if (updateSkinBones) {
        if (!bCalledPreRender) {
            UpdateRpHAnim();
            bCalledPreRender = true;
        }
    } else if (!bCalledPreRender) { // Return static local bone position instead
        inOutPos = m_matrix->TransformPoint(GetPedBoneStdPosition(bone));
        return;
    }

    // Return actual position
    RwV3dTransformPoints(&inOutPos, &inOutPos, 1, GetBoneMatrix(bone));
}

/*!
* @addr 0x5E4390
* @brief Give ped an object to hold. If he already has one and \a replace is `true` they will drop it, otherwise do nothing.
*/
void CPed::GiveObjectToPedToHold(int32 modelIndex, uint8 replace) {

    // Deal with ped already holding an entity.
    // If `replace` is `true`, just drop the entity, otherwise do nothing.
    if (GetTaskManager().HasAnyOf<TASK_SIMPLE_HOLD_ENTITY>()) {
        if (!GetEntityThatThisPedIsHolding() || !replace) {
            return;
        }
        DropEntityThatThisPedIsHolding(true);
    }

    // Create object
    auto object = new CObject(modelIndex, false);
    object->SetPosn(GetPosition());
    CWorld::Add(object);

    // Create task
    CVector pos{ 0.f, 0.f, 0.f };
    GetTaskManager().SetTaskSecondary(new CTaskSimpleHoldEntity(object, &pos, PED_NODE_RIGHT_HAND), TASK_SECONDARY_PARTIAL_ANIM);
}

/*!
* @addr  0x5E4500
* @brief Set ped's state. If he's now !IsAlive() blip is deleted (if `bClearRadarBlipOnDeath` is set) and `ReleaseCoverPoint()` is called.
*/
void CPed::SetPedState(ePedState pedState) {
    m_nPedState = pedState;
    if (!IsAlive()) {
        ReleaseCoverPoint();
        if (bClearRadarBlipOnDeath) {
            CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(this));
            // TODO: Shouldn't we `bClearRadarBlipOnDeath = false` here?
        }
    }
}

/*!
* @addr 0x5E47E0
* @brief Set ped's created by. If created by mission, set it's hearing and seeing range to 30.
*/
void CPed::SetCharCreatedBy(ePedCreatedBy createdBy) {
    m_nCreatedBy = createdBy;

    SetPedDefaultDecisionMaker();

    if (IsCreatedByMission()) {
        auto intel = GetIntelligence();

        intel->SetSeeingRange(30.f);
        intel->SetHearingRange(30.f);
        if (!IsPlayer()) {
            intel->m_fDmRadius = 0.f;
            intel->m_nDmNumPedsToScan = 0;
        }
    }
}

/*!
 * @addr 0x5E4C50
 */
void CPed::CalculateNewVelocity() {
    ((void(__thiscall*)(CPed*))0x5E4C50)(this);
}

/*!
* @addr 0x5E52E0
*/
void CPed::CalculateNewOrientation() {
    if (CReplay::Mode != MODE_PLAYBACK && IsPedInControl()) {
        SetOrientation(0.f, 0.f, m_fCurrentRotation);
    }
}

/*!
* @addr 0x5E5320
*/
void CPed::ClearAll() {
    if (IsPedInControl() || IsStateDead()) {
        bRenderPedInCar = true;
        bHitSteepSlope = false;
        bCrouchWhenScared = false;
        m_nPedState = PEDSTATE_NONE;
        m_nMoveState = PEDMOVE_NONE;
        m_pEntityIgnoredCollision = nullptr;
    }
}

/*!
 * @addr 0x5E5380
 */
void CPed::DoFootLanded(bool leftFoot, uint8 arg1) {
    ((void(__thiscall*)(CPed*, bool, uint8))0x5E5380)(this, leftFoot, arg1);
}

/*!
* @addr 0x5E57F0
*/
void CPed::PlayFootSteps() {
    return plugin::CallMethod<0x5E57F0>(this);
    // Below code is kinda working.. kinda. I'm too lazy to fix it, good luck future me!
    /* auto& anim = *RpAnimBlendClumpGetFirstAssociation(m_pRwClump);

    const auto IsWalkRunSprintAnim = [&] {
        switch (anim.m_nAnimId) {
        case ANIM_ID_WALK:
        case ANIM_ID_RUN:
        case ANIM_ID_SPRINT: {
            return true;
        }
        }
        return false;
    };

    if (bDoBloodyFootprints) {
        if (m_nDeathTimeMS && m_nDeathTimeMS < 300) {
            m_nDeathTimeMS -= 1;
            if (!m_nDeathTimeMS) {
                bDoBloodyFootprints = false;
            }
        }
    }

    if (!bIsStanding) {
        return;
    }

    // 0x5E58FB, 0x5E5A0B, 0x5E5AB9, 0x5E5E64, 0x5E5D87
    const auto DoProcessLanding = [this] {
        if (bIsLanding) { // Redundant check.. probably inlined function?
            if (const auto task = GetTaskManager().GetSimplestActiveTask(); task->GetTaskType() == TASK_SIMPLE_LAND) {
                auto landedTask = task->As<CTaskSimpleLand>();
                if (landedTask->RightFootLanded()) {
                    DoFootLanded(false, true);
                } else if (landedTask->RightFootLanded()) {
                    DoFootLanded(true, true);
                }
            }
        }
    };

    // TODO: Is this inlined? What is this doing exactly? (Anim stuff)
    float walkBlendTotal{}, idleBlendTotal{};
    CAnimBlendAssociation* walkAssoc{};
    auto* lastAssoc = &anim;
    do { // 0x5E58A1
        if (lastAssoc->m_nFlags & ANIMATION_WALK) {
            walkBlendTotal += lastAssoc->m_fBlendAmount;
            walkAssoc = lastAssoc;
        } else {
            if ((lastAssoc->m_nFlags & ANIMATION_ADD_TO_BLEND) == 0) {
                if (lastAssoc->m_nAnimId != ANIM_ID_FIGHT_IDLE) {
                    if (lastAssoc->m_nFlags & ANIMATION_IS_PARTIAL || bIsDucking) {
                        idleBlendTotal += lastAssoc->m_fBlendAmount;
                    }
                }
            }
        }

        lastAssoc = RpAnimBlendGetNextAssociation(lastAssoc);
    } while (lastAssoc);


    if (!walkAssoc || walkBlendTotal <= 0.5f || idleBlendTotal >= 1.f) { // 0x5E58FB
        DoProcessLanding();
        return;
    }

    auto* walkAssocHier = walkAssoc->m_pHierarchy;

    float minAnimTime = walkAssocHier->m_fTotalTime / 15.f;
    float maxAnimTime = walkAssocHier->m_fTotalTime / 2.f + minAnimTime; // Weird.. Why adding `minAnimTime` to it?

    if (bIsDucking) {
        minAnimTime += 0.2f;
        maxAnimTime += 0.2f;
    }

    if (m_pStats == &CPedStats::ms_apPedStats[STAT_BURGULAR_STATUS]) { // 0X5E5968

        // NOTE: The number `15` seems to be reoccurring, it's used above as well.
        const float animTimeMult = walkAssoc->m_nAnimId != AnimationId::ANIM_ID_WALK ? 8.f / 15.f : 5.f / 15.f;

        float adhesionMult{ 1.f };
        switch (g_surfaceInfos.GetAdhesionGroup(m_nContactSurface)) {
        case eAdhesionGroup::ADHESION_GROUP_SAND: { // 0X5E599F
            if (CGeneral::GetRandomNumber() % 64) {
                m_vecAnimMovingShiftLocal *= 0.2f;
            }

            DoProcessLanding();
            return;
        }
        case eAdhesionGroup::ADHESION_GROUP_WET: { // 0X5E59B1
            m_vecAnimMovingShiftLocal *= 0.3f;
            DoProcessLanding();
            return;
        }
        case eAdhesionGroup::ADHESION_GROUP_LOOSE: { // 0x5E5A25
            if (CGeneral::GetRandomNumber() % 128) {
                m_vecAnimMovingShiftLocal *= 0.5f;
            }
            adhesionMult = 0.5f;
            break;
        }
        }

        if (m_pedAudio.m_iRadioStationScriptRequest) { // Move condition out here, but originally it was at 0x5E5AFA and 0x5E5A68
            const auto DoAddSkateAE = [&, this](eAudioEvents audio) {
                // 0x5E5AB4
                m_pedAudio.AddAudioEvent(audio,
                    CAEAudioUtility::AudioLog10(adhesionMult) * 20.f,
                    walkAssoc->m_nAnimId == AnimationId::ANIM_ID_WALK ? 1.f : 0.75f
                );
            };

            if (   walkAssoc->m_fCurrentTime <= 0.f
                || walkAssoc->m_fCurrentTime - walkAssoc->m_fTimeStep > 0.f
            ) {
                if (adhesionMult > 0.2f
                    && walkAssoc->m_fCurrentTime > animTimeMult
                    && walkAssoc->m_fCurrentTime - walkAssoc->m_fTimeStep <= animTimeMult
                ) {
                    // 0x5E5B46
                    DoAddSkateAE(eAudioEvents::AE_PED_SKATE_RIGHT);
                }
            } else {
                // 0x5E5AB4
                DoAddSkateAE(eAudioEvents::AE_PED_SKATE_LEFT);
            }
        }

        DoProcessLanding();
        return;
    }

    // 0x5E5E56 and 0x5E5D57.. Seems like inlined?
    const auto DoFootStepAE = [&, this](bool isLeftFoot) {
        if (m_pedAudio.m_iRadioStationScriptRequest) {
            const auto DoAddFootStepAE = [&, this](float volume, float speed) {
                m_pedAudio.AddAudioEvent(isLeftFoot ? eAudioEvents::AE_PED_FOOTSTEP_RIGHT : eAudioEvents::AE_PED_FOOTSTEP_LEFT, volume, speed);
            };

            if (bIsDucking) {
                DoAddFootStepAE(-18.f, 0.8f);
            } else {
                const auto DoAddMovingFootStepAE = [&, this](float volume, float speed) {
                    if (m_nAnimGroup == ANIM_GROUP_PLAYERSNEAK) {
                        DoAddFootStepAE(volume - 6.f, speed - 0.1f);
                    } else {
                        DoAddFootStepAE(volume, speed);
                    }
                };

                switch (m_nMoveState) {
                case PEDMOVE_RUN:
                    DoAddMovingFootStepAE(-6.f, 1.1f);
                    break;
                case PEDMOVE_SPRINT:
                    DoAddMovingFootStepAE(0.f, 1.2f);
                    break;
                default:
                    DoAddMovingFootStepAE(-12.f, 0.9f);
                    break;
                }
            }
        }
        DoFootLanded(isLeftFoot, IsWalkRunSprintAnim());
    };

    // 0x5E5B50
    if (   minAnimTime > walkAssoc->m_fCurrentTime
        || walkAssoc->m_fCurrentTime - walkAssoc->m_fTimeStep >= minAnimTime
    ) {
        // 0x5E5D8E
        if (walkAssoc->m_fCurrentTime >= (double)maxAnimTime
            && walkAssoc->m_fCurrentTime - walkAssoc->m_fTimeStep < maxAnimTime)
        {
            // 0x5E592B - 0x5E5E56
            DoFootStepAE(false); // Do right footstep AE
            DoProcessLanding();
            return;
        }
    }

    if (IsPlayer()) { // 0x5E5B79
        if (const auto pd = m_pPlayerData) {
            const auto DoEventSoundQuiet = [this](float volume) {
                // 0x5E5BCB
                CEventSoundQuiet event{ this, volume, (uint32)-1, {0.f, 0.f, 0.f}};
                GetEventGlobalGroup()->Add(&event);
            };

            const auto isWearingBalaclava = pd->m_pPedClothesDesc->GetIsWearingBalaclava();
            switch (m_nMoveState) {
            case PEDMOVE_JOG:   // 0x5E5BAD -- hehehe, 0x 5E5 - BAD
            case PEDMOVE_RUN: { // 0x5E5BB6
                // 0x5E5BDD
                if (pd->m_fMoveBlendRatio >= 2.f) {
                    DoEventSoundQuiet(isWearingBalaclava ? 55.f : 45.f);
                } else {
                    const auto DoEventSoundQuiet_MoveBlendFactor = [&](float moveBlendFactor) {
                        // 0x5E5C31
                        if (const auto volume = (pd->m_fMoveBlendRatio - 1.f) * moveBlendFactor + 30.f; volume > 0.f) {
                            DoEventSoundQuiet(volume);
                        }
                    };

                    // 0x05E5BF3
                    if (isWearingBalaclava && pd->m_fMoveBlendRatio > 1.1f) {
                        DoEventSoundQuiet_MoveBlendFactor(20.f);
                    } else if (pd->m_fMoveBlendRatio > 1.5f) {
                        DoEventSoundQuiet_MoveBlendFactor(15.f);
                    }
                }
                break;
            }
            case PEDMOVE_SPRINT: { // 0x5E5BBB
                DoEventSoundQuiet(isWearingBalaclava ? 65.f : 55.f);
                break;
            }
            }
            if (pd->m_pPedClothesDesc->GetIsWearingBalaclava()) {
            }
        }
    }

    // 0x5E5C8D
    DoFootStepAE(true); // Do left foot step AE
    DoFootLanded(true, IsWalkRunSprintAnim());
    DoProcessLanding(); */
}

/*!
* @addr 0x5E5ED0
* @brief Create model for current active weapon. Also creates FX for molotov if `this->IsPlayer()`.
* @param modelIndex Model that should be created for the current weapon.
*/
void CPed::AddWeaponModel(int32 modelIndex) {
   if (modelIndex == MODEL_INVALID) {
       return;
   }

   // Make sure this weapon is supposed to have a model
   // (May be set to false even if it does, eg.: in case of infrared or night googles, see `TakeOffGoggles`)
   auto& activeWep = GetActiveWeapon();
   if (activeWep.m_DontPlaceInHand) {
       return;
   }

   // Remove old model (if any)
   if (m_pWeaponObject) {
       RemoveWeaponModel(MODEL_INVALID);
   }

   // Create clump for model
   auto& wepMI = *CModelInfo::GetModelInfo(modelIndex);
   m_pWeaponObject = (RpClump*)wepMI.CreateInstance();
   m_pGunflashObject = m_pWeaponObject ? CClumpModelInfo::GetFrameFromName(m_pWeaponObject, "gunflash") : nullptr;
   wepMI.AddRef();

   m_nWeaponModelId = modelIndex;

   // Create FX for molotovs
   if (IsPlayer()) {
       if (   activeWep.m_Type == WEAPON_MOLOTOV
           && modelIndex == eModelID::MODEL_MOLOTOV
           && !activeWep.m_FxSystem
        ) {
           activeWep.m_FxSystem = g_fxMan.CreateFxSystem("molotov_flame", CVector{ 0.f, 0.f, 0.f }, GetBoneMatrix(eBoneTag::BONE_R_HAND), false);
           if (const auto fx = activeWep.m_FxSystem) {
               fx->SetLocalParticles(true);
               fx->CopyParentMatrix();
               fx->Play();
           }
       }
   }
}

/*!
* @addr 0x5E6010
* @brief Take off goggles (Infrared/Night Vision weapon)
*/
void CPed::TakeOffGoggles()
{
    auto& wepInSlot = GetWeaponInSlot(GetWeaponSlot(WEAPON_INFRARED));

    // Game checks if wepInSlot.m_nType != UNARMED here, not sure why? Probably compiler mistake on switch case codegen..

    switch (wepInSlot.m_Type) {
    case WEAPON_INFRARED:
    case WEAPON_NIGHTVISION: {
        // Remove googles model
        RemoveGogglesModel();

        wepInSlot.m_DontPlaceInHand = false;

        // Since we've took off the goggles we might have to load it's weapon model
        if (&wepInSlot == &GetActiveWeapon()) {
            AddWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);
        }

        break;
    }
    }
}

/*!
* @addr 0x5E6080
* @brief Give ped weapon \a weaponType with ammo \a ammo. If ped has already the same weapon, just add the ammo to the weapon's current total ammo, and reload it.
* @returns Slot of \a weaponType
*/
eWeaponSlot CPed::GiveWeapon(eWeaponType weaponType, uint32 ammo, bool likeUnused) {
    const auto givenWepInfo = CWeaponInfo::GetWeaponInfo(weaponType);
    auto& wepInSlot = GetWeaponInSlot(givenWepInfo->m_nSlot);
    const auto wepSlot = (eWeaponSlot)givenWepInfo->m_nSlot;

    if (wepInSlot.m_Type != weaponType) { // Another weapon in the slot, remove it, and set this weapon

        // Remove previous weapon (and possibly add any ammo it had to `ammo`)
        if (wepInSlot.m_Type != WEAPON_UNARMED) {
            switch (wepSlot) {
            case eWeaponSlot::SHOTGUN:
            case eWeaponSlot::SMG:
            case eWeaponSlot::RIFLE: {
                ammo += wepInSlot.m_TotalAmmo;
                break;
            }
            }

            RemoveWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);

            if (givenWepInfo->m_nSlot == CWeaponInfo::GetWeaponInfo(WEAPON_INFRARED)->m_nSlot) {
                RemoveGogglesModel();
            }

            wepInSlot.Shutdown();
        }

        // Give new
        wepInSlot.Initialise(weaponType, ammo, this);

        // Now `wepInSlot` is the weapon we've given to the player

        if (givenWepInfo->m_nSlot == m_nActiveWeaponSlot && !bInVehicle) {
            AddWeaponModel(givenWepInfo->m_nModelId1);
        }
    } else { // Same weapon already in the slot, update its ammo count and `Reload()` it
        if (wepSlot == eWeaponSlot::GIFT) { // Gifts have no ammo :D
            return eWeaponSlot::GIFT;
        }

        wepInSlot.m_TotalAmmo = std::min(99'999u, wepInSlot.m_TotalAmmo + ammo);
        wepInSlot.Reload(this);

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (wepInSlot.m_TotalAmmo > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }

    if (wepInSlot.m_State != WEAPONSTATE_OUT_OF_AMMO) {
        wepInSlot.m_State = WEAPONSTATE_READY;
    }

    return wepSlot;
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet1() {
    GiveWeapon(WEAPON_BRASSKNUCKLE, 1, true);
    GiveWeapon(WEAPON_BASEBALLBAT, 1, true);
    GiveWeapon(WEAPON_MOLOTOV, 10, true);
    GiveWeapon(WEAPON_PISTOL, 100, true);
    GiveWeapon(WEAPON_SHOTGUN, 50, true);
    GiveWeapon(WEAPON_MICRO_UZI, 150, true);
    GiveWeapon(WEAPON_AK47, 120, true);
    GiveWeapon(WEAPON_COUNTRYRIFLE, 25, true);
    GiveWeapon(WEAPON_RLAUNCHER, 200, true);
    GiveWeapon(WEAPON_SPRAYCAN, 200, true);
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet2() {
    GiveWeapon(WEAPON_KNIFE, 0, true);
    GiveWeapon(WEAPON_GRENADE, 10, true);
    GiveWeapon(WEAPON_DESERT_EAGLE, 40, true);
    GiveWeapon(WEAPON_SAWNOFF_SHOTGUN, 40, true);
    GiveWeapon(WEAPON_TEC9, 150, true);
    GiveWeapon(WEAPON_M4, 150, true);
    GiveWeapon(WEAPON_SNIPERRIFLE, 21, true);
    GiveWeapon(WEAPON_FLAMETHROWER, 500, true);
    GiveWeapon(WEAPON_EXTINGUISHER, 200, true);
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet3() {
    GiveWeapon(WEAPON_CHAINSAW, 0, true);
    GiveWeapon(WEAPON_REMOTE_SATCHEL_CHARGE, 5, true);
    GiveWeapon(WEAPON_PISTOL_SILENCED, 40, true);
    GiveWeapon(WEAPON_SPAS12_SHOTGUN, 30, true);
    GiveWeapon(WEAPON_MP5, 100, true);
    GiveWeapon(WEAPON_M4, 150, true);
    GiveWeapon(WEAPON_RLAUNCHER_HS, 200, true);
}

/*!
 * @notsa
 */
void CPed::GiveWeaponSet4() {
    // todo: GiveWeapon(WEAPON_INFRARED, 200, true);
    // todo: GiveWeapon(WEAPON_NIGHTVISION, 200, true);
    GiveWeapon(WEAPON_MINIGUN, 500, true);
    GiveWeapon(WEAPON_DILDO2, 0, true);
}

/*!
* @addr 0x5E61F0
* @brief Set current weapon to be the one in \a slot
*/
void CPed::SetCurrentWeapon(int32 slot) {
    if (slot == -1) {
        return;
    }

    // Remove current weapon's model (if any)
    if (const auto currWepType = GetActiveWeapon().m_Type; currWepType != WEAPON_UNARMED) {
        RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(currWepType)->m_nModelId1);
    }

    // Set as active slot
    m_nActiveWeaponSlot = slot;

    // Set chosen weapon in player data
    if (const auto playerData = AsPlayer()->m_pPlayerData) {
        playerData->m_nChosenWeapon = slot;
    }

    // Load weapon in this slot (if any)
    if (const auto wepInSlotType = m_aWeapons[slot].m_Type; wepInSlotType != WEAPON_UNARMED) {
        AddWeaponModel(CWeaponInfo::GetWeaponInfo(wepInSlotType)->m_nModelId1);
    }
}

/*!
* @addr 0x5E6280
* @brief Set current weapon as \a weaponType
*/
void CPed::SetCurrentWeapon(eWeaponType weaponType) {
    SetCurrentWeapon(GetWeaponSlot(weaponType));
}

/*!
* @addr  0x5E62B0
* @brief If ped's current weapon is \a weaponType clear it, and set current weapon as UNARMED
*/
void CPed::ClearWeapon(eWeaponType weaponType)
{
    auto wepSlot = CWeaponInfo::GetWeaponInfo(weaponType)->m_nSlot;
    if (wepSlot == -1) {
        return; // Weapon has no slot. (How could this happen?)
    }

    auto& wep = m_aWeapons[wepSlot];
    if (wep.m_Type != weaponType) {
        return; // Slot doesn't contain the given weapon - Might happen as some weapons share slots.
    }

    if (m_nActiveWeaponSlot == wepSlot) {
        SetCurrentWeapon(WEAPON_UNARMED);
    }

    wep.Shutdown();

    switch (weaponType) {
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
        RemoveGogglesModel();
        break;
    }
}

/*!
* @addr 0x5E6320
* @brief Clears every weapon from the ped.
*/
void CPed::ClearWeapons()
{
    RemoveWeaponModel(MODEL_INVALID);
    RemoveGogglesModel();
    for (auto& m_aWeapon : m_aWeapons) {
        m_aWeapon.Shutdown();
    }
    CWeaponInfo* getWeaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_UNARMED, eWeaponSkill::STD);
    SetCurrentWeapon(getWeaponInfo->m_nSlot);
}

/*!
* @addr 0x5E6370
* @brief Saves ped's in foot weapon and equips a weapon that can be used in a vehicle if available.
* @param isJetpack  1: when entering jetpack, 0: any other vehicle
*/
void CPed::RemoveWeaponWhenEnteringVehicle(int32 isJetpack) {
    assert(isJetpack == 0 || isJetpack == 1);
    if (m_pPlayerData) {
        m_pPlayerData->m_bInVehicleDontAllowWeaponChange = true;
    }

    if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        return;
    }

    const auto SaveCurrentWeaponAndEquipInSlot = [&](eWeaponSlot slot) {
        // if (m_nSavedWeapon == WEAPON_UNIDENTIFIED) // always true
        m_nSavedWeapon = GetActiveWeapon().GetType();
        SetCurrentWeapon(GetWeaponInSlot(slot).GetWeaponInfo().m_nSlot);
    };

    const auto EquipHandgunIfPossible = [&] {
        const auto &shotgun = GetWeaponInSlot(eWeaponSlot::SHOTGUN), &handgun = GetWeaponInSlot(eWeaponSlot::HANDGUN);
        if (shotgun.GetType() == WEAPON_SAWNOFF_SHOTGUN && shotgun.GetTotalAmmo() > 0 || handgun.GetType() == WEAPON_PISTOL && handgun.GetTotalAmmo() > 0) {
            SaveCurrentWeaponAndEquipInSlot(eWeaponSlot::HANDGUN);
        } else {
            RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(this, eWeaponSkill::STD)->m_nModelId1);
        }
    };

    const auto HandleDriveByWeapons = [&](const CWeapon& w) {
        if (w.GetTotalAmmo() > 0) {
            SaveCurrentWeaponAndEquipInSlot(eWeaponSlot::SMG);
        } else if (isJetpack != 1) {
            RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(this, eWeaponSkill::STD)->m_nModelId1);
        } else {
            EquipHandgunIfPossible();
        }
    };

    if (!IsPlayer() || !AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy) {
        return RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(this, eWeaponSkill::STD)->m_nModelId1);
    }

    if (const auto& w = GetWeaponInSlot(eWeaponSlot::SMG); notsa::contains({ WEAPON_MICRO_UZI, WEAPON_TEC9 }, w.GetType())) {
        return HandleDriveByWeapons(w);
    }

    if (isJetpack == 1) {
        // Whole logic of this function is complicated asf because the player is not
        // supposed to be able to use MP5 while using jetpack. Only Tec-9 or Uzi.
        return EquipHandgunIfPossible();
    }

    if (const auto& w = GetWeaponInSlot(eWeaponSlot::SMG); w.GetType() == eWeaponType::WEAPON_MP5) {
        return HandleDriveByWeapons(w);
    }
}

/*!
* @addr  0x5E6490
* @brief If ped has no saved weapon and is not a player load current weapon's model, otherwise set saved weapon as current.
*/
void CPed::ReplaceWeaponWhenExitingVehicle() {
    if (m_pPlayerData) {
        m_pPlayerData->m_bInVehicleDontAllowWeaponChange = false;
    }

    if (IsPlayer() && m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        SetCurrentWeapon(m_nSavedWeapon);
        m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    } else {                                                           // Not player, or has no saved weapon
        AddWeaponModel(GetActiveWeapon().GetWeaponInfo().m_nModelId1); // Load current active weapon
    }
}

/*!
* @addr 0x5E6530
* @brief Save ped's current weapon and set current to UNARMED.
*/
void CPed::ReplaceWeaponForScriptedCutscene()
{
    m_nSavedWeapon = GetActiveWeapon().m_Type;
    SetCurrentWeapon(0);
}

/*!
* @addr 0x5E6550
* @brief Set ped's current weapon to saved weapon
*/
void CPed::RemoveWeaponForScriptedCutscene()
{
    if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        CWeaponInfo* weaponInfo = CWeaponInfo::GetWeaponInfo(m_nSavedWeapon, eWeaponSkill::STD);
        SetCurrentWeapon(weaponInfo->m_nSlot);
        m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    }
}

/*!
* @addr 0x5E65A0
*/
void CPed::PreRenderAfterTest()
{
    auto* const intel = GetIntelligence();
    if (const auto* swim = intel->GetTaskSwim()) {
        swim->ApplyRollAndPitch(this);
        m_pedIK.bSlopePitch = false;
    } else if (auto* jetpack = intel->GetTaskJetPack()) {
        jetpack->ApplyRollAndPitch(this);
        m_pedIK.bSlopePitch = false;
    }

    if (intel->GetTaskInAir()) {
        m_pedIK.bSlopePitch = false;
    } else if (m_pedIK.bSlopePitch || !IsPlayer() && m_pedIK.m_fSlopePitch != 0.0f) {
        m_pedIK.PitchForSlope();
    }
    bCalledPreRender = true;
    UpdateRpHAnim();

    if (!CTimer::bSkipProcessThisFrame && m_pWeaponObject && m_pPlayerData) {
        if (GetActiveWeapon().GetType() == eWeaponType::WEAPON_MINIGUN) {
            if (const auto f = CClumpModelInfo::GetFrameFromName(m_pWeaponObject, "minigun2")) {
                RwMatrixRotate(
                    RwFrameGetMatrix(f),
                    &CPedIK::XaxisIK,
                    RadiansToDegrees(CTimer::GetTimeStep() * m_pPlayerData->m_fGunSpinSpeed),
                    rwCOMBINEPRECONCAT
                );
            }
        }
    }

    if (m_bIsVisible && CTimeCycle::m_CurrentColours.m_nShadowStrength != 0) {
        const auto [shadowNeeded, activeTask] = [&]() -> std::pair<bool, CTask*> {
            if (!bInVehicle) {
                return std::make_pair(false, intel->m_TaskMgr.FindActiveTaskByType(TASK_COMPLEX_ENTER_ANY_CAR_AS_DRIVER));
            }

            return std::make_pair(intel->m_TaskMgr.FindActiveTaskFromList({ TASK_COMPLEX_LEAVE_CAR, TASK_COMPLEX_DRAG_PED_FROM_CAR }) != nullptr, nullptr);
        }();

        // Low quality circle below feet shadow
        const auto DrawDummyShadow = [&] {
            if (!m_pShadowData && (!bInVehicle || shadowNeeded)) {
                CShadows::StoreShadowForPedObject(
                    this,
                    CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
                    CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],
                    CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
                    CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],
                    CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
                    CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
                );
            }
        };

        // FIX_BUGS: Original check was only for 1st player in high FX quality.
        if (g_fx.GetFxQuality() != FX_QUALITY_VERY_HIGH && (g_fx.GetFxQuality() != FX_QUALITY_HIGH || !IsPlayer())) {
            DrawDummyShadow();
        } else if (const auto b = GetBonePosition(eBoneTag::BONE_ROOT); DistanceBetweenPoints2D(b, TheCamera.GetPosition2D()) <= MAX_DISTANCE_PED_SHADOWS_SQR) {
            const auto IsVehicleRTShadable = [](eVehicleType t) {
                switch (t) {
                case VEHICLE_TYPE_BMX:
                case VEHICLE_TYPE_BIKE:
                case VEHICLE_TYPE_QUAD:
                    return true;
                default:
                    return false;
                }
            };

            auto drawRealTimeShadow = true;
            if (!physicalFlags.bSubmergedInWater) {
                if (const auto* veh = GetVehicleIfInOne()) {
                    drawRealTimeShadow = IsVehicleRTShadable(veh->m_nVehicleSubType);
                }

                if (activeTask) {
                    drawRealTimeShadow = false;
                    if (const auto* targetVeh = notsa::cast<CTaskComplexEnterCarAsDriver>(activeTask)->GetTargetCar()) {
                        drawRealTimeShadow = IsVehicleRTShadable(targetVeh->m_nVehicleSubType);
                    }
                }

                if (const auto bsp = GetBonePosition(eBoneTag::BONE_SPINE1); IsAlive() && GetPosition().z - 0.2f > bsp.z) {
                    drawRealTimeShadow = bIsDucking;
                }

                if (drawRealTimeShadow) {
                    g_realTimeShadowMan.DoShadowThisFrame(this);
                    DrawDummyShadow();
                }
            }
        }
    }

    if (GetModelID() == MODEL_PLAYER) {
        ShoulderBoneRotation(m_pRwClump);
        m_bDontUpdateHierarchy = true;
    }
    float windMod{};
    const auto rainAffectsPlayer = IsPlayer() && CWindModifiers::FindWindModifier(GetPosition(), &windMod, &windMod) && !CCullZones::PlayerNoRain();
    const auto drivingOpenTopVeh = IsStateDriving() && IsInVehicle() && (m_pVehicle->IsBike() || m_pVehicle->IsAutomobile() && m_pVehicle->IsOpenTopCar());

    const auto GetHierMatrix = [h = GetAnimHierarchyFromSkinClump(m_pRwClump)](AnimationId id) {
        return &RpHAnimHierarchyGetMatrixArray(h)[RpHAnimIDGetIndex(h, id)];
    };

    if (!m_pPlayerData || !m_pPlayerData->m_pPedClothesDesc->IsWearingModel("vest") && !m_pPlayerData->m_pPedClothesDesc->IsWearingModel("torso")) {
        if (rainAffectsPlayer || drivingOpenTopVeh) {
            float vehSpeed = drivingOpenTopVeh ? m_pVehicle->GetMoveSpeed().Magnitude() : 0.0f;

            if (rainAffectsPlayer) {
                vehSpeed = std::max(vehSpeed, std::abs(windMod - 1.0f));
            }

            static constexpr float flt_8D1378 = 0.2f, flt_8D1380 = 0.2f;
            static constexpr float flt_8D137C = 0.1f;

            const auto ScaleAnimHierMat = [GetHierMatrix](float range, AnimationId id) {
                const CVector scale{
                    CGeneral::GetRandomNumberInRange(1.0f - range, 1.0f + range),
                    CGeneral::GetRandomNumberInRange(1.0f - range, 1.0f + range),
                    CGeneral::GetRandomNumberInRange(1.0f - range, 1.0f + range),
                };
                RwMatrixScale(GetHierMatrix(id), &scale, rwCOMBINEPRECONCAT);
                return scale;
            };

            ScaleAnimHierMat(flt_8D1378 * vehSpeed, ANIM_ID_ROADCROSS);
            auto scale = ScaleAnimHierMat(flt_8D137C * vehSpeed, ANIM_ID_SHOT_RIGHTP);
            RwMatrixScale(GetHierMatrix(ANIM_ID_GAS_CWR), &scale, rwCOMBINEPRECONCAT);
            if (drivingOpenTopVeh || !intel->GetTaskJetPack()) {
                RwMatrixScale(GetHierMatrix(ANIM_ID_IDLE), &scale, rwCOMBINEPRECONCAT);
            }
            scale = ScaleAnimHierMat(flt_8D1380 * vehSpeed, ANIM_ID_HIT_FRONT);
            RwMatrixScale(GetHierMatrix(ANIM_ID_KD_LEFT), &scale, rwCOMBINEPRECONCAT);
        }
    }

    if (bIsTalking && m_nBodypartToRemove == 2) {
        const CVector scale{};
        RwMatrixScale(GetHierMatrix(ANIM_ID_WALK_START), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(ANIM_ID_IDLE_HBHB_0), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(ANIM_ID_RUN_STOP), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(ANIM_ID_RUN_STOPR), &scale, rwCOMBINEPRECONCAT);
    }

    if (m_Wobble > 0.0f) {
        static constexpr float WOBBLE_FACTOR = 5.0f; // 0x8D21F0

        const auto angle = std::sin(m_Wobble) * -WOBBLE_FACTOR;
        m_Wobble -= CTimer::GetTimeStep() * m_WobbleSpeed;

        if (IsPlayer()) {
            RwMatrixRotate(GetHierMatrix(ANIM_ID_SMKCIG_PRTL_F), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
            RwMatrixRotate(GetHierMatrix(ANIM_ID_DRNKBR_PRTL_F), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
        }
        RwMatrixRotate(GetHierMatrix(ANIM_ID_BIKE_HIT), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
    }

    if (CWeather::Earthquake > 0.0f) {
        const auto swing = CGeneral::GetRandomNumberInRange(-CWeather::Earthquake, CWeather::Earthquake);

        RwMatrixRotate(GetHierMatrix(ANIM_ID_FIGHTSH_LEFT), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_GUNMOVE_BWD), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_HIT_L), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_KD_RIGHT), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_HIT_FRONT), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_KD_LEFT), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_FIGHTSH_BWD), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_GUNMOVE_R), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_HIT_BACK), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_KO_SKID_FRONT), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        RwMatrixRotate(GetHierMatrix(ANIM_ID_WALK_START), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
    }

    if (bIsTalking && m_nBodypartToRemove == 2 && !IsStateDead() && !bIsDyingStuck && CTimer::GetFrameCounter() % 8 > 3) {
        g_fx.AddBlood(GetHierMatrix(ANIM_ID_WALK_START)->pos, 0.6f * GetUp(), 16, m_fContactSurfaceBrightness);
    }

    if (CWeather::Rain > 0.3f
        && TheCamera.m_fSoundDistUp > 15.0f
        && !bInVehicle
        && CGame::CanSeeOutSideFromCurrArea()
        && GetPosition().z < 900.0f
        && !CCullZones::CamNoRain()
    ) {
        if (DistanceBetweenPoints(TheCamera.GetPosition(), GetPosition()) < 25.0f) {
            auto* const pedModelInfo = GetModelInfo()->AsPedModelInfoPtr();
            pedModelInfo->AnimatePedColModelSkinnedWorld(m_pRwClump);

            if (const auto& s = FindPlayerSpeed(); std::abs(s.x) <= 0.05f
                && std::abs(s.y) <= 0.05f
                && !IsStateDying()
                && !notsa::contains({ PEDSTATE_FALL, PEDSTATE_ATTACK, PEDSTATE_FIGHT }, m_nPedState)
                && IsPedHeadAbovePos(0.3f)
                && !RpAnimBlendClumpGetAssociation(m_pRwClump, ANIM_ID_IDLE_TIRED)
            ) {
                const auto* colData = pedModelInfo->GetColModel()->GetData();
                for (const auto& sphere : colData->GetSpheres()) {
                    if (notsa::contains(std::initializer_list<uint8>{ 5, 6, 9 }, sphere.m_Surface.m_nPiece)) {
                        FxPrtMult_c p{ 1.0f, 1.0f, 1.0f, 0.35f, 0.01f, 0.0f, 0.03f };
                        CVector     pos = sphere.m_vecCenter;
                        pos.x += CGeneral::GetRandomNumberInRange(-0.08f, 0.08f);
                        pos.y += CGeneral::GetRandomNumberInRange(-0.08f, 0.08f);
                        pos.z += CGeneral::GetRandomNumberInRange(-0.08f, 0.02f);
                        CVector vel = s * 50.0f;
                        g_fx.m_Splash->AddParticle(&pos, &vel, 0.0f, &p, -1.0f, 1.2f, 0.6f, false);
                    }
                }
            }
        }
    }

    if (m_pPlayerData && m_pPlayerData->m_nWetness && m_pPlayerData->m_nWaterCoverPerc < 30u) {
        FxPrtMult_c p{1.0f, 1.0f, 1.0f, 0.2f, 0.15f, 0.0f, 0.1f};
        CVector     pos = GetPosition();
        pos.x += CGeneral::GetRandomNumberInRange(-0.03f, 0.03f);
        pos.y += CGeneral::GetRandomNumberInRange(-0.03f, 0.03f);
        pos.z += CGeneral::GetRandomNumberInRange(-0.8f, 0.2f);
        p.m_Color.alpha *= (float)m_pPlayerData->m_nWetness / 100.0f;
        CVector vel{};
        g_fx.m_WaterSplash->AddParticle(&pos, &vel, 0.0f, &p, -1.0f, 1.2f, 0.6f, false);
    }

    if (const auto* veh = GetVehicleIfInOne()) {
        m_fContactSurfaceBrightness = veh->m_fContactSurfaceBrightness;
    }
}

/*!
* @addr 0x5E7980
* @brief Set ped's state to IDLE
*/
void CPed::SetIdle() {
    switch (m_nPedState) {
    case PEDSTATE_IDLE:
    case PEDSTATE_MUG:
    case PEDSTATE_FLEE_ENTITY:
        break;

    case PEDSTATE_AIMGUN:
        m_nPedState = PEDSTATE_IDLE;
        m_nMoveState = PEDMOVE_STILL;
        break;

    default:
        m_nMoveState = PEDMOVE_STILL;
        break;
    }
}

/*!
* @addr 0x5E79B0
* @brief Set ped to look in direction \a heading
*/
void CPed::SetLook(float heading) {
    if (!IsPedInControl()) {
        return;
    }

    m_nPedState = PEDSTATE_LOOK_HEADING;

    if (m_nLookTime >= CTimer::GetTimeInMS()) {
        return;
    }

    bIsLooking = true;
    m_fLookDirection = heading;
    m_nLookTime = 0;

    ClearReference(m_pLookTarget);

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5E7A60
* @brief Set ped to look at \a entity
*/
void CPed::SetLook(CEntity* entity) {
    if (IsPedInControl()) {
        m_nPedState = PEDSTATE_LOOK_ENTITY;
        SetLookFlag(entity, false, false);
    }
}

/*!
* @addr 0x5E7B20
* @copydoc TurnBody
*/
void CPed::Look() {
    TurnBody();
}

/*!
* @addr 0x5E7CB0
* @brief Attach a weapon to us or something, not sure.
* @returns \a entity
*/
CEntity* CPed::AttachPedToEntity(CEntity* entity, CVector offset, uint16 turretAngleA, float turretAngleB, eWeaponType weaponType) {
    if (!entity || bInVehicle) {
        return nullptr;
    }

    // BUG/NOTE: ClearReference not called here?
    m_pAttachedTo = entity->AsPhysical();
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));

    m_vecTurretOffset = offset;
    m_fTurretAngleB = turretAngleB;
    m_fTurretAngleA = turretAngleA;

    // Deal collision with `entity`
    if (!IsPlayer()) {
        if (entity->IsVehicle()) {
            m_pEntityIgnoredCollision = entity->AsPhysical();
        }
    } else { // For player just disable collision
        m_bUsesCollision = false;
    }

    if (m_nSavedWeapon == WEAPON_UNIDENTIFIED) {
        m_nSavedWeapon = GetActiveWeapon().m_Type;
        m_nTurretAmmo = GetActiveWeapon().m_TotalAmmo; // todo: unify types
    }

    if (!IsPlayer()) {
        GiveWeapon(weaponType, 30'000, true);
        SetCurrentWeapon(weaponType);
        PositionAttachedPed();
    } else {
        if (weaponType != WEAPON_UNARMED) {
            GiveWeapon(weaponType, 30'000, true);
        }

        m_pPlayerData->m_nChosenWeapon = weaponType;

        if (weaponType == WEAPON_CAMERA) {
            TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_CAMERA);
        } else {
            // With the pool cue we can aim as well, so it needs a different cam mode.
            if (   entity->m_nModelIndex == eModelID::MODEL_POOLCUE
                && !CWeaponInfo::GetWeaponInfo(weaponType)->flags.b1stPerson
            ) {
                TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_AIMWEAPON_ATTACHED);
                m_pPlayerData->m_bFreeAiming = true;
            } else {
                TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_HELICANNON_1STPERSON);
            }
        }
        m_nPedState = PEDSTATE_SNIPER_MODE;
        PositionAttachedPed();
    }

    return entity;
}

/*!
* @addr 0x5E7E60
*/
void CPed::AttachPedToBike(CEntity* entity, CVector offset, uint16 turretAngleA, float turretAngleB, float turretPosnMode, eWeaponType weaponType) {
    if (AttachPedToEntity(entity, offset, turretAngleA, turretAngleB, weaponType)) {
        m_nTurretPosnMode = turretPosnMode;
    }
}

/*!
* @addr 0x5E7EC0
*/
void CPed::DettachPedFromEntity(){
    const auto wasAttachedTo = m_pAttachedTo;

    // BUG/NOTE: Again, ClearOldReference not called.
    m_pAttachedTo = nullptr;

    switch (m_nPedState) {
    case PEDSTATE_DIE: {
        m_pEntityIgnoredCollision = wasAttachedTo;
        ApplyMoveForce(wasAttachedTo->GetMatrix().GetForward() * -4.f);
        bIsStanding = false;
        break;
    }
    case PEDSTATE_DEAD: // Skip this
        break;

    default: {
        CAnimManager::BlendAnimation(m_pRwClump, m_nAnimGroup, ANIM_ID_IDLE, 1000.f);
        bIsStanding = true;

        // Restore old weapon if any
        if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
            GetActiveWeapon().m_AmmoInClip = 0;
            GetActiveWeapon().m_TotalAmmo = 0;

            SetCurrentWeapon(m_nSavedWeapon);
            GetActiveWeapon().m_TotalAmmo = (uint32)m_nTurretAmmo;

            m_nSavedWeapon = WEAPON_UNIDENTIFIED;
        }

        if (IsPlayer()) {
            AsPlayer()->ClearWeaponTarget();
        }

        break;
    }
    }
}

/*!
* @addr 0x5E8830
*/
void CPed::SetAimFlag(float heading) {
    bIsAimingGun = true;
    bIsRestoringGun = false;
    ClearReference(m_pLookTarget);
    m_nLookTime = 0;

    if (bIsDucking) {
        m_pedIK.bUseArm = false;
    } // It's intentionally overwritten

    m_pedIK.bUseArm = CanWeRunAndFireWithWeapon();
}

/*!
* @addr 0x5E88E0
*/
bool CPed::CanWeRunAndFireWithWeapon() {
    return GetActiveWeapon().GetWeaponInfo(this).flags.bAimWithArm;
}

/*!
* @addr 0x5E8910
*/
void CPed::RequestDelayedWeapon() {
    if (m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        return;
    }

    // Simplified a little using an array. Originally it had too much copy paste.

    const auto models = CWeaponInfo::GetWeaponInfo(m_nDelayedWeapon)->GetModels();

    // Request models
    for (auto model : models) {
        if (model != -1) {
            CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY);
        }
    }

    // If it has no model, or at least one model is loaded..
    if (rng::all_of(models, [](auto m) { return m == -1 || CStreaming::IsModelLoaded(m); })) {
        GiveWeapon(m_nDelayedWeapon, m_nDelayedWeaponAmmo, true);
        m_nDelayedWeapon = WEAPON_UNIDENTIFIED;
    }
}

/*!
* @addr 0x5E89B0
* @brief Set delayed weapon (If ped doesn't already have one). Also drops entity in hand unless ped is a player.
*/
void CPed::GiveDelayedWeapon(eWeaponType weaponType, uint32 ammo) {
    // If not a player drop entity in ped's hand (if any)
    if (!IsPlayer()) {
        if (const auto task = (CTaskSimpleHoldEntity*)GetIntelligence()->GetTaskHold(false)) {
            if (task->m_pEntityToHold) {
                if (task->m_nBoneFrameId == ePedNode::PED_NODE_RIGHT_HAND) {
                    DropEntityThatThisPedIsHolding(true);
                }
            }
        }
    }

    // Set delayed weapon (If ped doesn't already have one)
    if (m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        m_nDelayedWeaponAmmo = ammo;
        m_nDelayedWeapon = weaponType;
        RequestDelayedWeapon();
    }
}

/*!
* @addr 0x5E8A30
*/
bool IsPedPointerValid(CPed* ped)
{
    return ((bool(__cdecl *)(CPed*))0x5E8A30)(ped);
}

/*!
* @addr 0x5E8AB0
*/
void CPed::GiveWeaponAtStartOfFight()
{
    if (m_nCreatedBy != PED_MISSION && GetActiveWeapon().m_Type == WEAPON_UNARMED)
    {
        const auto GiveRandomWeaponByType = [this](eWeaponType type, uint16 maxRandom)
        {
            if ((m_nRandomSeed % 1024) >= maxRandom)
                return;

            if (m_nDelayedWeapon != WEAPON_UNIDENTIFIED)
                return;

            GiveDelayedWeapon(type, 50);
            SetCurrentWeapon(GetWeaponSlot(type));
        };

        switch (m_nPedType)
        {
        case PED_TYPE_GANG1:
        case PED_TYPE_GANG2:
        case PED_TYPE_GANG3:
        case PED_TYPE_GANG4:
        case PED_TYPE_GANG5:
        case PED_TYPE_GANG6:
        case PED_TYPE_GANG7:
        case PED_TYPE_GANG8:
        case PED_TYPE_GANG9:
        case PED_TYPE_GANG10:
            GiveRandomWeaponByType(WEAPON_PISTOL, 400);
            break;
        case PED_TYPE_DEALER:
        case PED_TYPE_CRIMINAL:
        case PED_TYPE_PROSTITUTE:
            GiveRandomWeaponByType(WEAPON_KNIFE, 200);
            GiveRandomWeaponByType(WEAPON_PISTOL, 400);
            break;
        default:
            break;
        }
    }
}

/*!
* @addr 5E8BE0
* @brief If ped has no weapons give them one. (AK-47 if `CHEAT_NO_ONE_CAN_STOP_US` is active, RLauncher if `CHEAT_ROCKET_MAYHEM` is active, or a pistol otherwise)
*/
void CPed::GiveWeaponWhenJoiningGang()
{
    if (GetActiveWeapon().m_Type == WEAPON_UNARMED && m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        if (CCheat::IsActive(CHEAT_NO_ONE_CAN_STOP_US)) {
            GiveDelayedWeapon(WEAPON_AK47, 200);
            SetCurrentWeapon(CWeaponInfo::GetWeaponInfo(WEAPON_AK47, eWeaponSkill::STD)->m_nSlot);
        }
        else {
            CWeaponInfo* weaponInfo = nullptr;
            if (CCheat::IsActive(CHEAT_ROCKET_MAYHEM)) {
                GiveDelayedWeapon(WEAPON_RLAUNCHER, 200);
                weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_RLAUNCHER, eWeaponSkill::STD);
            }
            else {
                GiveDelayedWeapon(WEAPON_PISTOL, 200);
                weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_PISTOL, eWeaponSkill::STD);
            }
            SetCurrentWeapon(weaponInfo->m_nSlot);
        }
    }
}

/*!
* @addr 0x5EFF50
*/
bool CPed::GetPedTalking() {
    return m_pedSpeech.GetPedTalking();
}

/*!
* @addr 0x5EFF60
*/
void CPed::DisablePedSpeech(bool stopCurrentSpeech) {
    m_pedSpeech.DisablePedSpeech(stopCurrentSpeech);
}

/*!
* @addr 0x5EFF70
*/
void CPed::EnablePedSpeech() {
    m_pedSpeech.EnablePedSpeech();
}

/*!
* @addr 0x5EFF80
*/
void CPed::DisablePedSpeechForScriptSpeech(bool stopCurrentSpeech) {
    m_pedSpeech.DisablePedSpeechForScriptSpeech(stopCurrentSpeech);
}

/*!
* @addr 0x5EFF90
*/
void CPed::EnablePedSpeechForScriptSpeech() {
    m_pedSpeech.EnablePedSpeechForScriptSpeech();
}

/*!
* @addr 0x5EFFA0
*/
bool CPed::CanPedHoldConversation() const {
    return m_pedSpeech.CanPedHoldConversation();
}

/*!
* @addr 0x5EFFB0
*/
void CPed::SayScript(eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    m_pedSpeech.AddScriptSayEvent(AE_SCRIPT_SPEECH_PED, scriptID, overrideSilence, isForceAudible, isFrontEnd);
}

/*!
* @addr 0x5EFFE0
* @returns Played soundID - TODO: I'm not sure about this..
*/
int16 CPed::Say(eGlobalSpeechContext gCtx, uint32 startTimeDelay, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    return gCtx != CTX_GLOBAL_NO_SPEECH
        ? m_pedSpeech.AddSayEvent(AE_SPEECH_PED, gCtx, startTimeDelay, probability, overrideSilence, isForceAudible, isFrontEnd)
        : -1;
}

/*!
* @addr 0x5F0060
*/
RwObject* SetPedAtomicVisibilityCB(RwObject* rwObject, void* data) {
    if (!data) {
        rwObjectSetFlags(rwObject, 0); // TODO: Figure out what the flag is
    }
    return rwObject;
}

/*!
* @addr 0x5F0140
* @brief Remove body part
* @todo See if it works with body parts other than the head
*/
void CPed::RemoveBodyPart(ePedNode pedNode, char localDir) {
    UNUSED(localDir);

    if (m_apBones[pedNode]->KeyFrame) {
        if (CLocalisation::ShootLimbs()) {
            bRemoveHead = true;
            m_nBodypartToRemove = pedNode;
        }
    } else {
        DEV_LOG("Trying to remove ped component");
    }
}

/*!
* @addr 0x5F0190
*/
void CPed::SpawnFlyingComponent(int32 arg0, char arg1)
{
    // NOP
}

/*!
* @addr 0x5F01A0
*/
/*!
* @addr 0x5F01A0
* @brief Check if line of sight bullet would hit the ped (Does a basic check of colpoint.point.z against head position)
* @returns 0, 1 - Yes , 2 - No. Always `1` if ped is falling.
*/
uint8 CPed::DoesLOSBulletHitPed(CColPoint& colPoint) {
    // TODO: Below is just a copy of the code in `IsPedHeadAbovePos` - A separate function should be made.
    RwV3d zero{}; // Placeholder - 0, 0, 0
    RwV3d headPos{};

    // TODO: Doesn't this just return the position of the matrix? Eg.: `BoneMatrix.pos` ?
    RwV3dTransformPoint(&headPos, &zero, GetBoneMatrix((eBoneTag)m_apBones[ePedNode::PED_NODE_HEAD]->BoneTag));

    if (m_nPedState == PEDSTATE_FALL || colPoint.m_vecPoint.z < headPos.z) { // Ped falling, adjust
        return 1;
    } else if (headPos.z + 0.2f <= colPoint.m_vecPoint.z) {
        return 0;
    } else {
        return 2;
    }
}

/*!
* @addr  0x5F0250
* @brief Remove all weapon animations by blending them into the IDLE animation.
* @param likeUnused Unused
*/
void CPed::RemoveWeaponAnims(int32 likeUnused, float blendDelta) {
    UNUSED(likeUnused);

    bool bFoundNotPartialAnim{};
    for (auto i = 0; i < 34; i++) { // TODO: Magic number `34`
        if (const auto assoc = RpAnimBlendClumpGetAssociation(m_pRwClump, ANIM_ID_FIRE)) {
            assoc->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            if ((assoc->m_Flags & ANIMATION_IS_PARTIAL)) {
                assoc->m_BlendDelta = blendDelta;
            } else {
                bFoundNotPartialAnim = true;
            }
        }
    }

    if (bFoundNotPartialAnim) {
        CAnimManager::BlendAnimation(m_pRwClump, m_nAnimGroup, ANIM_ID_IDLE, -blendDelta);
    }
}

/*!
* @addr 0x5F02C0
* @returns If world space \a zPos is above ped's head
*/
bool CPed::IsPedHeadAbovePos(float zPos) {
    RwV3d zero{}; // Placeholder - 0, 0, 0
    RwV3d headPos{};

    // TODO: Doesn't this just return the position of the matrix? Eg.: `BoneMatrix.pos` ?
    RwV3dTransformPoint(&headPos, &zero, GetBoneMatrix((eBoneTag)m_apBones[ePedNode::PED_NODE_HEAD]->BoneTag));

    return zPos + GetPosition().z < headPos.z;
}

/*!
* @addr 0x5F0360
*/
void CPed::KillPedWithCar(CVehicle* car, float fDamageIntensity, bool bPlayDeadAnimation)
{
    ((void(__thiscall *)(CPed*, CVehicle*, float, bool))0x5F0360)(this, car, fDamageIntensity, bPlayDeadAnimation);
}

/*!
* @addr 0x6AE0D0
*/
template<typename PtrListType>
void CPed::MakeTyresMuddySectorList(PtrListType& ptrList)
{
    ((void(__thiscall *)(CPed*, PtrListType&))0x6AE0D0)(this, ptrList);
}

/*!
* @addr  0x6B4200
* @brief Do sector list processing in a range of -/+2 (Calls \r MakeTyresMuddySectorList)
*/
void CPed::DeadPedMakesTyresBloody() {
    const auto& pos = GetPosition();
    const auto startSectorX = std::max(CWorld::GetLodSectorX(pos.x - 2.0f), 0);
    const auto startSectorY = std::max(CWorld::GetLodSectorY(pos.y - 2.0f), 0);
    const auto endSectorX   = std::min(CWorld::GetLodSectorX(pos.x + 2.0f), MAX_LOD_PTR_LISTS_X - 1);
    const auto endSectorY   = std::min(CWorld::GetLodSectorY(pos.y + 2.0f), MAX_LOD_PTR_LISTS_Y - 1);

    CWorld::IncrementCurrentScanCode();

    for (int32 sy = startSectorY; sy <= endSectorY; ++sy) {
        for (int32 sx = startSectorX; sx <= endSectorX; ++sx) {
            MakeTyresMuddySectorList(GetRepeatSector(sx, sy)->GetList(REPEATSECTOR_VEHICLES));
        }
    }
}

/*!
* @notsa
* @returns If player is in a vehicle that has a driver as a passenger
*/
bool CPed::IsInVehicleThatHasADriver() {
    if (bInVehicle) { // todo: IsInVehicleAsPassenger - Before refactoring check if `IsPassanger` returns true if `this` is the driver.
        if (m_pVehicle && m_pVehicle->IsPassenger(this) && m_pVehicle->m_pDriver)
            return true;
    }
    return false;
}

/*!
* @notsa
*/
int32 CPed::GetGroupId() {
    return GetGroup()
        ? GetGroup()->GetId()
        : -1;
}

/*!
* @notsa
* @returns If ped is follower of \a group
*/
bool CPed::IsFollowerOfGroup(const CPedGroup& group) const {
    return group.GetMembership().IsFollower(this);
}

/*!
* @notsa
* @returns Bone transformation matrix into object space. To transform to world space ped's matrix must be used as well.
*/
RwMatrix* CPed::GetBoneMatrix(eBoneTag bone) const {
    if (const auto h = GetAnimHierarchyFromClump(m_pRwClump)) {
        return RpHAnimHierarchyGetNodeMatrix(h, bone);
    }
    return nullptr;
}

/*!
* @addr 0x5E4880
* @brief Set model index (Also re-inits animblend, MoneyCount, and default decision-marker)
*/
void CPed::SetModelIndex(uint32 modelIndex) {
    assert(modelIndex != MODEL_PLAYER || IsPlayer());

    m_bIsVisible = true;

    CEntity::SetModelIndex(modelIndex);

    RpAnimBlendClumpInit(m_pRwClump);
    RpAnimBlendClumpFillFrameArray(m_pRwClump, m_apBones.data());

    auto* mi = GetModelInfo()->AsPedModelInfoPtr();

    SetPedStats(mi->m_nStatType);
    RestoreHeadingRate();

    SetPedDefaultDecisionMaker();

    // Set random money count
    const auto GetRandomMoneyCount = [this]() -> int16 {
        if (CGeneral::GetRandomNumberInRange(0.f, 100.f) < 3.f) { // Moved up here
            return 400;
        } else if (CPopCycle::IsPedInGroupTheseGroups(m_nModelIndex, { POPCYCLE_GROUP_BUSINESS, POPCYCLE_GROUP_CASUAL_RICH })) {
            return CGeneral::GetRandomNumber() % 50 + 20;
        } else {
            return CGeneral::GetRandomNumber() % 25;
        }
    };
    m_nMoneyCount = GetRandomMoneyCount();

    m_nAnimGroup = mi->m_nAnimType;
    CAnimManager::AddAnimation(m_pRwClump, m_nAnimGroup, ANIM_ID_IDLE);

    if (!CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = true;
    }

    // Deal with animation stuff once again
    RpAnimBlendClumpGetData(m_pRwClump)->m_PedPosition = (CVector*)&m_vecAnimMovingShiftLocal;

    // Create hit col model
    if (!mi->m_pHitColModel) {
        mi->CreateHitColModelSkinned(m_pRwClump);
    }

    // And finally update our rph anim
    UpdateRpHAnim();
}

/*!
* @addr 0x5DEBF0
*/
void CPed::DeleteRwObject()
{
    CEntity::DeleteRwObject();
}

/*!
* @addr 0x5E8CD0
*/
void CPed::ProcessControl()
{
    plugin::CallMethod<0x5E8CD0, CPed*>(this);
}

/*!
* @addr 0x5E4110
* @brief Set player's position (Also resets move/turn speed, and cancels all tasks)
*/
void CPed::Teleport(CVector destination, bool resetRotation) {
    UNUSED(resetRotation);

    if (IsPlayer() || GetTaskManager().HasAnyOf<TASK_COMPLEX_LEAVE_CAR>()) {
        GetIntelligence()->FlushImmediately(true);
    }

    CWorld::Remove(this);
    SetPosn(destination);
    bIsStanding = false;
    ClearReference(m_pDamageEntity);
    // todo: m_pDamageEntity = nullptr;
    CWorld::Add(this);

    m_vecMoveSpeed.Reset();
    m_vecTurnSpeed.Reset();
}

/*!
* @addr 0x5E3C30
*/
void CPed::SpecialEntityPreCollisionStuff(CPhysical* colPhysical,
                                          bool  bIgnoreStuckCheck,
                                          bool& bCollisionDisabled,
                                          bool& bCollidedEntityCollisionIgnored,
                                          bool& bCollidedEntityUnableToMove,
                                          bool& bThisOrCollidedEntityStuck)
{
    plugin::CallMethod<0x5E3C30, CPed*, CPhysical*, bool, bool&, bool&, bool&, bool&>(this, colPhysical, bIgnoreStuckCheck, bCollisionDisabled, bCollidedEntityCollisionIgnored, bCollidedEntityUnableToMove, bThisOrCollidedEntityStuck);
}

/*!
* @addr 0x5E3E90
*/
uint8 CPed::SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2)
{
    return plugin::CallMethodAndReturn<uint8, 0x5E3E90, CPed*, bool&, bool&>(this, bProcessCollisionBeforeSettingTimeStep, unk2);
}

/*!
* @addr 0x5E8A20
*/
void CPed::PreRender()
{
    if (m_nPedState != PEDSTATE_DRIVING)
        PreRenderAfterTest();
}

/*!
* @addr 0x5E7680
*/
void CPed::Render() {
    // Save alpha fn. ref for player peds
    uint32 storedAlphaRef{1};
    if (IsPlayer()) {
        RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(&storedAlphaRef));
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1u));
    }

    // 0x5E76BE
    if (bDontRender || !(m_bIsVisible || CMirrors::ShouldRenderPeds())) {
        return;
    }

    // 0x5E76F9 - 0x5E7735
    // Now do some extra checks if in vehicle (possibly early out)
    if (   bInVehicle
        && m_pVehicle
        && !GetTaskManager().FindActiveTaskFromList({ TASK_COMPLEX_LEAVE_CAR, TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP })
    ) {
        // 0x5E774A
        if (!bRenderPedInCar) {
            return;
        }

        // 0x5E7765 - 0x5E7774
        if (   !m_pVehicle->IsBike()
            && !m_pVehicle->IsSubQuad()
            && !IsPlayer()
        ) {
            // 0x5E77DD
            // Okay, let's check if the ped is close enough to the camera

            const auto IsPedInRangeOfCamera = [this](auto range) {
                const auto distSq = (TheCamera.GetPosition() - GetPosition()).SquaredMagnitude();
                const auto finalRange = range * TheCamera.m_fLODDistMultiplier;
                return distSq < finalRange * finalRange;
            };
            if (!IsPedInRangeOfCamera(m_pVehicle->IsBoat() ? 40.f : 25.f)) { // Boats have bigger range
                return;
            }
        }
    }

    RenderBigHead();
    RenderThinBody();

    // 0x5E77E3
    // Render us (And any extra FX)
    if (CPostEffects::IsVisionFXActive()) {
        CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects(this);
        CPostEffects::NightVisionSetLights();
        CEntity::Render();
        CPostEffects::InfraredVisionRestoreLightsForHeatObjects();
    } else {
        CEntity::Render();
    }

    // 0x5E7817
    // Render weapon (and gun flash) as well. (Done for local player only if flag is set.)
    if (m_pWeaponObject) {
        if (!m_pPlayerData || m_pPlayerData->m_bRenderWeapon) {
            if ((!bInVehicle || !GetIntelligence()->GetTaskSwim()) && !GetIntelligence()->GetTaskHold(false)) {
                weaponPedsForPc_Insert(this);
                if (m_nWeaponGunflashAlphaMP1 > 0 || m_nWeaponGunflashAlphaMP2 > 0) {
                    ResetGunFlashAlpha();
                }
            }
        }
    }

    // 0x5E787C
    // Render goggles object
    if (m_pGogglesObject) {
        const auto* const headMat = GetBoneMatrix(eBoneTag::BONE_HEAD);

        // Update goggle's matrix with head's
        *RwFrameGetMatrix(RpClumpGetFrame(m_pGogglesObject)) = *headMat; // TODO: Is there a better way to do this?

        // Calculate it's new position
        RwV3d pos{0.f, 0.084f, 0.f};                  // Offset
        RwV3dTransformPoints(&pos, &pos, 1, headMat); // Transform offset into the head's space

        RwV3dAssign(RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(m_pGogglesObject))), &pos);
        RwFrameUpdateObjects(RpClumpGetFrame(m_pGogglesObject)); // After changing the position it has to be updated

        RpClumpRender(m_pGogglesObject);
    }

    // 0x5E7927
    // Render JetPack (if any)
    if (const auto task = GetIntelligence()->GetTaskJetPack()) {
        task->RenderJetPack(this);
    }

    bHasBeenRendered = true;

    // 0x5E794D
    // Restore alpha test fn for player peds
    if (IsPlayer()) {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(storedAlphaRef));
    }
}

// https://github.com/gennariarmando/bobble-heads
// NOTSA
void CPed::RenderBigHead() const {
    if (!CCheat::IsActive(CHEAT_BIG_HEAD)) {
        return;
    }

    auto hier = GetAnimHierarchyFromSkinClump(m_pRwClump);
    auto* matrices = RpHAnimHierarchyGetMatrixArray(hier);

    const float scale = 3.0f;
    const CVector s = { scale, scale, scale };
    CVector t = { 0.0f, -(scale / 6.0f) / 10.0f, 0.0f };

    for (auto& bone : { BONE_L_BROW, BONE_R_BROW, BONE_JAW }) {
        auto index = RpHAnimIDGetIndex(hier, bone);
        if (RwMatrix* mat = &matrices[index]) {
            RwMatrixScale(mat, &s, rwCOMBINEPRECONCAT);
            if (bone == BONE_JAW) {
                t.x = ((scale / 8.0f) / 10.0f) / 8.0f;
                t.y /= 8.0f;
            }
            RwMatrixTranslate(mat, &t, rwCOMBINEPRECONCAT);
        }
    }

    auto index = RpHAnimIDGetIndex(hier, BONE_HEAD);
    if (RwMatrix* mat = &matrices[index]) {
        RwMatrixScale(mat, &s, rwCOMBINEPRECONCAT);
    }
}

// NOTSA
void CPed::RenderThinBody() const {
    if (!CCheat::IsActive(CHEAT_THIN_BODY)) {
        return;
    }
}

/*!
 * @addr 0x553F00
 */
bool CPed::SetupLighting() {
  ActivateDirectional();
  return CRenderer::SetupLightingForEntity(this);
}

/*!
* @addr 0x5533B0
*/
void CPed::RemoveLighting(bool bRemove) {
    UNUSED(bRemove);

    if (!physicalFlags.bRenderScorched) {
        CPointLights::RemoveLightsAffectingObject();
    }

    SetAmbientColours();
    DeActivateDirectional();
}

/*!
* @addr 0x5E7B70
*/
void CPed::FlagToDestroyWhenNextProcessed() {
    m_bRemoveFromWorld = true;

    if (!IsInVehicle()) {
        return;
    }

    if (m_pVehicle->IsDriver(this)) {
        ClearReference(m_pVehicle->m_pDriver);
        if (IsPlayer() && m_pVehicle->m_nStatus != STATUS_WRECKED) {
            m_pVehicle->m_nStatus = STATUS_ABANDONED;
        }
    } else {
        m_pVehicle->RemovePassenger(this);
    }

    bInVehicle = false;

    if (IsVehiclePointerValid(m_pVehicle)) {
        SafeCleanUpRef(m_pVehicle);
    }

    m_pVehicle = nullptr;

    SetPedState(IsCreatedByMission() ? PEDSTATE_DEAD : PEDSTATE_NONE);
}

/*!
* @addr 0x5E2530
*/
int32 CPed::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint)
{
    return plugin::CallMethodAndReturn<int32, 0x5E2530, CPed*, CEntity*, CColPoint*>(this, entity, colPoint);
}

// NOTSA
bool CPed::IsInVehicleAsPassenger() const noexcept {
    return bInVehicle && m_pVehicle && m_pVehicle->m_pDriver != this;
}

bool CPed::IsJoggingOrFaster() const {
    switch (m_nMoveState) {
    case PEDMOVE_JOG:
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return true;
    }
    return false;
}

bool CPed::IsRunningOrSprinting() const {
    switch (m_nMoveState) {
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return true;
    }
    return false;
}

bool CPed::IsPedStandingInPlace() const {
    switch (m_nMoveState) {
    case PEDMOVE_NONE:
    case PEDMOVE_STILL:
    case PEDMOVE_TURN_L:
    case PEDMOVE_TURN_R:
        return true;
    }
    return false;
}

// 0x6497A0
bool SayJacked(CPed* jacked, CVehicle* vehicle, uint32 timeDelay) {
    if (vehicle->m_vehicleAudio.GetVehicleTypeForAudio())
        return jacked->Say(CTX_GLOBAL_JACKED_GENERIC, timeDelay) != -1;
    else
        return jacked->Say(CTX_GLOBAL_JACKED_CAR, timeDelay) != -1;
}

// 0x6497F0
bool SayJacking(CPed* jacker, CPed* jacked, CVehicle* vehicle, uint32 timeDelay) {
    if (vehicle->m_vehicleAudio.GetVehicleTypeForAudio() == 1)
        return jacker->Say(CTX_GLOBAL_JACKING_BIKE, timeDelay) != -1;

    if (vehicle->m_vehicleAudio.GetVehicleTypeForAudio())
        return jacker->Say(CTX_GLOBAL_JACKING_GENERIC, timeDelay) != -1;

    if (jacked->GetSpeechAE().IsPedFemaleForAudio())
        return jacker->Say(CTX_GLOBAL_JACKING_CAR_FEM, timeDelay) != -1;

    return jacker->Say(CTX_GLOBAL_JACKING_CAR_MALE, timeDelay) != -1;
}

// NOTSA
int32 CPed::GetPadNumber() const {
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1:
        return 0;
    case PED_TYPE_PLAYER2:
        return 1;
    default:
        assert(true && "Inappropriate usage of GetPadNumber");
        return 0;
    }
}

bool CPed::IsRightArmBlockedNow() const {
    if (bIsDucking) {
        return bDuckRightArmBlocked;
    }
    return bRightArmBlocked;
}
