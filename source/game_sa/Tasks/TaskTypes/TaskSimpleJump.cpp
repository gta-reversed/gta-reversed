#include "StdInc.h"

#include "TaskSimpleJump.h"
#include "TaskSimpleClimb.h"
#include "Shadows.h"

void CTaskSimpleJump::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleJump, 0x87054C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x679AA0);
    RH_ScopedInstall(CheckIfJumpBlocked, 0x67D590);
    RH_ScopedInstall(Launch, 0x679B80);
    RH_ScopedInstall(StartLaunchAnim, 0x67D7A0);
    RH_ScopedInstall(JumpAnimFinishCB, 0x67A020);
    // VTABLE
    RH_ScopedVMTInstall(Clone, 0x67C510);
    RH_ScopedVMTInstall(MakeAbortable, 0x679B60);
    RH_ScopedVMTInstall(ProcessPed, 0x680C60);
}

CTaskSimpleJump* CTaskSimpleJump::Constructor(bool bCanClimb) {
    this->CTaskSimpleJump::CTaskSimpleJump(bCanClimb);
    return this;
}

// 0x679AA0
CTaskSimpleJump::CTaskSimpleJump(bool bCanClimb) {
    m_bCanClimb = bCanClimb;
    m_pClimbEntity = nullptr;
    m_bIsFinished = false;
    m_bIsJumpBlocked = false;
    m_bClimbJump = false;
    m_bLaunchAnimStarted = false;
    m_bHighJump = false;
    m_pAnim = nullptr;
}

// 0x679AF0
CTaskSimpleJump::~CTaskSimpleJump() {
    if (m_pAnim)
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    CEntity::SafeCleanUpRef(m_pClimbEntity);
};// 0x67C510
CTask* CTaskSimpleJump::Clone() const {
    auto newTask = new CTaskSimpleJump(m_bCanClimb);
    newTask->m_bHighJump = this->m_bHighJump;
    return newTask;
}

// 0x679B60
bool CTaskSimpleJump::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_pAnim) {
        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        m_pAnim->m_BlendDelta = -4.0F;
    }

    return priority == ABORT_PRIORITY_IMMEDIATE;
}

// 0x680C60
bool CTaskSimpleJump::ProcessPed(CPed* ped) {
    if (!m_bIsFinished) {
        if (!m_pAnim) {
            if (!StartLaunchAnim(ped)) {
                m_bLaunchAnimStarted = false;
                return true;
            }
            m_bLaunchAnimStarted = true;
        }
        return this->m_bIsFinished;
    }

    if ((ped->IsPlayer() || m_bCanClimb) && CGame::CanSeeOutSideFromCurrArea())
        m_pClimbEntity = CTaskSimpleClimb::TestForClimb(ped, m_vecClimbPos, m_fClimbAngle, m_nClimbSurfaceType, true);

    if (!m_pClimbEntity)
        CheckIfJumpBlocked(ped);

    if (!m_bIsJumpBlocked) {
        Launch(ped);
    } else if (ped->IsPlayer()) {
        CVector empty{};
        CEventSoundQuiet event(ped, 45.0F, -1, empty);
        GetEventGlobalGroup()->Add(&event, false);
    }

    return true;
}

// 0x67D590
bool CTaskSimpleJump::CheckIfJumpBlocked(CPed* ped) {
    if (ped->m_bIsStuck || ped->GetEventHandler().GetCurrentEventType() == EVENT_STUCK_IN_AIR)
        return false;

    auto pedColData = CModelInfo::GetModelInfo(ped->m_nModelIndex)->m_pColModel->m_pColData;

    CVector posn = ped->GetPosition() + ped->GetForward() * 0.25F + CVector(0.0F, 0.0F, pedColData->m_pSpheres[2].m_vecCenter.z + 0.25F);

    m_bClimbJump = CWorld::TestSphereAgainstWorld(posn, 0.25F, 0, true, true, false, true, false, false) != 0;

    CVector savedPedPosition = ped->GetPosition();
    ped->SetPosn((*ped->m_matrix).TransformPoint(CVector(0.0F, 0.0F, 0.75F)));

    for (uint32 i = 0; i < pedColData->m_nNumSpheres; i++)
        pedColData->m_pSpheres[i].m_fRadius = 0.3F;

    if (ped->TestCollision(false))
        m_bIsJumpBlocked = true;

    ped->SetPosn(savedPedPosition);

    for (uint32 i = 0; i < pedColData->m_nNumSpheres; i++)
        pedColData->m_pSpheres[i].m_fRadius = 0.35F;

    return m_bIsJumpBlocked;
}

// 0x679B80
void CTaskSimpleJump::Launch(CPed* ped) {
    float fHorizontalJumpSpeed = 0.1F;

    auto pSprintAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SPRINT);
    if (pSprintAnim)
        fHorizontalJumpSpeed = lerp(0.17F, 0.22F, pSprintAnim->m_BlendAmount);
    else {
        auto pRunAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_RUN);
        if (pRunAnim)
            fHorizontalJumpSpeed = lerp(0.1F, 0.17F, pRunAnim->m_BlendAmount);
    }

    float fJumpForce = (ped->IsPlayer() || m_bHighJump) ? 8.5F : 4.5F;

    if (ped->m_pPlayerData) {
        float modifier = CStats::GetFatAndMuscleModifier(STAT_MOD_2);
        fHorizontalJumpSpeed *= modifier;
        fJumpForce *= modifier;
    }

    if (ped->IsPlayer() && CCheat::IsActive(CHEAT_MEGAJUMP))
        fJumpForce *= 10.0F;

    ped->ApplyMoveForce(0.0F, 0.0F, fJumpForce);

    if (m_bClimbJump) {
        ped->m_vecMoveSpeed.x = 0.0F;
        ped->m_vecMoveSpeed.y = 0.0F;
    } else if (!m_pClimbEntity && (ped->m_standingOnEntity || ped->m_vecMoveSpeed.SquaredMagnitude2D() < fHorizontalJumpSpeed)) {
        ped->m_vecMoveSpeed.x = -fHorizontalJumpSpeed * sin(ped->m_fCurrentRotation);
        ped->m_vecMoveSpeed.y = fHorizontalJumpSpeed * cos(ped->m_fCurrentRotation);

        if (ped->m_standingOnEntity) {
            ped->m_vecMoveSpeed.x += ped->m_standingOnEntity->AsPhysical()->m_vecMoveSpeed.x;
            ped->m_vecMoveSpeed.y += ped->m_standingOnEntity->AsPhysical()->m_vecMoveSpeed.y;
        }
    }

    ped->bIsStanding = false;
    ped->bIsInTheAir = true;

    if (!m_pClimbEntity) {
        if (m_bClimbJump) {
            auto anim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0F);
            anim->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
        } else
            CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_JUMP_GLIDE, 8.0F);
    }

    if (ped->bDoBloodyFootprints && CLocalisation::Blood()) {
        auto hier = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
        CVector v;
        RwV3dTransformPoints(&v, &v, 1, &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, ped->m_apBones[PED_NODE_LEFT_FOOT]->BoneTag)]);

        CVector v1 = ped->GetForward() * 0.2F;
        v += v1 + CVector(0.0F, 0.0F, -0.1F);
        CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &v, v1.x * 0.26F, v1.y * 0.26F, ped->GetForward().x * 0.14F, ped->GetForward().y * 0.14F, 255, 255, 0, 0, 4.0F,
                                     3000, 1.0F);

        v.Set(0.0F, 0.0F, 0.0F);
        RwV3dTransformPoints(&v, &v, 1, &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, ped->m_apBones[PED_NODE_RIGHT_FOOT]->BoneTag)]);
        v += v1 + CVector(0.0F, 0.0F, -0.1F);
        CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &v, v1.x * 0.26F, v1.y * 0.26F, ped->GetForward().x * 0.14F, ped->GetForward().y * 0.14F, 255, 255, 0, 0, 4.0F,
                                     3000, 1.0F);

        if (ped->m_nDeathTimeMS <= 40) {
            ped->m_nDeathTimeMS = 0;
            ped->bDoBloodyFootprints = false;
        } else
            ped->m_nDeathTimeMS -= 40;
    }
}

// 0x67D7A0
bool CTaskSimpleJump::StartLaunchAnim(CPed* ped) {
    m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_JUMP_LAUNCH);
    if (!m_pAnim)
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_JUMP_LAUNCH_R);
    if (m_pAnim)
        return false;

    if (g_surfaceInfos.IsSteepSlope(ped->m_nContactSurface) && DotProduct(ped->GetForward(), ped->field_578) < 0.0F ||
        ped->IsPlayer() && !CGameLogic::IsPlayerAllowedToGoInThisDirection(ped, ped->GetForward(), 5.0F)) {
        ped->m_pedIK.bSlopePitch = true;
        return false;
    }

    auto moveAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_SPRINT);
    if (!moveAnim || moveAnim->m_BlendAmount < 0.3F)
        moveAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_RUN);
    if (!moveAnim || moveAnim->m_BlendAmount < 0.3F)
        moveAnim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_WALK);

    float fMoveAnimBlendAmount = 0.0F;
    if (moveAnim && moveAnim->m_BlendAmount > 0.3F) {
        fMoveAnimBlendAmount = moveAnim->m_CurrentTime / moveAnim->m_BlendHier->m_fTotalTime + 0.367F;
        if (fMoveAnimBlendAmount > 1.0F)
            fMoveAnimBlendAmount -= 1.0F;
    }

    m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, fMoveAnimBlendAmount >= 0.5F ? ANIM_ID_JUMP_LAUNCH_R : ANIM_ID_JUMP_LAUNCH, 8.0F);

    if (ped->m_pPlayerData)
        m_pAnim->m_Speed = CStats::GetFatAndMuscleModifier(STAT_MOD_2);
    m_pAnim->SetFinishCallback(JumpAnimFinishCB, this);
    ped->m_fAimingRotation = ped->m_fCurrentRotation;

    return true;
}

// 0x67A020
void CTaskSimpleJump::JumpAnimFinishCB(CAnimBlendAssociation* anim, void* data) {
    auto task = reinterpret_cast<CTaskSimpleJump*>(data);
    task->m_bIsFinished = true;
    task->m_pAnim = nullptr;
}
