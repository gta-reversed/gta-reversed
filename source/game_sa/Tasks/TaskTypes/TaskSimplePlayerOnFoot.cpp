#include "StdInc.h"

#include "TaskSimplePlayerOnFoot.h"

#include "TaskSimpleUseGun.h"
#include "TaskSimpleThrowProjectile.h"

#include "TaskComplexJump.h"
#include "TaskComplexUseGoggles.h"
#include "TaskSimpleStealthKill.h"
#include "IKChainManager_c.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleFight.h"
#include "TaskSimpleDuck.h"

int32& gLastRandomNumberForIdleAnimationID = *reinterpret_cast<int32*>(0x8D2FEC);
uint32& gLastTouchTimeDelta = *reinterpret_cast<uint32*>(0xC19664);
float& gDuckAnimBlendData = *reinterpret_cast<float*>(0x8D2FF0); // 4.0f
bool& gbUnknown_8D2FE8 = *reinterpret_cast<bool*>(0x8D2FE8); // default value true; also always true

// 0x685750
CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot() : CTaskSimple() {
    m_nAnimationBlockIndex = CAnimManager::GetAnimationBlockIndex("playidles");
    dword_14 = 0;
    m_nTimer = 0;
    m_pLookingAtEntity = nullptr;
    m_nFrameCounter = CTimer::GetFrameCounter();
}

// 0x6857E0
bool CTaskSimplePlayerOnFoot::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // return plugin::CallMethodAndReturn<bool, 0x6857E0, CTaskSimplePlayerOnFoot*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);

    bool abortable = false;
    auto eventDamage = static_cast<const CEventDamage*>(event);

    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        ped->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.0f);
        abortable = true;
        goto LABEL_15;
    }

    if (priority == ABORT_PRIORITY_URGENT) {
        if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK))
            goto LABEL_12;

        if (event) {
            if (event->GetEventPriority() < 61)
                return false;

            if (event->GetEventType() == EVENT_DAMAGE) {
                auto secondary = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK);
                if (secondary->GetTaskType() == TASK_SIMPLE_THROW || eventDamage->m_bKnockOffPed) {
                    ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
                }
                goto LABEL_12;
            }
        }

        if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        LABEL_12:
            abortable = true;
            goto LABEL_15;
        }

        abortable = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(ped, ABORT_PRIORITY_URGENT, event);
        if (abortable) {
        LABEL_15:
            if (
                (ped->m_pTargetedObject || (ped->m_pPlayerData->m_nPlayerFlags & 8) != 0 || TheCamera.Using1stPersonWeaponMode())
                && event && (event->GetEventType() == EVENT_DAMAGE || event->GetEventType() == EVENT_IN_WATER)
            ) {
                if (   event->GetEventType() != EVENT_DAMAGE
                    || eventDamage->m_damageResponse.m_bHealthZero
                    && eventDamage->m_bAddToEventGroup
                    || !ped->m_pAttachedTo
                    && (
                         eventDamage->m_bKnockOffPed
                      || eventDamage->m_weaponType > WEAPON_LAST_WEAPON
                      && eventDamage->m_weaponType != WEAPON_UZI_DRIVEBY
                    )
                ) {
                    TheCamera.ClearPlayerWeaponMode();
                    CWeaponEffects::ClearCrossHair(ped->m_nPedType);
                    if (ped->m_pTargetedObject) {
                        ped->m_pTargetedObject->CleanUpOldReference(&ped->m_pTargetedObject);
                    }
                    ped->m_pTargetedObject = nullptr;
                }
            }
        }
    }
    return abortable;
}

// 0x688810
bool CTaskSimplePlayerOnFoot::ProcessPed(CPed* ped) {
    // return plugin::CallMethodAndReturn<bool, 0x688810, CTaskSimplePlayerOnFoot*, CPed*>(this, ped);

    auto* player = static_cast<CPlayerPed*>(ped);

    if (player->GetPadFromPlayer()) {
        CPedIntelligence* intelligence = player->GetIntelligence();
        bool bPedMoving = player->m_nMoveState >= PEDMOVE_WALK;
        if (   player->GetActiveWeapon().m_nType == WEAPON_CHAINSAW
            && intelligence->GetTaskFighting()
            && intelligence->GetTaskFighting()->m_nCurrentMove == FIGHT_ATTACK_FIGHTIDLE
        ) {
            bPedMoving = true;
        }

        player->SetMoveState(PEDMOVE_STILL);

        if (player->bIsDucking) {
            PlayerControlDucked(player);
        } else if (!intelligence->GetTaskFighting() || bPedMoving) {
            CTaskSimpleUseGun* simpleTaskUseGun = intelligence->GetTaskUseGun();
            if (   simpleTaskUseGun
                && simpleTaskUseGun->m_pWeaponInfo
                && !simpleTaskUseGun->m_pWeaponInfo->flags.bAimWithArm
            ) {
                PlayerControlZeldaWeapon(player);
            } else {
                PlayerControlZelda(player, false);
            }
        } else {
            PlayerControlFighter(player);
        }

        ProcessPlayerWeapon(player);
    }

    m_nFrameCounter = CTimer::GetFrameCounter();
    return false;
}

// 0x6859A0
void CTaskSimplePlayerOnFoot::ProcessPlayerWeapon(CPlayerPed* player)
{
    return plugin::CallMethod<0x6859A0, CTaskSimplePlayerOnFoot*, CPlayerPed*>(this, player);

    /*
    CPlayerPedData* playerData = player->m_pPlayerData;
    CPedIntelligence* intelligence = player->GetIntelligence();
    CTaskManager* taskManager = &player->GetTaskManager();
    CPad* pad = player->GetPadFromPlayer();

    eWeaponType weaponType = player->GetActiveWeapon().m_nType;
    eWeaponSkill weaponSkill = player->GetWeaponSkill();
    CWeaponInfo* weaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, weaponSkill);

    if (playerData->m_bHaveTargetSelected && !player->m_pTargetedObject) {
        TheCamera.ClearPlayerWeaponMode();
        CWeaponEffects::ClearCrossHair(player->m_nPedType);
    }

    player->m_nWeaponAccuracy = weaponInfo->flags.bCanAim ? 95 : 100;

    if (    pad->WeaponJustDown(player)
        && (player->m_pTargetedObject || CCamera::m_bUseMouse3rdPerson && player->m_p3rdPersonMouseTarget)
    ) {
        player->PlayerHasJustAttackedSomeone();
    }
    
    if (player->m_pFire || !weaponInfo->flags.b1stPerson) {
        if (!pad->GetTarget() && weaponType == WEAPON_RLAUNCHER_HS) {
            playerData->m_nFireHSMissilePressedTime = 0;
            playerData->m_LastHSMissileTarget = nullptr;
        }
    } else {
        if (   pad->GetEnterTargeting()
            || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget
            || pad->GetTarget()
            && m_nFrameCounter < (CTimer::GetFrameCounter() - 1)
        ) {
            auto weaponMode = eCamMode::MODE_NONE;
            switch (weaponType) {
            case WEAPON_RLAUNCHER: {
                weaponMode = MODE_ROCKETLAUNCHER;
                break;
            }
            case WEAPON_RLAUNCHER_HS: {
                playerData->m_nFireHSMissilePressedTime = CTimer::GetTimeInMS();
                playerData->m_LastHSMissileTarget = nullptr;
                weaponMode = MODE_ROCKETLAUNCHER_HS;
                break;
            }
            case WEAPON_SNIPERRIFLE: {
                weaponMode = MODE_SNIPER;
                break;
            }
            case WEAPON_CAMERA: {
                weaponMode = MODE_CAMERA;
                break;
            }
            default: {
                weaponMode = MODE_M16_1STPERSON;
                break;
            }
            }
            TheCamera.SetNewPlayerWeaponMode(weaponMode, 0, 0);
            player->SetPedState(PEDSTATE_SNIPER_MODE);
            return;
        }

        if (!TheCamera.Using1stPersonWeaponMode()) {
            weaponType = player->GetActiveWeapon().m_nType;
            if (weaponType == WEAPON_RLAUNCHER || weaponType == WEAPON_RLAUNCHER_HS || weaponType == WEAPON_SNIPERRIFLE || weaponType == WEAPON_CAMERA) {
                CTaskSimpleUseGun* simpleTaskUseGun = intelligence->GetTaskUseGun();
                if (simpleTaskUseGun) {
                    simpleTaskUseGun->PlayerPassiveControlGun();
                }
                player->m_pPlayerData->m_bHaveTargetSelected = 0;
                return;
            }
        }
    }

    CTaskSimpleUseGun* newSimpleUseGunTask = nullptr;
    int32 gunCommand[4] = {0};

    if (weaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE) {
        int32 fightCommand = 0;
        gunCommand[0] = 0;

        if (!player->m_pTargetedObject && !pad->GetTarget() && !taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            if (pad->MeleeAttackJustDown(false)) {
                fightCommand = 11;
                gunCommand[0] = 11;
            }

            CAnimBlendAssociation* animAssoc = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_KILL_PARTIAL);
            if (animAssoc) {
                animAssoc->m_fBlendAmount = -2.0f;
            }

            if (fightCommand == 0) {
            HANDLE_FIGHTING_TASK_IF_EXISTS:
                if (intelligence->GetTaskFighting()) {
                    auto taskSimpleFight = static_cast<CTaskSimpleFight*>(taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
                    if (player->m_nMoveState == PEDMOVE_STILL && pad->GetSprint()) {
                        taskSimpleFight->ControlFight(player->m_pTargetedObject, 15);
                    } else {
                        if (playerData->m_nChosenWeapon == player->m_nActiveWeaponSlot) {
                            taskSimpleFight->ControlFight(player->m_pTargetedObject, 0);
                        } else {
                            taskSimpleFight->ControlFight(player->m_pTargetedObject, 1);
                        }
                    }
                }
                goto PED_WEAPON_AIMING_CODE;
            }

            // fightCommand cannot be 19 here, so we don't need the code here.
            if (fightCommand == 19) {
                // Just in case, if this executes somehow, then we probably need to add the code.
                // But It won't.
                assert(fightCommand != 19);
                // LAB_00685c62:
            }

            goto EXECUTE_MELEE_ATTACK;
        } else {
            CPed* targetEntity = nullptr;
            if (!player->m_pTargetedObject) {
                if (CCamera::m_bUseMouse3rdPerson && player->m_p3rdPersonMouseTarget) {
                    targetEntity = player->m_p3rdPersonMouseTarget;
                }
            } else {
                if (player->m_pTargetedObject->IsPed()) {
                    targetEntity = reinterpret_cast<CPed*>(player->m_pTargetedObject);
                }
            }

            CAnimBlendAssociation* animAssociation = nullptr;
            int32 animGroupID = weaponInfo->m_eAnimGroup;
            if (   targetEntity
                && pad->GetTarget()
                && playerData->m_fMoveBlendRatio < 1.9f
                && player->m_nMoveState != PEDMOVE_SPRINT
                && !taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)
                && animGroupID != ANIM_GROUP_DEFAULT
                && CAnimManager::GetAnimationBlock(animGroupID)
                && CAnimManager::GetAnimationBlock(animGroupID)->bLoaded
                && intelligence->TestForStealthKill(targetEntity, false)
            ) {
                if (player->bIsDucking) {
                    CTaskSimpleDuck* duckTask = intelligence->GetTaskDuck(true);
                    if (duckTask && duckTask->IsTaskInUseByOtherTasks()) {
                        animAssociation = RpAnimBlendClumpGetFirstAssociation(player->m_pRwClump);
                    } else {
                        animAssociation = CAnimManager::BlendAnimation(player->m_pRwClump, weaponInfo->m_eAnimGroup, ANIM_ID_KILL_PARTIAL, 8.0f);
                    }
                } else {
                    animAssociation = CAnimManager::BlendAnimation(player->m_pRwClump, weaponInfo->m_eAnimGroup, ANIM_ID_KILL_PARTIAL, 8.0f);
                }
            } else {
                animAssociation = RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_KILL_PARTIAL);
                if (animAssociation) {
                    animAssociation->m_fBlendAmount = -2.0f;
                }
            }

            bool bCheckButtonCircleStateOnly = false;
            if (player->m_pTargetedObject || pad->GetTarget()) {
                bCheckButtonCircleStateOnly = true;
            }
            uint8 meleeAttackJustDown = pad->MeleeAttackJustDown(bCheckButtonCircleStateOnly);
            if (meleeAttackJustDown && animAssociation && animAssociation->m_fBlendAmount > 0.5f && targetEntity && intelligence->TestForStealthKill(targetEntity, true)) {
                auto* pTaskSimpleStealthKill = new CTaskSimpleStealthKill(true, targetEntity, weaponInfo->m_eAnimGroup);
                taskManager->SetTask(pTaskSimpleStealthKill, 3, false);

                eWeaponType activeWeaponType = player->GetActiveWeapon().m_nType;
                CPedDamageResponseCalculator damageCalculator(player, 0.0f, activeWeaponType, PED_PIECE_TORSO, false);
                CEventDamage eventDamage(player, CTimer::GetTimeInMS(), activeWeaponType, PED_PIECE_TORSO, 0, false, targetEntity->bInVehicle);
                if (eventDamage.AffectsPed(targetEntity)) {
                    damageCalculator.ComputeDamageResponse(targetEntity, &eventDamage.m_damageResponse, false);
                    targetEntity->GetEventGroup().Add(&eventDamage, false);
                    CCrime::ReportCrime(eCrimeType::CRIME_SEALTH_KILL_PED_WITH_KNIFE, targetEntity, player);
                    player->m_weaponAudio.AddAudioEvent(AE_WEAPON_STEALTH_KILL);
                }
                player->ClearWeaponTarget();
            } else {
                switch (meleeAttackJustDown) {
                case 1: {
                    fightCommand = 11;
                    gunCommand[0] = fightCommand;
                    break;
                }
                case 4: {
                    if (!CWeaponInfo::GetWeaponInfo(player->GetActiveWeapon().m_nType, eWeaponSkill::WEAPSKILL_STD)->flags.bHeavy) {
                        fightCommand = 12;
                        gunCommand[0] = 12;
                    } else {
                        fightCommand = 11;
                        gunCommand[0] = fightCommand;
                    }
                    break;
                }
                case 3: {
                    fightCommand = 2;
                    gunCommand[0] = fightCommand;
                    break;
                }
                default: {
                    if (   pad->GetMeleeAttack(false)
                        && player->GetActiveWeapon().m_nType == WEAPON_CHAINSAW
                        && taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)
                    ) {
                        fightCommand = 11;
                        gunCommand[0] = fightCommand;
                    } else {
                        goto HANDLE_FIGHTING_TASK_IF_EXISTS;
                    }
                }
                };

            EXECUTE_MELEE_ATTACK:
                if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
                    if (intelligence->GetTaskFighting()) {
                        auto* taskSimpleFight = static_cast<CTaskSimpleFight*>(taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
                        taskSimpleFight->ControlFight(player->m_pTargetedObject, gunCommand[0]);
                    }
                } else {
                    auto* taskSimpleFight = new CTaskSimpleFight(player->m_pTargetedObject, fightCommand, 2000u);
                    taskManager->SetTaskSecondary(taskSimpleFight, TASK_SECONDARY_ATTACK);
                }
            }
        }
    } else {
        if (weaponInfo->m_nWeaponFire == WEAPON_FIRE_USE) {
            if (pad->WeaponJustDown(nullptr)) {
                uint8 activeWeaponSlot = player->m_nActiveWeaponSlot;
                weaponType = player->m_aWeapons[activeWeaponSlot].m_nType;
                CWeapon* playerWeapon = &player->m_aWeapons[activeWeaponSlot];
                if (weaponType == WEAPON_DETONATOR) {
                    playerWeapon->Fire(player, &player->GetPosition(), &player->GetPosition(), nullptr, nullptr, nullptr);
                } else if (weaponType > WEAPON_CAMERA && weaponType <= WEAPON_INFRARED && !taskManager->GetTaskPrimary(TASK_PRIMARY_PRIMARY)) {
                    auto* pCTaskComplexUseGoggles = new CTaskComplexUseGoggles();
                    taskManager->SetTask(pCTaskComplexUseGoggles, 3, false);
                    player->m_pPlayerData->m_bDontAllowWeaponChange = true;
                }
            }
        } else {
            if (!pad->GetWeapon(player)) {
                if (intelligence->GetTaskThrow()) {
                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK));
                    pTaskSimpleThrowProjectile->ControlThrow(true, nullptr, nullptr);
                }
            } else {
                uint32 nWeaponFire = weaponInfo->m_nWeaponFire;
                if (player->m_nMoveState != PEDMOVE_SPRINT && playerData->m_nChosenWeapon == player->m_nActiveWeaponSlot && (nWeaponFire - 1) < 4) {
                    switch (nWeaponFire) {
                    case WEAPON_FIRE_INSTANT_HIT:
                    case WEAPON_FIRE_AREA_EFFECT: {
                        CEntity* targetedObject = player->m_pTargetedObject;
                        gunCommand[0] = 2;
                        if (CTaskSimpleUseGun::RequirePistolWhip(player, targetedObject)) {
                            gunCommand[0] = 5;
                        } else if (player->GetActiveWeapon().m_nState == 2) {
                            if (!pad->GetTarget() && !targetedObject && !playerData->m_bFreeAiming) {
                                break;
                            }
                            gunCommand[0] = 1;
                        }
                        CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                        if (secondaryTask) {
                            if (secondaryTask->GetTaskType() == TASK_SIMPLE_USE_GUN) {
                                CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                                if (taskUseGun) {
                                    taskUseGun->ControlGun(player, targetedObject, gunCommand[0]);
                                }
                            } else {
                                secondaryTask->MakeAbortable(player, ABORT_PRIORITY_URGENT, nullptr);
                            }
                        } else {
                            auto* taskUseGun = new CTaskSimpleUseGun(targetedObject, CVector(0.0f, 0.0f, 0.f), gunCommand[0], 1, false);
                            taskManager->SetTaskSecondary(taskUseGun, 0);
                            player->m_pPlayerData->m_fAttackButtonCounter = 0;
                        }
                        if (!pad->GetTarget()) {
                            if (player->GetActiveWeapon().m_nType == WEAPON_EXTINGUISHER) {
                                playerData->m_fLookPitch = -CWeapon::ms_fExtinguisherAimAngle;
                            } else {
                                playerData->m_fLookPitch = 0.0f;
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_PROJECTILE: {
                        uint8 activeWeaponSlot = player->m_nActiveWeaponSlot;
                        CWeapon* activeWeapon = &player->m_aWeapons[activeWeaponSlot];
                        if (activeWeapon->m_nType == WEAPON_RLAUNCHER || activeWeapon->m_nType == WEAPON_RLAUNCHER_HS) {
                            gunCommand[0] = 2;
                            if (activeWeapon->m_nState == WEAPONSTATE_RELOADING) {
                                gunCommand[0] = 1;
                            }
                            CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                            if (secondaryTask) {
                                if (secondaryTask->GetTaskType() == TASK_SIMPLE_USE_GUN) {
                                    if (intelligence->GetTaskUseGun()) {
                                        auto* taskUseGun = static_cast<CTaskSimpleUseGun*>(secondaryTask);
                                        taskUseGun->ControlGun(player, player->m_pTargetedObject, gunCommand[0]);
                                    }
                                } else {
                                    taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)->MakeAbortable(player, ABORT_PRIORITY_URGENT, nullptr);
                                }
                            } else {
                                auto* taskUseGun = new CTaskSimpleUseGun(player->m_pTargetedObject, CVector(0.0f, 0.0f, 0.0f), gunCommand[0], 1, false);
                                taskManager->SetTaskSecondary(taskUseGun, 0);
                            }
                        } else {
                            if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK) || !pad->WeaponJustDown(player)) {
                                CTask* secondaryTask = taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
                                if (secondaryTask && secondaryTask->GetTaskType() != TASK_SIMPLE_THROW) {
                                    secondaryTask->MakeAbortable(player, ABORT_PRIORITY_URGENT, nullptr);
                                } else if (intelligence->GetTaskThrow()) {
                                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(secondaryTask);
                                    pTaskSimpleThrowProjectile->ControlThrow(pad->WeaponJustDown(player), nullptr, nullptr);
                                }
                            } else {
                                auto* pTaskSimpleThrowProjectile = new CTaskSimpleThrowProjectile(nullptr, CVector(0.0f, 0.0f, 0.0f));
                                taskManager->SetTaskSecondary(pTaskSimpleThrowProjectile, 0);
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_CAMERA: {
                        uint8 activeWeaponSlot = player->m_nActiveWeaponSlot;
                        CWeapon* activeWeapon = &player->m_aWeapons[activeWeaponSlot];
                        if (CCamera::GetActiveCamera().m_nMode == MODE_CAMERA && CTimer::GetTimeInMS() > activeWeapon->m_nTimeForNextShot) {
                            CVector firingPoint(0.0f, 0.0f, 0.6f);
                            CVector outputFiringPoint = *player->m_matrix * firingPoint;
                            activeWeapon->Fire(player, &outputFiringPoint, nullptr, nullptr, nullptr, nullptr);
                        }
                        break;
                    }
                    };
                }
            }
        }
    }

PED_WEAPON_AIMING_CODE:

    CVector firingPoint(0.0f, 0.0f, 0.0f);
    CVector upVector(0.0f, 0.0f, 0.0f);

    if (player->GetActiveWeapon().m_nState == WEAPONSTATE_RELOADING && weaponInfo->flags.bReload) {
        if (!intelligence->GetTaskUseGun()) {
            AssocGroupId animGroupId = weaponInfo->m_eAnimGroup;
            AnimationId crouchReloadAnimID = weaponInfo->flags.bReload ? ANIM_ID_RELOAD : ANIM_ID_WALK;
            if (!player->bIsDucking) {
                if (!RpAnimBlendClumpGetAssociation(player->m_pRwClump, crouchReloadAnimID)) {
                    CAnimManager::BlendAnimation(player->m_pRwClump, animGroupId, crouchReloadAnimID, 4.0f);
                }
            } else {
                auto duckTask = (CTaskSimpleDuck*)intelligence->GetTaskDuck(true);
                if (weaponInfo->GetCrouchReloadAnimationID() && gbUnknown_8D2FE8 && duckTask) {
                    if (!duckTask->IsTaskInUseByOtherTasks()) {
                        crouchReloadAnimID = weaponInfo->GetCrouchReloadAnimationID();
                        if (!RpAnimBlendClumpGetAssociation(player->m_pRwClump, crouchReloadAnimID)) {
                            crouchReloadAnimID = weaponInfo->GetCrouchReloadAnimationID();
                            CAnimManager::BlendAnimation(player->m_pRwClump, animGroupId, crouchReloadAnimID, 4.0f);
                        }
                    }
                }
            }
        } else {
            auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
            taskUseGun->ControlGun(player, player->m_pTargetedObject, 4);
        }
    }

    int32 fightCommand = 0;
    if (!pad->GetTarget() || player->m_pPlayerData->m_nChosenWeapon != player->m_nActiveWeaponSlot || player->m_nMoveState == PEDMOVE_SPRINT && weaponInfo->m_nWeaponFire || TheCamera.Using1stPersonWeaponMode()) {
        if (!pad->GetTarget() && !player->m_pAttachedTo || player->m_pPlayerData->m_nChosenWeapon != player->m_nActiveWeaponSlot || player->m_nMoveState == PEDMOVE_SPRINT ||
            !TheCamera.Using1stPersonWeaponMode()) {
            if ((player->m_pTargetedObject || player->m_pPlayerData->m_bFreeAiming) && intelligence->GetTaskFighting()) {
                if (playerData->m_vecFightMovement.y >= -0.5) {
                    fightCommand = 15;
                } else {
                    fightCommand = 16;
                }
                CTaskSimpleFight* taskSimpleFight = intelligence->GetTaskFighting();
                taskSimpleFight->ControlFight(nullptr, fightCommand);
            }
            if (intelligence->GetTaskUseGun()) {
                if (    pad->GetWeapon(nullptr)
                    || (float)pad->GetPedWalkUpDown() <= 50.0f
                    && (float)pad->GetPedWalkUpDown() >= -50.0f
                    && (float)pad->GetPedWalkLeftRight() <= 50.0f
                    && (float)pad->GetPedWalkLeftRight() >= -50.0f
                ) {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->PlayerPassiveControlGun();
                } else {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->ControlGun(player, player->m_pTargetedObject, 7);
                }

                CCam* camera = &CCamera::GetActiveCamera();
                if (!weaponInfo->flags.bAimWithArm && camera->m_nMode == MODE_FOLLOWPED) {
                    player->m_fAimingRotation = atan2(-camera->m_vecFront.x, camera->m_vecFront.y);
                }
                if (player->m_pTargetedObject || playerData->m_bFreeAiming) {
                    CTaskSimpleUseGun* taskUseGun = intelligence->GetTaskUseGun();
                    taskUseGun->SkipAim(player);
                }
            }
            if (player->m_pTargetedObject) {
                player->ClearWeaponTarget();
            }
            player->Clear3rdPersonMouseTarget();
            playerData->m_bFreeAiming = 0;
            goto MAKE_PLAYER_LOOK_AT_ENTITY;
        }
        if (!taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
            newSimpleUseGunTask = new CTaskSimpleUseGun(player->m_pTargetedObject, CVector(0.0f, 0.0f, 0.0f), 1, 1, false);
            firingPoint.Set(0.0f, 0.0f, 0.0f);

            taskManager->SetTaskSecondary(newSimpleUseGunTask, 0);
            goto MAKE_PLAYER_LOOK_AT_ENTITY;
        }
        if (intelligence->GetTaskUseGun()) {
            auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
            taskUseGun->ControlGun(player, player->m_pTargetedObject, 1);
        }
        goto MAKE_PLAYER_LOOK_AT_ENTITY;
    }
    if (!weaponInfo->flags.bCanAim || playerData->m_bFreeAiming) {
        if (playerData->m_bFreeAiming && weaponInfo->flags.bCanAim && (pad->ShiftTargetLeftJustDown() || pad->ShiftTargetRightJustDown()) && !CCamera::m_bUseMouse3rdPerson) {
            if (player->m_pTargetedObject) {
                if (pad->ShiftTargetLeftJustDown()) {
                    player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, true);
                }
                if (pad->ShiftTargetRightJustDown()) {
                    player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, false);
                }
            } else {
                bool shiftTargetLeftJustDown = pad->ShiftTargetLeftJustDown();
                player->FindNextWeaponLockOnTarget(nullptr, shiftTargetLeftJustDown);
            }
        } else if (!weaponInfo->flags.bOnlyFreeAim || player->m_pTargetedObject || playerData->m_bFreeAiming) {
            if (CCamera::m_bUseMouse3rdPerson && playerData->m_bFreeAiming) {
                bool bWeaponIsNotMelee = true;
                if (!weaponInfo->m_nWeaponFire) {
                    bWeaponIsNotMelee = false;
                }
                player->Compute3rdPersonMouseTarget(bWeaponIsNotMelee);
            }
        } else {
            playerData->m_bFreeAiming = 1;
        }
    } else {
        if (player->m_pTargetedObject) {
            CPed* targetedEntity = (CPed*)player->m_pTargetedObject;
            CWeapon* activeWeapon = &player->GetActiveWeapon();
            auto weaponSkill = eWeaponSkill::WEAPSKILL_POOR;
            int32 pedState = 0;
            if (
                (fabs((float)pad->AimWeaponLeftRight(player)) > 100.0f || fabs((float)pad->AimWeaponUpDown(player)) > 100.0f)
                && !CGameLogic::IsCoopGameGoingOn()
                || targetedEntity == (CPed*)nullptr
                || CCamera::m_bUseMouse3rdPerson == true
                || targetedEntity
                && targetedEntity->IsPed()
                && (
                      !CPlayerPed::PedCanBeTargettedVehicleWise(targetedEntity)
                   || !CLocalisation::KickingWhenDown()
                   && ((pedState = targetedEntity->m_nPedState, pedState == PEDSTATE_DIE)
                   || pedState == PEDSTATE_DEAD)
                  )
                || player->DoesTargetHaveToBeBroken(player->m_pTargetedObject, activeWeapon)
                || !player->bCanPointGunAtTarget
                && (activeWeapon->m_nType, weaponSkill = player->GetWeaponSkill(), !(CWeaponInfo::GetWeaponInfo(activeWeapon->m_nType, weaponSkill)->flags.bCanAim))
            ) {
                player->ClearWeaponTarget();
                playerData->m_bFreeAiming = 1;
            }
            
            if (player->m_pTargetedObject) {
                if (pad->ShiftTargetLeftJustDown()) {
                    player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, true);
                }
                if (pad->ShiftTargetRightJustDown()) {
                    player->FindNextWeaponLockOnTarget(player->m_pTargetedObject, false);
                }
            }

            if (CWeaponInfo::GetWeaponInfo(activeWeapon->m_nType, eWeaponSkill::WEAPSKILL_STD)->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT) {
                targetedEntity = (CPed*)player->m_pTargetedObject;
                if (targetedEntity && targetedEntity->IsPed() && intelligence->IsInSeeingRange(player->GetPosition())) {
                    CTask* activePrimaryTask = intelligence->GetActivePrimaryTask();
                    if (!activePrimaryTask || activePrimaryTask->GetTaskType() != TASK_COMPLEX_REACT_TO_GUN_AIMED_AT) {
                        if (activeWeapon->m_nType != WEAPON_PISTOL_SILENCED) {
                            player->Say(176, 0, 1.0f, 0, 0, 0);
                        }
                        CPedGroup* pedGroup = CPedGroups::GetPedsGroup(targetedEntity);
                        if (pedGroup) {
                            if (!CPedGroups::AreInSameGroup(targetedEntity, player)) {
                                auto* eventGunAimedAt = new CEventGunAimedAt(player);
                                CEventGroupEvent eventGroupEvent(targetedEntity, eventGunAimedAt);
                                pedGroup->m_groupIntelligence.AddEvent(&eventGroupEvent);
                            }
                        } else {
                            CEventGunAimedAt eventGunAimedAt(player);
                            intelligence->m_eventGroup.Add(&eventGunAimedAt, false);
                        }
                    }
                }
            }
        } else if (CCamera::m_bUseMouse3rdPerson) {
            player->ClearWeaponTarget();
        } else if (pad->GetEnterTargeting() || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget || m_nFrameCounter < (uint32)(CTimer::GetFrameCounter() - 1)) {
            player->FindWeaponLockOnTarget();
        }
        if (!player->m_pTargetedObject) {
            playerData->m_bFreeAiming = 1;
        }
    }

    if (player->m_pTargetedObject) {
        if (weaponInfo->m_nWeaponFire || player->bIsDucking) {
            playerData->m_bFreeAiming = 0;
        }
    } else {
        uint32 nWeaponFire = weaponInfo->m_nWeaponFire;
        if (!nWeaponFire || nWeaponFire == WEAPON_FIRE_PROJECTILE || nWeaponFire == WEAPON_FIRE_USE) {
            goto MAKE_PLAYER_LOOK_AT_ENTITY;
        }
    }

    TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON, 0, 0);

    if (player->m_pTargetedObject) {
        TheCamera.UpdateAimingCoors(&player->m_pTargetedObject->GetPosition());
    } else {
        CMatrix* playerMatrix = player->m_matrix;
        firingPoint = playerMatrix->GetForward();

        firingPoint.x = firingPoint.x * 5.0f;
        firingPoint.y = firingPoint.y * 5.0f;
        firingPoint.z = (sin(player->m_pPlayerData->m_fLookPitch) + firingPoint.z) * 5.0f;

        firingPoint += player->GetPosition();
        TheCamera.UpdateAimingCoors(&firingPoint);
    }

    if (taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK)) {
        if (intelligence->GetTaskUseGun()) {
            auto taskUseGun = (CTaskSimpleUseGun*)taskManager->GetTaskSecondary(TASK_SECONDARY_ATTACK);
            taskUseGun->ControlGun(player, player->m_pTargetedObject, 1);
        }
    } else {
        newSimpleUseGunTask = new CTaskSimpleUseGun(player->m_pTargetedObject, CVector(0.0f, 0.0f, 0.0f), 1, 1, false);
        firingPoint.Set(0.0f, 0.0f, 0.0f);
        taskManager->SetTaskSecondary(newSimpleUseGunTask, 0);
    }

MAKE_PLAYER_LOOK_AT_ENTITY:

    playerData->m_bHaveTargetSelected = player->m_pTargetedObject ? true : false;
    CPed* targetedObject = player->m_pTargetedObject->AsPed();
    const auto AbortLookingIfPossible = [&]() {
        if (m_pLookingAtEntity && g_ikChainMan.IsLooking(player) && g_ikChainMan.GetLookAtEntity(player) == m_pLookingAtEntity) {
            g_ikChainMan.AbortLookAt(player, 250);
        }
        m_pLookingAtEntity = targetedObject;
    };

    if (!targetedObject) {
        return AbortLookingIfPossible();
    }

    bool bTargetedPedDead = false;
    if (targetedObject->IsPed() && (targetedObject->bFallenDown || targetedObject->m_nPedState == PEDSTATE_DEAD)) {
        bTargetedPedDead = true;
    }

    if (
           (weaponInfo->m_nWeaponFire || intelligence->GetTaskFighting() && !bTargetedPedDead)
        && (player->bIsDucking || !weaponInfo->flags.bAimWithArm)
    ) {
        return AbortLookingIfPossible();
    }

    CVector distance = targetedObject->GetPosition() - player->GetPosition();
    if (DotProduct(distance, player->GetForwardVector()) <= 0.0f) {
        return AbortLookingIfPossible();
    } else if (!g_ikChainMan.IsLooking(player) || (g_ikChainMan.GetLookAtEntity(player) != (CEntity*)targetedObject)) {
        auto pedBoneId = BONE_UNKNOWN;
        if (targetedObject->IsPed()) {
            pedBoneId = BONE_HEAD;
        }
        g_ikChainMan.LookAt("ProcPlyrWeapon", player, targetedObject, gDefaultTaskTime, pedBoneId, nullptr, false, 0.25f, 500, 3, false);
    }

    m_pLookingAtEntity = targetedObject;
    */
}

// 0x6872C0
void CTaskSimplePlayerOnFoot::PlayIdleAnimations(CPlayerPed* player) {
    // return plugin::CallMethod<0x6872C0, CTaskSimplePlayerOnFoot*, CPlayerPed*>(this, player);

    if (CGameLogic::IsCoopGameGoingOn())
        return;

    CPad* pad = player->GetPadFromPlayer();
    AssocGroupId animGroupID = ANIM_GROUP_DEFAULT;
    if (TheCamera.m_bWideScreenOn
        || player->bIsDucking
        || player->bCrouchWhenShooting
        || player->GetIntelligence()->GetTaskHold(false)
        || pad->DisablePlayerControls
        || player->m_nMoveState > PEDMOVE_STILL
        || (animGroupID = player->m_nAnimGroup, animGroupID != ANIM_GROUP_PLAYER) && animGroupID != ANIM_GROUP_FAT && animGroupID != ANIM_GROUP_MUSCULAR
    ) {
        pad->SetTouched();
    }

    CAnimBlock* animBlock = &CAnimManager::ms_aAnimBlocks[m_nAnimationBlockIndex];
    uint32      touchTimeDelta = pad->GetTouchedTimeDelta();
    if (touchTimeDelta <= 10000) {
        if (animBlock->bLoaded) {
            CStreaming::SetModelIsDeletable(IFPToModelId(m_nAnimationBlockIndex));
            CAnimBlendAssociation* animAssoc = nullptr;
            for (animAssoc = RpAnimBlendClumpGetFirstAssociation(player->m_pRwClump); animAssoc; animAssoc = RpAnimBlendGetNextAssociation(animAssoc)) {
                if (animAssoc->m_nFlags & ANIM_FLAG_200) {
                    animAssoc->m_fBlendDelta = -8.0f;
                }
            }
        }
        gLastTouchTimeDelta = 0;
        return;
    }

    CStreaming::RequestModel(IFPToModelId(m_nAnimationBlockIndex), STREAMING_GAME_REQUIRED);
    if (!animBlock->bLoaded)
        return;

    const auto PlayRandomIdleAnim = [&]() {
        if (!(player->bIsLooking && player->bIsRestoringLook) && touchTimeDelta - gLastTouchTimeDelta > 20000) {
            // Play random idle animation
            int32 randomNumber = 0;
            do {
                randomNumber = CGeneral::GetRandomNumberInRange(0, 4);
            } while (gLastRandomNumberForIdleAnimationID == randomNumber);

            constexpr struct {
                AnimationId  animId;
                AssocGroupId assoc;
            } animations[] = {
                { ANIM_ID_STRETCH, ANIM_GROUP_PLAYIDLES },
                { ANIM_ID_TIME,    ANIM_GROUP_PLAYIDLES },
                { ANIM_ID_SHLDR,   ANIM_GROUP_PLAYIDLES },
                { ANIM_ID_STRLEG,  ANIM_GROUP_PLAYIDLES },
            };
            auto                   animation = animations[randomNumber];
            CAnimBlendAssociation* animNewAssoc = CAnimManager::BlendAnimation(player->m_pRwClump, animation.assoc, animation.animId, 8.0f);
            animNewAssoc->m_nFlags |= ANIM_FLAG_200;
            gLastTouchTimeDelta = touchTimeDelta;
            gLastRandomNumberForIdleAnimationID = randomNumber;
            if (CStats::GetStatValue(STAT_MANAGEMENT_ISSUES_MISSION_ACCOMPLISHED) != 0.0f && CTimer::GetTimeInMS() > 1200000) {
                player->Say(336, 0, 0.2f, 0, 0, 0);
            }
        }
    };

    CAnimBlendAssociation* animAssoc1 = RpAnimBlendClumpGetFirstAssociation(player->m_pRwClump);
    if (animAssoc1) {
        while (true) {
            uint32 animHierarchyIndex = (uint32)animAssoc1->m_pHierarchy - (uint32)CAnimManager::ms_aAnimations;
            animHierarchyIndex = animHierarchyIndex / 6 + (animHierarchyIndex >> 0x1f) >> 2;
            animHierarchyIndex = animHierarchyIndex + (animHierarchyIndex >> 0x1f);

            int32 animBlockFirstAnimIndex = animBlock->startAnimation;
            if (animHierarchyIndex >= animBlockFirstAnimIndex && animHierarchyIndex < animBlockFirstAnimIndex + animBlock->animationCount) {
                break;
            }
            animAssoc1 = RpAnimBlendGetNextAssociation(animAssoc1);
            if (!animAssoc1) {
                return PlayRandomIdleAnim();
            }
        }
    } else {
        PlayRandomIdleAnim();
    }
}

// 0x687530
void CTaskSimplePlayerOnFoot::PlayerControlFighter(CPlayerPed* player) {
    plugin::CallMethod<0x687530, CTaskSimplePlayerOnFoot*, CPlayerPed*>(this, player);
}

// 0x687C20
void CTaskSimplePlayerOnFoot::PlayerControlZeldaWeapon(CPlayerPed* player) {
    // return plugin::Call<0x687C20, CPlayerPed*>(player);

    CTaskSimpleUseGun* taskUseGun = player->GetIntelligence()->GetTaskUseGun();
    if (!taskUseGun)
        return;

    if (player->m_pAttachedTo != nullptr)
        return;

    CVector2D moveSpeed(0.0f, 0.0f);
    CPad*     pad = player->GetPadFromPlayer();
    float     pedWalkUpDown = moveSpeed.y;
    float     pedWalkLeftRight = moveSpeed.x;
    if (!taskUseGun->m_pWeaponInfo->flags.b1stPerson || CGameLogic::IsPlayerUse2PlayerControls(player)) {
        pedWalkUpDown = pad->GetPedWalkUpDown();
        pedWalkLeftRight = pad->GetPedWalkLeftRight();
    } else if (TheCamera.Using1stPersonWeaponMode()) {
        pedWalkUpDown = pad->GetPedWalkUpDown(player);
        pedWalkLeftRight = pad->GetPedWalkLeftRight(player);
    }

    moveSpeed.x = pedWalkLeftRight / 128.0f;
    moveSpeed.y = pedWalkUpDown / 128.0f;

    if (CGameLogic::IsPlayerUse2PlayerControls(player)) {
        float moveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
        CEntity* targetedObject = player->m_pTargetedObject;

        if (moveBlendRatio > 0.0f) {
            float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            float negativeSinRadian = -sin(limitedRadianAngle);
            float cosRadian = cos(limitedRadianAngle);
            if (targetedObject) {
                if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, negativeSinRadian, cosRadian, 0.0f, 0.0f)) {
                    moveBlendRatio = 0.0f;
                }
                CMatrixLink* pedMatrix = player->m_matrix;
                moveSpeed.x = (cosRadian * pedMatrix->GetRight().y + negativeSinRadian * pedMatrix->GetRight().x + pedMatrix->GetRight().z * 0.0f) * moveBlendRatio;
                moveSpeed.y = -((cosRadian * pedMatrix->GetForward().y + negativeSinRadian * pedMatrix->GetForward().x + pedMatrix->GetForward().z * 0.0f) * moveBlendRatio);
            } else {
                player->m_fAimingRotation = limitedRadianAngle;
                float moveSpeedY = 0.0f;
                if (CGameLogic::IsPlayerAllowedToGoInThisDirection(player, negativeSinRadian, cosRadian, 0.0f, 0.0f)) {
                    moveSpeedY = moveBlendRatio;
                }
                moveSpeed.x = 0.0f;
                moveSpeed.y = -moveSpeedY;
            }
        }

        if (targetedObject) {
            CVector2D distance = targetedObject->GetPosition() - player->GetPosition();
            player->m_fAimingRotation = atan2(-distance.x, distance.y);
        }
    }

    float moveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
    if (moveBlendRatio > 1.0) {
        float moveSpeedMultiplier = 1.0f / moveBlendRatio;
        moveSpeed.x = moveSpeed.x * moveSpeedMultiplier;
        moveSpeed.y = moveSpeedMultiplier * moveSpeed.y;
    }

    taskUseGun->ControlGunMove(&moveSpeed);
    if (pad->DuckJustDown()) {
        if (CTaskSimpleDuck::CanPedDuck(player)) {
            player->GetIntelligence()->SetTaskDuckSecondary(0);
        }
    }
}

// 0x687F30
void CTaskSimplePlayerOnFoot::PlayerControlDucked(CPlayerPed* player) {
    // return plugin::Call<0x687F30, CPlayerPed*>(player);

    auto* duckTask = static_cast<CTaskSimpleDuck*>(player->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_DUCK));
    if (!duckTask)
        return;

    if (duckTask->m_bIsFinished || duckTask->m_bIsAborting)
        return;

    CPad* pad = player->GetPadFromPlayer();

    CVector2D moveSpeed;
    moveSpeed.x = pad->GetPedWalkLeftRight() / 128.0f;
    moveSpeed.y = pad->GetPedWalkUpDown() / 128.0f;

    float pedMoveBlendRatio = sqrt(moveSpeed.x * moveSpeed.x + moveSpeed.y * moveSpeed.y);
    if (player->m_pAttachedTo) {
        pedMoveBlendRatio = 0.0f;
    } else if (pedMoveBlendRatio > 1.0f) {
        pedMoveBlendRatio = 1.0f;
    }

    if (pad->DuckJustDown() || pad->GetSprint() || pad->JumpJustDown() || pad->ExitVehicleJustDown() || !CTaskSimpleDuck::CanPedDuck(player)) {
        player->GetIntelligence()->ClearTaskDuckSecondary();
        auto useGunTask = player->GetIntelligence()->GetTaskUseGun();
        if (!useGunTask || useGunTask->m_pWeaponInfo->flags.bAimWithArm) {
            int32 pedMoveState = PEDMOVE_NONE;
            if (pad->GetSprint()) {
                if (pedMoveBlendRatio <= 0.5f) {
                    return;
                }
                auto pNewAnimation = CAnimManager::BlendAnimation(player->m_pRwClump, player->m_nAnimGroup, ANIM_ID_RUN, gDuckAnimBlendData);
                pNewAnimation->m_nFlags |= ANIM_FLAG_STARTED;
                player->m_pPlayerData->m_fMoveBlendRatio = 1.5f;
                pedMoveState = PEDMOVE_RUN;
            } else {
                if (pedMoveBlendRatio <= 0.5f) {
                    return;
                }
                auto pNewAnimation = CAnimManager::BlendAnimation(player->m_pRwClump, player->m_nAnimGroup, ANIM_ID_WALK, gDuckAnimBlendData);
                pNewAnimation->m_nFlags |= ANIM_FLAG_STARTED;
                player->m_pPlayerData->m_fMoveBlendRatio = 1.5f;
                pedMoveState = PEDMOVE_WALK;
            }
            player->m_nMoveState = pedMoveState;
            player->m_nSwimmingMoveState = pedMoveState;
        } else if (pedMoveBlendRatio > 0.5f) {
            auto pNewAnimation = CAnimManager::BlendAnimation(player->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_GUNMOVE_FWD, gDuckAnimBlendData);
            pNewAnimation->m_nFlags |= ANIM_FLAG_STARTED;
            player->m_pPlayerData->m_fMoveBlendRatio = 1.0f;
            moveSpeed.x = 1.0f;
            moveSpeed.y = 0.0f;
            CTaskSimpleUseGun* pTaskSimpleUseGun = player->GetIntelligence()->GetTaskUseGun();
            pTaskSimpleUseGun->ControlGunMove(&moveSpeed);
        }
    } else if (!pad->GetTarget() || player->GetActiveWeapon().IsTypeMelee()) {
        if (pedMoveBlendRatio > 0.0f) {
            float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            player->m_fAimingRotation = limitedRadianAngle;
            CVector moveDirection(0.0f, -sin(limitedRadianAngle), cos(limitedRadianAngle));
            if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, moveDirection.x, moveDirection.y, 0.0f, 0.0f)) {
                pedMoveBlendRatio = 0.0f;
            }
        }
        moveSpeed.y = -pedMoveBlendRatio;

        player->m_pPlayerData->m_fMoveBlendRatio = pedMoveBlendRatio;
        moveSpeed.x = 0.0f;
        duckTask->ControlDuckMove(0.0f, moveSpeed.y);
    } else {
        if (CGameLogic::IsPlayerUse2PlayerControls(player)) {
            float   radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -moveSpeed.x, moveSpeed.y) - TheCamera.m_fOrientation;
            float   limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);
            CVector moveDirection(0.0f, -sin(limitedRadianAngle), cos(limitedRadianAngle));
            if (!CGameLogic::IsPlayerAllowedToGoInThisDirection(player, moveDirection.x, moveDirection.y, 0.0f, 0.0f)) {
                pedMoveBlendRatio = 0.0f;
            }
            CMatrix* matrix = player->m_matrix;
            CEntity* targetedObject = player->m_pTargetedObject;
            moveSpeed.x = (moveDirection.y * matrix->GetRight().y + moveDirection.x * matrix->GetRight().x + matrix->GetRight().z * 0.0f) * pedMoveBlendRatio;
            moveSpeed.y = -((moveDirection.y * matrix->GetForward().y + matrix->GetForward().z * 0.0f + moveDirection.x * matrix->GetForward().x) * pedMoveBlendRatio);
            if (targetedObject) {
                CVector distance = targetedObject->GetPosition() - player->GetPosition();
                player->m_fAimingRotation = atan2(-distance.x, distance.y);
            } else {
                player->m_fAimingRotation = limitedRadianAngle;
            }
        }
        duckTask->ControlDuckMove(moveSpeed.x, moveSpeed.y);
        player->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
    }
}

// 0x6883D0
int32 CTaskSimplePlayerOnFoot::PlayerControlZelda(CPlayerPed* player, bool bAvoidJumpingAndDucking)
{
    // return plugin::CallMethodAndReturn<int32, 0x6883D0, CTaskSimplePlayerOnFoot*, CPlayerPed*, bool>(this, player, bAvoidJumpingAndDucking);

    CPlayerPedData * playerData = player->m_pPlayerData;
    playerData->m_vecFightMovement = CVector2D();

    CPad* pad = player->GetPadFromPlayer();
    float pedWalkLeftRight = pad->GetPedWalkLeftRight();
    float pedWalkUpDown = pad->GetPedWalkUpDown();
    float pedMoveBlendRatio = sqrt(pedWalkUpDown * pedWalkUpDown + pedWalkLeftRight * pedWalkLeftRight) * (1.0f / 60.0f);
    if (player->m_pAttachedTo) {
        pedMoveBlendRatio = 0.0f;
    }

    const float radianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, -pedWalkLeftRight, pedWalkUpDown) - TheCamera.m_fOrientation;
    const float limitedRadianAngle = CGeneral::LimitRadianAngle(radianAngle);

    if (pad->NewState.m_bPedWalk && pedMoveBlendRatio > 1.0f) {
        pedMoveBlendRatio = 1.0f;
    } else if (pedMoveBlendRatio <= 0.0f) {
        player->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
        goto DONT_MODIFY_MOVE_BLEND_RATIO;
    }

    player->m_fAimingRotation = limitedRadianAngle;

    if (CGameLogic::IsPlayerAllowedToGoInThisDirection(player, -sin(limitedRadianAngle), cos(limitedRadianAngle), 0.0f, 0.0f)) {
        float fMaximumMoveBlendRatio = CTimer::GetTimeStep() * 0.07f;
        if (pedMoveBlendRatio - playerData->m_fMoveBlendRatio <= fMaximumMoveBlendRatio) {
            if (-fMaximumMoveBlendRatio <= pedMoveBlendRatio - playerData->m_fMoveBlendRatio)
                playerData->m_fMoveBlendRatio = pedMoveBlendRatio;
            else
                playerData->m_fMoveBlendRatio -= fMaximumMoveBlendRatio;
        } else {
            playerData->m_fMoveBlendRatio = fMaximumMoveBlendRatio + playerData->m_fMoveBlendRatio;
        }
    } else {
        player->m_pPlayerData->m_fMoveBlendRatio = 0.0f;
    }

DONT_MODIFY_MOVE_BLEND_RATIO:
    if (!(CWeaponInfo::GetWeaponInfo(player->GetActiveWeapon().m_nType, eWeaponSkill::STD)->flags.bHeavy)) {
        if (!player->m_standingOnEntity || !player->m_standingOnEntity->m_bIsStatic || player->m_standingOnEntity->m_bHasContacted) {
            if (!player->GetIntelligence()->GetTaskHold(false) || !((CTaskSimpleHoldEntity*)player->GetIntelligence()->GetTaskHold(false))->m_pAnimBlendAssociation) {
                CAnimBlendHierarchy*  animHierarchy = nullptr;
                CAnimBlendAssocGroup* animGroup = &CAnimManager::ms_aAnimAssocGroups[player->m_nAnimGroup];

                if (player->m_pPlayerData->m_bPlayerSprintDisabled || g_surfaceInfos->CantSprintOn(player->m_nContactSurface) ||
                    (animHierarchy = animGroup->GetAnimation(ANIM_ID_RUN)->m_pHierarchy, animHierarchy == animGroup->GetAnimation(ANIM_ID_SPRINT)->m_pHierarchy)) {
                    if (pad->GetSprint()) {
                        player->m_nMoveState = PEDMOVE_RUN;
                    }
                } else if (player->ControlButtonSprint(SPRINT_GROUND) >= 1.0f) {
                    player->m_nMoveState = PEDMOVE_SPRINT;
                }
            }
        }
    }

    player->SetRealMoveAnim();
    // What is the point of calling RpAnimBlendClumpGetAssociation here?
    // Nvm, let's keep it.
    RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_IDLE_ARMED);
    RpAnimBlendClumpGetAssociation(player->m_pRwClump, ANIM_ID_IDLE_CHAT);

    if (bAvoidJumpingAndDucking) {
        PlayIdleAnimations(player);
        player->m_pedIK.bSlopePitch = true;
        return player->m_pedIK.m_nFlags;
    }

    if (pad->DuckJustDown() && CTaskSimpleDuck::CanPedDuck(player)) {
        player->GetIntelligence()->SetTaskDuckSecondary(0);
    }

    if (
        !player->bIsInTheAir
        && !(CWeaponInfo::GetWeaponInfo(player->GetActiveWeapon().m_nType, eWeaponSkill::STD)->flags.bHeavy)
        && pad->JumpJustDown()
        && !pad->GetTarget()
        && !player->m_pAttachedTo
    ) {
        eCamMode cameraMode = CCamera::GetActiveCamera().m_nMode; // Looks like inlined code
        if (cameraMode != MODE_SNIPER && cameraMode != MODE_ROCKETLAUNCHER && cameraMode != MODE_ROCKETLAUNCHER_HS && cameraMode != MODE_M16_1STPERSON &&
            cameraMode != MODE_HELICANNON_1STPERSON && cameraMode != MODE_CAMERA
        ) {
            player->ClearWeaponTarget();
            if (player->GetTaskManager().GetActiveTask()) {
                CTask* pActiveTask = player->GetTaskManager().GetActiveTask();
                if (pActiveTask->GetTaskType() != TASK_COMPLEX_JUMP)
                    player->GetTaskManager().SetTask(new CTaskComplexJump(COMPLEX_JUMP_TYPE_JUMP), 3, false);
            }
        }
    }

    PlayIdleAnimations(player);
    player->m_pedIK.bSlopePitch = true;
    return player->m_pedIK.m_nFlags;
}

void CTaskSimplePlayerOnFoot::InjectHooks() {
    RH_ScopedClass(CTaskSimplePlayerOnFoot);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x685750);
    RH_ScopedInstall(Destructor, 0x6857D0);
    RH_ScopedInstall(ProcessPed_Reversed, 0x688810);
    RH_ScopedInstall(MakeAbortable_Reversed, 0x6857E0);
    // RH_ScopedInstall(ProcessPlayerWeapon, 0x6859A0);
    RH_ScopedInstall(PlayIdleAnimations, 0x6872C0);
    RH_ScopedInstall(PlayerControlZeldaWeapon, 0x687C20);
    RH_ScopedInstall(PlayerControlDucked, 0x687F30);
    RH_ScopedInstall(PlayerControlZelda, 0x6883D0);
}

// 0x685750
CTaskSimplePlayerOnFoot* CTaskSimplePlayerOnFoot::Constructor() {
    this->CTaskSimplePlayerOnFoot::CTaskSimplePlayerOnFoot();
    return this;
}

// 0x68B0C0
CTaskSimplePlayerOnFoot* CTaskSimplePlayerOnFoot::Destructor() {
    this->CTaskSimplePlayerOnFoot::~CTaskSimplePlayerOnFoot();
    return this;
}

bool CTaskSimplePlayerOnFoot::ProcessPed_Reversed(CPed* ped) {
    return CTaskSimplePlayerOnFoot::ProcessPed(ped);
}

// 0x6857E0
bool CTaskSimplePlayerOnFoot::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return CTaskSimplePlayerOnFoot::MakeAbortable(ped, priority, event);
}
