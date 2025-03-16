#include "StdInc.h"

#include "TaskSimplePlayHandSignalAnim.h"

#include "IKChainManager_c.h"

void CTaskSimplePlayHandSignalAnim::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePlayHandSignalAnim, 0x86D5B8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(ProcessPed, 0x61BDA0);
    RH_ScopedInstall(StartAnim, 0x61AF60);
}

// 0x61AE50
CTaskSimplePlayHandSignalAnim::CTaskSimplePlayHandSignalAnim(AnimationId animationId, float fBlendFactor, bool bFatHands, bool bHoldLastFrame) : CTaskSimpleAnim(bHoldLastFrame) {
    m_PlayerIdlesAnimBlockId = animationId;
    m_LeftHand = nullptr;
    m_RightHand = nullptr;
    m_AnimBlenDelta = fBlendFactor;
    m_DoUseFatHands = bFatHands;
}

CTaskSimplePlayHandSignalAnim::~CTaskSimplePlayHandSignalAnim() {
    if (m_LeftHand) {
        CWorld::Remove(m_LeftHand);
        delete m_LeftHand;
        --CObject::nNoTempObjects;
    }

    if (m_RightHand) {
        CWorld::Remove(m_RightHand);
        delete m_RightHand;
        --CObject::nNoTempObjects;
    }
}

// 0x61BDA0
bool CTaskSimplePlayHandSignalAnim::ProcessPed(CPed* ped) {
    if (g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_LEFT, ped) || m_bIsFinished) {
        return true;
    }

    if (!m_pAnim) {
        if (CObject::nNoTempObjects + 1 >= 150) {
            return true;
        }

        CTaskSimplePlayHandSignalAnim::StartAnim(ped);
    }

    return m_bIsFinished;
}

// 0x61AF60
void CTaskSimplePlayHandSignalAnim::StartAnim(CPed* ped) {
    const auto animId = static_cast<AnimationId>(CGeneral::GetRandomNumberInRange((int32)ANIM_ID_GSIGN1, (int32)ANIM_ID_GSIGN5 + 1));

    //
    // Pointing / weapon logic
    //
    if (ped->GetEntityThatThisPedIsHolding() || g_ikChainMan.IsArmPointing(eIKArm::IK_ARM_RIGHT, ped) || ped->GetActiveWeapon().m_Type != eWeaponType::WEAPON_UNARMED) {
        m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_HANDSIGNALL, animId, m_AnimBlenDelta);
        m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);

        if (m_PlayerIdlesAnimBlockId == -1) {
            return;
        }

        const auto handIndex = m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL;
        m_LeftHand    = new CHandObject(handIndex, ped, true);
        if (!m_LeftHand) {
            return;
        }

        CWorld::Add(m_LeftHand);
        auto* animHierarchy = CAnimManager::GetAnimAssociation(ANIM_GROUP_LHAND, m_PlayerIdlesAnimBlockId)->m_BlendHier;
        CAnimManager::AddAnimation(m_LeftHand->m_pRwClump, animHierarchy, 0);
        ++CObject::nNoTempObjects;
        return;
    }

    //
    // Both arms logic
    //
    m_pAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_HANDSIGNAL, animId, m_AnimBlenDelta);
    m_pAnim->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, this);
    if (m_PlayerIdlesAnimBlockId == -1) {
        return;
    }

    const auto CreateHandObject = [ped, this](eModelID handModel, AssocGroupId animGrpId, bool isLeftHand) {
        const auto handObj = new CHandObject(handModel, ped, isLeftHand);
        if (handObj) {
            CWorld::Add(handObj);
            CAnimManager::AddAnimation(
                handObj->m_pRwClump,
                CAnimManager::GetAnimAssociation(ANIM_GROUP_LHAND, m_PlayerIdlesAnimBlockId)->GetAnimHierarchy(),
                0
            );
            CObject::nNoTempObjects++;
        }
        return handObj;
    };
    m_LeftHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDL : MODEL_SHANDL, ANIM_GROUP_LHAND, true);
    m_RightHand = CreateHandObject(m_DoUseFatHands ? MODEL_FHANDR : MODEL_SHANDR, ANIM_GROUP_RHAND, false);
}
