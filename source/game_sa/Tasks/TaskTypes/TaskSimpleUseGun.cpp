#include "StdInc.h"

#include "IKChainManager_c.h"
#include "TaskSimpleUseGun.h"
#include "TaskSimpleGetUp.h"
#include "TaskSimpleDuck.h"
#include "TaskSimpleFight.h"

void CTaskSimpleUseGun::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleUseGun, 0x86D724, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    //RH_ScopedInstall(Constructor, 0x61DE60);
    //RH_ScopedInstall(Destructor, 0x61DF30);

    RH_ScopedGlobalInstall(FinishGunAnimCB, 0x61F3A0);
    RH_ScopedGlobalInstall(RequirePistolWhip, 0x61E200);
    RH_ScopedInstall(Reset, 0x624DC0);
    RH_ScopedInstall(AimGun, 0x61ED10);
    RH_ScopedInstall(FireGun, 0x61EB10);
    RH_ScopedInstall(StartCountDown, 0x61E160);
    RH_ScopedInstall(ClearAnim, 0x61E190);
    RH_ScopedInstall(PlayerPassiveControlGun, 0x61E0A0);
    RH_ScopedInstall(ControlGun, 0x61E040);
    RH_ScopedInstall(AbortIK, 0x61DFA0);
    RH_ScopedInstall(ControlGunMove, 0x61E0C0);
    RH_ScopedInstall(RemoveStanceAnims, 0x61E8E0);
    RH_ScopedInstall(StartAnim, 0x624F30);
    RH_ScopedInstall(SetMoveAnim, 0x61E3F0);
    RH_ScopedVMTInstall(Clone, 0x622F20);
    RH_ScopedVMTInstall(GetTaskType, 0x61DF20);
    RH_ScopedVMTInstall(MakeAbortable, 0x624E30);
    RH_ScopedVMTInstall(ProcessPed, 0x62A380, { .reversed = false });
    RH_ScopedVMTInstall(SetPedPosition, 0x624ED0);
}

// 0x61DE60
CTaskSimpleUseGun::CTaskSimpleUseGun(CEntity* targetEntity, CVector vecTarget, eGunCommand firstCmd, uint16 burstLength, bool isAimImmediate) :
    m_NextCmd{ firstCmd },
    m_TargetPos{ vecTarget },
    m_TargetEntity{ targetEntity },
    m_BurstShots{ burstLength },
    m_IsAimImmediate{ isAimImmediate } {
}

// 0x61DF30
CTaskSimpleUseGun::~CTaskSimpleUseGun() {
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x61E0C0
bool CTaskSimpleUseGun::ControlGunMove(const CVector2D& moveDir) {
    const auto maxDelta = CTimer::GetTimeStep() * 0.07f;
    for (size_t i = 0; i < 2; i++) {
        m_MoveCmd[i] = std::clamp(moveDir[i], m_MoveCmd[i] - maxDelta, m_MoveCmd[i] + maxDelta);
    }
    m_HasMoveControl = true;
    return true;
}

// 0x61F3A0
void CTaskSimpleUseGun::FinishGunAnimCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = CTask::Cast<CTaskSimpleUseGun>(static_cast<CTask*>(data));

    if (self->m_BurstShots > 0 && self->m_LastCmd == eGunCommand::RELOAD && self->m_NextCmd < eGunCommand::END_LEISURE) {
        switch (anim->GetAnimId()) {
        case ANIM_ID_RELOAD:
        case ANIM_ID_CROUCHRELOAD: {
            self->m_NextCmd = eGunCommand::RELOAD;
        }
        }
    }

    if (self->m_Anim == anim) {
        self->m_Anim = nullptr;
    }
}

// 0x61EB10
void CTaskSimpleUseGun::FireGun(CPed* ped, bool isLeftHand) {
    const auto DoFireGun = [&](CVector origin, CVector barrelPos) {
        ped->GetActiveWeapon().Fire(
            ped,
            &origin,
            &barrelPos,
            m_TargetEntity,
            m_TargetPos.x == 0.f || m_TargetPos.y == 0.f ? nullptr : &m_TargetPos,
            nullptr
        );
    };

    if (ped->bCalledPreRender) { // If pre-render was called, the bone positions are up-to-date
        CVector    barrelPos  = m_WeaponInfo->m_vecFireOffset;
        const auto handBoneId = isLeftHand ? BONE_L_HAND : BONE_R_HAND;
        CVector    origin     = ped->GetBonePosition(handBoneId);
        origin.z += barrelPos.z + 0.15f;
        ped->GetTransformedBonePosition(barrelPos, handBoneId);
        if (!ped->m_pedIK.bUseArm) {
            ped->m_pedIK.bGunReachedTarget = g_ikChainMan.IsFacingTarget(ped, isLeftHand ? eIKChainSlot::LEFT_ARM : eIKChainSlot::RIGHT_ARM);
        }
        DoFireGun(origin, barrelPos);
    } else { // Otherwise ped is probably not on screen anyways, so just do something good enough
        CVector origin = ped->GetPosition();
        origin.z += 0.7f;
        DoFireGun(origin, origin);
    }

    if (ped->GetActiveWeapon().GetState() == WEAPONSTATE_FIRING) {
        ped->DoGunFlash(250, isLeftHand);
    }
}

// 0x61E0A0
bool CTaskSimpleUseGun::PlayerPassiveControlGun() {
    m_IsInControl               = true;
    const auto IsCommandPassive = [](eGunCommand c) {
        return notsa::contains({ eGunCommand::UNKNOWN, eGunCommand::NONE, eGunCommand::AIM }, c);
    };
    if (IsCommandPassive(m_LastCmd) && IsCommandPassive(m_NextCmd)) {
        m_NextCmd = eGunCommand::END_LEISURE;
        return true;
    }
    return false;
}

// 0x61E8E0
void CTaskSimpleUseGun::RemoveStanceAnims(CPed* ped, float x) {
    if (m_Anim && !(m_Anim->m_Flags & ANIMATION_IS_PLAYING) && m_Anim->GetTimeProgress() < 1.f) {
        if (m_WeaponInfo && m_WeaponInfo->GetAnimLoopStart() == m_Anim->GetCurrentTime()) {
            m_Anim->SetCurrentTime(m_WeaponInfo->GetAnimLoopEnd());
        }
        m_Anim->SetFlag(ANIMATION_IS_PLAYING);
    }

    bool pedHasStanceAnims = false;
    for (const auto animId : {
        ANIM_ID_GUN_STAND,
        ANIM_ID_GUNMOVE_FWD,
        ANIM_ID_GUNMOVE_L,
        ANIM_ID_GUNMOVE_BWD,
        ANIM_ID_GUNMOVE_R,
    }) {
        if (const auto a = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, animId)) {
            a->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE);
            pedHasStanceAnims = true;
        }
    }

    ped->SetMoveState(PEDMOVE_STILL);
    ped->m_nSwimmingMoveState = PEDMOVE_STILL;

    if (!pedHasStanceAnims) {
        return;
    }

    const auto DoBlendAnimAndStart = [ped](AnimationId animId) {
        const auto animWalk = CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, animId);
        animWalk->SetFlag(ANIMATION_IS_PLAYING);
    };

    if (ped->IsPlayer()) {
        if (const auto pad = ped->AsPlayer()->GetPadFromPlayer()) {
            if (std::abs(pad->GetPedWalkUpDown()) > 50 || std::abs(pad->GetPedWalkLeftRight()) > 50) { // NOTSA: They didn't use abs ;)
                DoBlendAnimAndStart(ANIM_ID_WALK);

                ped->SetMoveState(PEDMOVE_WALK);
                ped->m_nSwimmingMoveState = PEDMOVE_WALK;

                if (const auto pd = ped->m_pPlayerData) {
                    pd->m_fMoveBlendRatio = 1.f;
                }
            }
        }
    }

    DoBlendAnimAndStart(ANIM_ID_IDLE);

    if (m_NextCmd == eGunCommand::END_LEISURE) {
        switch (ped->m_nAnimGroup) {
        case ANIM_GROUP_PLAYERROCKET:
        case ANIM_GROUP_PLAYERROCKETF:
        case ANIM_GROUP_PLAYERROCKETM:
            break;
        default:
            CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_GUN_2_IDLE, 8.f);
        }
    }
}

// 0x61E200
bool CTaskSimpleUseGun::RequirePistolWhip(CPed* ped, CEntity* targetEntity) {
    if (const auto tUseGun = ped->GetIntelligence()->GetTaskUseGun()) {
        if (tUseGun->GetLastGunCommand() == eGunCommand::PISTOLWHIP) {
            return false;
        }
    }
    const auto IsPistolWhipRequiredForPed = [
        &,
        &pedPos = ped->GetPosition()
    ](CPed& otherPed) {
        if (!otherPed.IsAlive()) { // Ped is dead :D
            return false;
        }
        const auto forPedHeadPos = otherPed.GetBonePosition(BONE_HEAD);
        if (pedPos.z >= forPedHeadPos.z || pedPos.z + 1.5f <= forPedHeadPos.z) { // Ped is below/above us
            return false;
        }
        if (otherPed.bFallenDown && !CTask::DynCast<CTaskSimpleGetUp>(ped->GetTaskManager().GetSimplestActiveTask())) { // Fell, but hasn't got up yet?
            return false;
        }
        const auto pedToOther = otherPed.GetPosition() - ped->GetPosition();
        if (pedToOther.SquaredMagnitude2D() >= 1.f) {
            return false;
        }
        const auto pedToOtherDot = ped->GetForward().Dot(pedToOther);
        if (pedToOtherDot <= 0.f) { // Is behind?
            return false;
        }
        if (std::abs(ped->GetRight().Dot(pedToOther)) / pedToOtherDot >= 1.f / SQRT_3) { // 1/sqrt3 = 0.57730001
            return false;
        }
        return true;
    };
    return targetEntity && targetEntity->IsPed()
        ? IsPistolWhipRequiredForPed(*targetEntity->AsPed())
        : rng::any_of(ped->GetIntelligence()->GetPedScanner().GetEntities<CPed>(), IsPistolWhipRequiredForPed);
}

// 0x61E040
bool CTaskSimpleUseGun::ControlGun(CPed* ped, CEntity* target, eGunCommand cmd) {
    m_IsInControl = true;
    if (target != m_TargetEntity) { // New target?
        m_TargetEntity  = target;
        m_IsArmIKInUse  = false;
        m_IsLookIKInUse = false;
    }
    m_NextCmd = std::max(cmd, m_NextCmd);
    return true;
}

// 0x61DFA0
void CTaskSimpleUseGun::AbortIK(CPed* ped) {
    if (m_IsArmIKInUse) {
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_RIGHT, ped);
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_LEFT, ped);
        m_IsArmIKInUse = false;
    }
    if (m_IsLookIKInUse) {
        g_ikChainMan.AbortLookAtIfLooking(ped);
        m_IsLookIKInUse = false;
    }
    ped->m_pedIK.bEverythingUsed = false;
}

// 0x61ED10
void CTaskSimpleUseGun::AimGun(CPed* ped) {
    const auto ikBlendTime = m_IsAimImmediate ? 0 : 250;
    const auto ikSpeed     = m_IsAimImmediate ? 1.f : 0.5f;

    const auto isAimingWithArm = ped->bIsDucking || !m_WeaponInfo->flags.bAimWithArm;
    if (isAimingWithArm) {
        if (!ped->m_pedIK.bEverythingUsed) {
            AbortIK(ped);
        }
    }
    ped->m_pedIK.bUseArm         = !isAimingWithArm;
    ped->m_pedIK.bEverythingUsed = isAimingWithArm;

    const auto DoLookAt = [this, ped](eBoneTag lookAtBone, CEntity* lookAtEntity, CVector* lookAtPos) {
        g_ikChainMan.LookAt(
            "TaskUseGun",
            ped,
            lookAtEntity,
            9'999'999,
            lookAtBone,
            lookAtPos,
            true,
            0.25f,
            250,
            3,
            false
        );
        m_IsLookIKInUse = true;
    };

    const auto DoPointArm = [&, this, ped](eIKArm arm, eBoneTag armBone = BONE_UNKNOWN, CEntity* lookAtEntity = nullptr, CVector* lookAtPos = nullptr) {
        g_ikChainMan.PointArm(
            "CTaskSimpleUseGun",
            arm,
            ped,
            lookAtEntity,
            armBone,
            lookAtPos,
            ikSpeed,
            ikBlendTime,
            100.f
        );
        m_IsArmIKInUse = true;
    };

    if (m_TargetEntity) { // If there's a target entity, aim at them
        const auto [lookAtPos, lookAtBone] = GetAimLookAtInfo(); // 0x61EDB8

        if (!ped->IsPlayer()) {
            ped->Say(CTX_GLOBAL_TARGET);
        }

        if (ped->m_pedIK.bUseArm) { // Removed redudant `if` check from here
            if (!m_IsLookIKInUse) {
                if (m_Anim->GetBlendAmount() > 0.98f) {
                    DoLookAt(lookAtBone, m_TargetEntity, nullptr); // 0x61EF0B
                }
            }
             
            if (!m_IsArmIKInUse) { // 0x61EF0B
                DoPointArm(eIKArm::IK_ARM_RIGHT, lookAtBone, m_TargetEntity, nullptr);
                if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked) {
                    DoPointArm(eIKArm::IK_ARM_LEFT, lookAtBone, m_TargetEntity, nullptr);
                } 
            }

            ped->m_pedIK.RotateTorsoForArm(m_TargetEntity->GetPosition());
        } else {
            ped->m_pedIK.PointGunAtPosition(lookAtPos, m_Anim->GetBlendAmount());
        }
    } else if (ped->m_pedIK.bUseArm) { // 0x61EFB4 - Process player free-aiming
        const auto pd = ped->m_pPlayerData;
        if (pd && pd->m_bFreeAiming && CVector2D{ m_TargetPos }.IsZero() && notsa::contains({ MODE_AIMWEAPON, MODE_AIMWEAPON_ATTACHED }, TheCamera.m_aCams[0].m_nMode)) { // Aim to in front of us
            CVector origin, target;
            TheCamera.Find3rdPersonCamTargetVector(20.f, ped->GetPosition() + CVector{0.f, 0.f, 0.7f}, origin, target);

            if (m_Anim->GetBlendAmount() > 0.98f) {
                DoLookAt(BONE_UNKNOWN, nullptr, &target);
            }

            DoPointArm(eIKArm::IK_ARM_RIGHT, BONE_UNKNOWN, nullptr, &target);
            if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked) {
                DoPointArm(eIKArm::IK_ARM_LEFT, BONE_UNKNOWN, nullptr, &target);
            }

            ped->m_pedIK.RotateTorsoForArm(target);
        } else if (!m_IsArmIKInUse || !m_IsLookIKInUse) { // 0x61F144 - Neither pointing with arm, or looking at anything -> Just look somewhere...
            if (m_TargetPos.x == 0.f || m_TargetPos.y == 0.f) { // TODO/NOTE: Bug? Perhaps they meant &&?
                CVector target{0.f, 2.f, 0.f};

                if (m_Anim->GetBlendAmount() > 0.98f) {
                    DoLookAt(BONE_UNKNOWN, nullptr, &target);
                }

                DoPointArm(eIKArm::IK_ARM_RIGHT, BONE_R_UPPER_ARM, ped, &target);
                if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked) {
                    DoPointArm(eIKArm::IK_ARM_LEFT, BONE_L_UPPER_ARM, ped, &target);
                }
            } else { // 0x61F172 - Either arm/look IK is in use, meaning there's a target
                if (m_Anim->GetBlendAmount() > 0.98f) {
                    DoLookAt(BONE_UNKNOWN, nullptr, &m_TargetPos);
                }

                DoPointArm(eIKArm::IK_ARM_RIGHT, BONE_UNKNOWN, nullptr, &m_TargetPos);
                if (m_WeaponInfo->flags.bTwinPistol && !ped->bLeftArmBlocked) {
                    DoPointArm(eIKArm::IK_ARM_LEFT, BONE_UNKNOWN, nullptr, &m_TargetPos);
                }
            }
        }
    } else if (m_TargetPos.x == 0.f || m_TargetPos.y == 0.f) { // 0x61F30E
        ped->m_pedIK.PointGunInDirection(
            ped->m_fCurrentRotation,
            ped->IsPlayer() ? ped->m_pPlayerData->m_fLookPitch : 0.f,
            false,
            m_Anim->GetBlendAmount()
        );
    } else { // 0x61F305
        ped->m_pedIK.PointGunAtPosition(m_TargetPos, m_Anim->GetBlendAmount());
    }

    if (m_WeaponInfo->flags.bTwinPistol && ped->bLeftArmBlocked) {
        g_ikChainMan.AbortPointArmIfPointing(eIKArm::IK_ARM_RIGHT, ped);
    }
}

// 0x61E190
void CTaskSimpleUseGun::ClearAnim(CPed* ped) {
    if (m_Anim) {
        if (m_Anim->GetBlendAmount() > 0.f && m_Anim->GetBlendDelta() >= 0.f) {
            m_Anim->SetBlendDelta(-8.f);
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }
    if (m_LastCmd < eGunCommand::END_LEISURE) {
        m_LastCmd = eGunCommand::NONE;
    }
    AbortIK(ped);
}

// 0x624E30
bool CTaskSimpleUseGun::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_LEISURE) {
        m_NextCmd = eGunCommand::END_LEISURE;
        return false;
    }
    RemoveStanceAnims(ped, -4.f);
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        CAnimManager::BlendAnimation(ped->m_pRwClump, ped->m_nAnimGroup, ANIM_ID_IDLE, 1000.f);
    }
    if (m_Anim) {
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            m_Anim->SetBlendDelta(-1000.f);
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }
    m_IsFinished = true;
    AbortIK(ped);
    if (const auto pd = ped->m_pPlayerData) {
        pd->m_fAttackButtonCounter = 0.f;
    }
    return true;
}

// 0x62A380
bool CTaskSimpleUseGun::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x62A380>(this, ped);
    /*
    * Code so far (should be) good, but has to be finished...
    * 
    m_IsLookIKInUse    = m_IsLookIKInUse && g_ikChainMan.IsLooking(ped);
    m_IsArmIKInUse     = m_IsArmIKInUse && g_ikChainMan.IsArmPointing(0, ped);
    m_IsFiringGunRightHandThisFrame = false;
    m_IsFiringGunLeftHandThisFrame = false;

    if (m_WeaponInfo) { // Inverted
        if (m_WeaponInfo != &ped->GetActiveWeapon().GetWeaponInfo(ped)) {
            MakeAbortable(ped);
        }
    } else {
        if (const auto pd = ped->m_pPlayerData) {
            if (pd->m_nChosenWeapon != ped->m_nActiveWeaponSlot) {
                return false;
            }
        }
        m_WeaponInfo = &ped->GetActiveWeapon().GetWeaponInfo(ped);
        if (m_WeaponInfo->m_nWeaponFire == 0 || m_WeaponInfo->flags.bThrow) {
            m_IsFinished = true;
            m_WeaponInfo = nullptr;

            AbortIK(ped);
            if (const auto pd = ped->m_pPlayerData) {
                pd->m_fAttackButtonCounter = 0.f;
            }
            return true;
        }
        m_MoveCmd = { 0.f, 0.f };
    }

    if (!m_WeaponInfo->flags.b1stPerson || !ped->m_pPlayerData) {
        ped->bTestForBlockedPositions = true;
    }

    if (!m_IsFinished) {
        if (const auto pd = ped->m_pPlayerData) {
            pd->m_fAttackButtonCounter *= std::pow(0.96f, CTimer::GetTimeStep());
        }
        if (m_IsInControl) {
            m_CountDownFrames = -1;
            if (ped->bDuckRightArmBlocked && (ped->bGetUpAnimStarted || !m_WeaponInfo->flags.bAimWithArm)) {
                if (m_LastCmd != eGunCommand::PISTOLWHIP && (m_NextCmd != eGunCommand::PISTOLWHIP || m_Anim)) { // 0x62A532
                    if (m_Anim) {
                        m_Anim->SetBlendDelta(-4.f);
                        m_Anim->SetFlag(ANIMATION_IS_PLAYING, false);
                        m_Anim->SetFlag(ANIMATION_FREEZE_LAST_FRAME, true);
                    } else if (notsa::contains({ eGunCommand::AIM, eGunCommand::FIRE, eGunCommand::FIREBURST }, m_NextCmd)) { // 0x62A63B - Inverted
                        AbortIK(ped);
                        m_IsLOSBlocked = true;
                        if (m_LastCmd == eGunCommand::FIREBURST) {
                            m_LastCmd = eGunCommand::AIM;
                        }

                    SET_MOVE_ANIM_AND_COMMAND_RET_0: // 0x62A586
                        if (m_WeaponInfo->flags.bAimWithArm || ped->bIsDucking || m_HasMoveControl) {
                            SetMoveAnim(ped);
                        }
                        if (m_NextCmd < eGunCommand::END_LEISURE) {
                            if (m_NextCmd != eGunCommand::RELOAD || notsa::contains({ eGunCommand::FIRE, eGunCommand::FIREBURST }, m_LastCmd)) {
                                m_NextCmd = eGunCommand::NONE;
                            }
                        }
                        m_IsInControl = false;
                        return false;
                    }
                }
            } else if (m_NextCmd == eGunCommand::PISTOLWHIP && m_LastCmd <= eGunCommand::FIREBURST) { // 0x62A5E4
                if (m_Anim) {
                    m_Anim->SetDefaultDeleteCallback();
                    m_Anim = nullptr;
                }
                AbortIK(ped);
            }

            if (!m_Anim) { // 0x62A55B
                StartAnim(ped);
                if (!m_Anim) {
                    AbortIK(ped);
                }
                goto SET_MOVE_ANIM_AND_COMMAND_RET_0;
            }

            if (m_LastCmd == eGunCommand::RELOAD) { // 0x62A65A
                if (notsa::contains({ ANIM_ID_RELOAD, ANIM_ID_CROUCHRELOAD }, m_Anim->GetAnimId())) {
                    if (m_Anim->GetBlendDelta() >= 0.f) {
                        m_Anim->SetBlendDelta(-4.f);
                    }
                }
                m_SkipAim = false;
                if (ped->m_pedIK.bUseArm) { // 0x62ACB7
                    const auto isAimTargetPedDead = 
                    const auto aimTargetPos = GetAimTargetPosition(ped, isAimTargetPedDead);
                    if (aimTargetPos.x != 0.f || aimTargetPos.y != 0.f) {
                        const auto aimDir = (aimTargetPos - ped->GetPosition()).Normalized(); // 0x62ADF0
                        const auto targetAngleToUs = CGeneral::LimitRadianAngle(aimDir.Heading() - ped->m_fCurrentRotation);
                        m_SkipAim = [&, this]{
                            if (m_TargetEntity && m_TargetEntity->IsPed() && m_TargetEntity->AsPed()->m_fHealth <= 0.f) {
                                if (DegreesToRadians(115.f - 40.f) < targetAngleToUs || targetAngleToUs < -DegreesToRadians(130.f - 40.f)) {
                                    return true;
                                }
                                if (aimDir.Dot(ped->GetUpVector()) < -0.8f) { // 0x62AE84
                                    return true;
                                }
                            } else {
                                if (DegreesToRadians(115.f) < targetAngleToUs || targetAngleToUs < -DegreesToRadians(130.f)) {
                                    return true;
                                }
                            }
                            return m_SkipAim;
                        }();
                    }
                    m_SkipAim |= ped->bIsDucking && ped->bDuckRightArmBlocked; // 0x62AEB6
                }
            }
        }
    }
    */
}

// 0x624ED0
bool CTaskSimpleUseGun::SetPedPosition(CPed* ped) {
    if (m_WeaponInfo == &ped->GetActiveWeapon().GetWeaponInfo(ped)) {
        if (m_IsFiringGunRightHandThisFrame) { // Right hand
            FireGun(ped, false);
            m_IsFiringGunRightHandThisFrame = false;
        }
        if (m_IsFiringGunLeftHandThisFrame) { // Left hand
            FireGun(ped, true);
            m_IsFiringGunLeftHandThisFrame = false;
        }
    }
    return false;
}

// notsa
CVector CTaskSimpleUseGun::GetAimTargetPosition(CPed* ped) const {
    if (m_TargetEntity) {
        if (m_TargetEntity->IsPed()) {
            return m_TargetEntity->AsPed()->GetBonePosition(BONE_SPINE1);
        }
        return m_TargetEntity->GetPosition();
    }
    if (ped->IsPlayer() && TheCamera.m_aCams[0].m_nMode == MODE_AIMWEAPON) {
        CVector src, target;
        TheCamera.Find3rdPersonCamTargetVector(20.f, ped->GetPosition() + CVector{0.f, 0.f, 0.7f}, src, target);
        return target;
    }
    return m_TargetPos;
}

// notsa
std::pair<CVector, eBoneTag> CTaskSimpleUseGun::GetAimLookAtInfo() const {
    if (m_TargetEntity->IsPed()) {
        const auto targetPed = m_TargetEntity->AsPed();
        if (const auto pd = targetPed->m_pPlayerData) {
            CVector ret = pd->m_vecTargetBoneOffset;
            targetPed->GetTransformedBonePosition(ret, (eBoneTag)pd->m_nTargetBone);
            return {ret, (eBoneTag)pd->m_nTargetBone};
        }
    }
    return {m_TargetEntity->GetPosition(), BONE_SPINE1};
}

// 0x624DC0
void CTaskSimpleUseGun::Reset(CPed* ped, CEntity* targetEntity, CVector targetPos, eGunCommand, int16 burstLength) {
    AbortIK(ped);

    m_TargetPos       = targetPos;
    m_BurstLength     = burstLength;
    m_BurstShots      = 0;
    m_CountDownFrames = -1;
    m_IsLOSBlocked    = false;
    m_IsArmIKInUse    = false;
    m_IsLookIKInUse   = false;
    m_TargetEntity    = targetEntity;
}

// 0x61E3F0
void CTaskSimpleUseGun::SetMoveAnim(CPed* ped) {
    const auto
        animGunStand   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUN_STAND),
        animGunMoveFwd = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_FWD),
        animGunMoveL   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_L),
        animGunMoveBwd = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_BWD),
        animGunMoveR   = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_GUNMOVE_R);

    // 0x61E444
    if (ped->bIsDucking) {
        if (const auto tDuck = ped->GetIntelligence()->GetTaskDuck()) {
            switch (m_LastCmd) {
            case eGunCommand::FIRE:
            case eGunCommand::FIREBURST:
            case eGunCommand::RELOAD:
                tDuck->ForceStopMove();
            }
        }
        m_MoveCmd = {0.f, 0.f};
        m_HasMoveControl = false;
        return;
    }

    // 0x61E4A5
    const auto isInMoveControl = m_HasMoveControl && (m_LastCmd != eGunCommand::FIRE || m_WeaponInfo->flags.bMoveFire);
    const auto moveCmdDist = isInMoveControl
        ? m_MoveCmd.Magnitude()
        : 0.f;
    const auto moveCmdAbsSum = isInMoveControl
        ? std::abs(m_MoveCmd.x) + std::abs(m_MoveCmd.y)
        : 0.f;

    const auto DoBlendAnim = [ped](AnimationId animId, float blendDelta) {
        return CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, animId, blendDelta);
    };

    // 0x61E505
    if (moveCmdAbsSum < 0.1f) {
        const auto moveAnim = m_WeaponInfo->flags.bAimWithArm
            ? ped->IsGangster() || ped->IsCop()
                ? DoBlendAnim(ANIM_ID_GANG_GUNSTAND, 4.f) // Why is this used for cops? (0x61E529, inverted)
                : DoBlendAnim(ANIM_ID_IDLE, 8.f)
            : DoBlendAnim(ANIM_ID_GUN_STAND, 8.f);

        const auto resetPlayTime = moveAnim && moveAnim->GetBlendAmount() > 0.95f;
        for (const auto anim : { animGunMoveBwd, animGunMoveL, animGunMoveR, animGunMoveBwd }) {
            if (anim) {
                anim->SetFlag(ANIMATION_IS_PLAYING, false);
                if (resetPlayTime) {
                    anim->SetCurrentTime(0.f);
                }
            }
        }

        m_MoveCmd        = { 0.f, 0.f };
        m_HasMoveControl = false;

        return; // 0x61E5F3
    }

    // 0x61E5F6 - Blend in new move anim according to the move command
    if (m_WeaponInfo->flags.bAimWithArm) {
        const auto moveAnim = DoBlendAnim(
            m_MoveCmd.x > 0.75f
                ? ANIM_ID_GUNMOVE_R
                : m_MoveCmd.x <= -0.75f
                    ? ANIM_ID_GUNMOVE_L
                    : m_MoveCmd.y <= 0.f
                        ? ANIM_ID_GUNMOVE_FWD
                        : ANIM_ID_GUNMOVE_BWD,
            8.f
        );
        moveAnim->SetFlag(ANIMATION_IS_PLAYING, true);
        ped->SetMoveState(PEDMOVE_NONE);
        return; // 0x61E675
    }

    //
    // Handle anim transition defined by the current move command
    //

    // 0x61E67C - Calculate lerp progress
    float t = 1.f;
    if (const auto aGS = animGunStand) {
        // 0x61E682 - Adjust blend amount
        if (aGS->GetBlendDelta() >= 0.f) {
            aGS->SetBlendDelta(0.f);
            aGS->SetBlendAmount(std::max(0.f, aGS->GetBlendAmount() - CTimer::GetTimeStep() * 0.16f));
        }

        // Calculate lerp t
        t = 1.f - std::clamp(CTimer::GetTimeStep() * aGS->GetBlendDelta() * 0.02f + aGS->GetBlendAmount(), 0.f, 1.f);
    }

    // Process transition from one gun move to another
    const auto DoTransitionGunMoveAnim = [&, ped, this](CAnimBlendAssociation* fromAnim, CAnimBlendAssociation* toAnim, AnimationId toAnimId, float toAnimBlendAmnt) {
        // If previous anim exists, blend it out
        if (fromAnim) {
            fromAnim->SetBlendAmount(0.f);
            fromAnim->SetBlendDelta(0.f);
        }

        // If new anim doesn't exists, create it
        if (!toAnim) {
            toAnim = CAnimManager::AddAnimation(ped->m_pRwClump,ANIM_GROUP_DEFAULT, toAnimId);
        }

        // Set params of the anim we're transitioning to
        toAnim->SetBlendDelta(0.f);
        toAnim->SetBlendAmount(toAnimBlendAmnt);
        toAnim->SetFlag(ANIMATION_IS_PLAYING, true);
        toAnim->SetSpeed(m_WeaponInfo->m_fMoveSpeed * moveCmdDist);
    };

    const auto moveCmdLerp = m_MoveCmd * (1.f / moveCmdAbsSum) * t;

    // 0x61E73F - Process X axis (left, right)
    if (moveCmdLerp.x < 0.f) { // right => left
        DoTransitionGunMoveAnim(animGunMoveR, animGunMoveL, ANIM_ID_GUNMOVE_L, std::abs(moveCmdLerp.x));
    } else if (moveCmdLerp.x > 0.f) { // left => right
        DoTransitionGunMoveAnim(animGunMoveL, animGunMoveR, ANIM_ID_GUNMOVE_R, std::abs(moveCmdLerp.x));
    }

    // 0x61E7E5 - Process Y axis (fwd, bwd)
    if (moveCmdLerp.y < 0.f) { // bwd => fwd
        DoTransitionGunMoveAnim(animGunMoveBwd, animGunMoveFwd, ANIM_ID_GUNMOVE_FWD, std::abs(moveCmdLerp.y));
    } else if (moveCmdLerp.y > 0.f) { // fwd => bwd
        DoTransitionGunMoveAnim(animGunMoveFwd, animGunMoveBwd, ANIM_ID_GUNMOVE_BWD, std::abs(moveCmdLerp.y));
    }

    // 0x61E88B
    if (m_LastCmd == eGunCommand::AIM) {
        if (m_Anim && m_Anim->GetBlendAmount() > 0.f && m_Anim->GetBlendDelta() >= 0.f && !m_WeaponInfo->flags.bMoveAim) {
            m_Anim->SetBlendDelta(-4.f);
        }
    }

    m_HasMoveControl = false;
}

// 0x624F30
void CTaskSimpleUseGun::StartAnim(CPed* ped) {
    const auto tDuck = ped->bIsDucking
        ? ped->GetIntelligence()->GetTaskDuck()
        : nullptr;

    if (m_Anim) {
        if (m_NextCmd == eGunCommand::END_NOW && m_LastCmd <= eGunCommand::FIREBURST) {
            if (m_Anim->GetBlendDelta() > -8.f && m_Anim->GetBlendAmount() > 0.f) {
                m_Anim->SetBlendDelta(-8.f);
            }
        }
        m_Anim->SetDefaultDeleteCallback();
        m_Anim = nullptr;
    }

    switch (m_NextCmd) {                      
    case eGunCommand::AIM:
    case eGunCommand::FIRE:
    case eGunCommand::FIREBURST: { // 0x624FC1
        if (m_NextCmd == eGunCommand::AIM) {
            if (!m_MoveCmd.IsZero() && !m_WeaponInfo->flags.bMoveFire || tDuck && tDuck->IsTaskInUseByOtherTasks()) { // 0x624FC3 - Check if we can fire right now
                switch (m_LastCmd) {
                case eGunCommand::FIRE:
                case eGunCommand::FIREBURST:
                    m_LastCmd = eGunCommand::AIM;
                }
                return; // Nope, we can't fire right now, so continue aiming
            }
        } else if (tDuck && tDuck->StopFireGun()) {
            return;
        }

        if (m_NextCmd == eGunCommand::FIREBURST) {
            m_BurstShots = m_BurstLength;
        }

        m_Anim = CAnimManager::BlendAnimation( // 0x62503A
            ped->m_pRwClump,
            m_WeaponInfo->m_eAnimGroup,
            ped->bIsDucking && m_WeaponInfo->flags.bCrouchFire
                ? ANIM_ID_CROUCHFIRE
                : ANIM_ID_FIRE
        );

        if (m_LastCmd == eGunCommand::RELOAD) {
            if (m_WeaponInfo->flags.bReload2Start) {
                m_Anim->SetCurrentTime(m_WeaponInfo->GetAnimLoopStart(ped->bIsDucking));
                m_Anim->SetFlag(ANIMATION_IS_PLAYING, false);
            }
        }

        m_Anim->SetFinishCallback(FinishGunAnimCB, this);

        break;
    }
    case eGunCommand::RELOAD: { // 0x6250E5
        if (tDuck && tDuck->StopFireGun()) {
            return;
        }

        if (m_LastCmd != eGunCommand::RELOAD) {
            m_BurstShots = m_WeaponInfo->flags.bTwinPistol ? 2 : 1;
        }

        if (m_BurstShots > 0) { // 0x625196 - Inverted
            m_Anim = CAnimManager::BlendAnimation( // 0x62511C
                ped->m_pRwClump,
                m_WeaponInfo->m_eAnimGroup,
                ped->bIsDucking && m_WeaponInfo->flags.bCrouchFire
                    ? ANIM_ID_CROUCHRELOAD
                    : ANIM_ID_RELOAD
            );
            m_Anim->Start();
            m_Anim->SetFinishCallback(FinishGunAnimCB, this);

            m_BurstShots--;
        } else {
            m_NextCmd = eGunCommand::NONE;
        }

        break;
    }
    case eGunCommand::PISTOLWHIP: {
        if (tDuck && tDuck->StopFireGun()) {
            return;
        }
        m_Anim = CAnimManager::BlendAnimation( // 0x6251DC
            ped->m_pRwClump,
            CTaskSimpleFight::m_aComboData[12].m_nAnimGroup,
            ped->bIsDucking
                ? ANIM_ID_FIGHT_2
                : ANIM_ID_FIGHT_1
        );
        m_Anim->SetFinishCallback(FinishGunAnimCB, this);
        break;
    }
    case eGunCommand::NONE:
    case eGunCommand::END_LEISURE: { // 0x62520A [Moved here for readability]
        RemoveStanceAnims(ped, 4.f);
        m_IsFinished = true;
        break;
    }
    case eGunCommand::END_NOW: { // 0x625214
        RemoveStanceAnims(ped, 8.f);
        m_IsFinished = true;
        break;
    }
    }

    m_LastCmd = std::exchange(m_NextCmd, eGunCommand::NONE);
}

// 0x0x61E160
void CTaskSimpleUseGun::StartCountDown(uint8 numIdleFrames, char isMax) {
    m_CountDownFrames = isMax ? std::min(numIdleFrames, m_CountDownFrames) : numIdleFrames;
    m_HasFiredGun     = false;
}
