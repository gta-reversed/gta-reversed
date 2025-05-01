#include "StdInc.h"

#include "TaskSimplePutDownEntity.h"

void CTaskSimplePutDownEntity::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePutDownEntity, 0x870b74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x691990);

    RH_ScopedVMTInstall(Clone, 0x692B70);
    RH_ScopedVMTInstall(GetTaskType, 0x691900);
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity() : CTaskSimpleHoldEntity(nullptr, nullptr, PED_NODE_RIGHT_HAND, HOLD_ENTITY_FLAG_1, ANIM_ID_NO_ANIMATION_SET, ANIM_GROUP_DEFAULT, false) {
    m_fPutDownHeightZ = 0.6f;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, AnimationId animId, AssocGroupId groupId, bool bDisAllowDroppingOnAnimEnd, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animId, groupId, bDisAllowDroppingOnAnimEnd)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, const char* animName, const char* animBlockName, eAnimationFlags animFlags, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, animName, animBlockName, animFlags)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

CTaskSimplePutDownEntity::CTaskSimplePutDownEntity(CEntity* entityToHold, const CVector* posn, uint8 boneFrameId, uint8 boneFlags, CAnimBlock* pAnimBlock, CAnimBlendHierarchy* animHierarchy, eAnimationFlags animFlags, float fPutDownHeightZ)
    : CTaskSimpleHoldEntity(entityToHold, posn, boneFrameId, boneFlags, pAnimBlock, animHierarchy, animFlags)
{
    m_fPutDownHeightZ = fPutDownHeightZ;
}

// 0x692B70
CTask* CTaskSimplePutDownEntity::Clone() const {
    return m_pAnimBlendHierarchy
        ? new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_pAnimBlock,
            m_pAnimBlendHierarchy,
            static_cast<eAnimationFlags>(m_animFlags),
            m_fPutDownHeightZ
        )
        : new CTaskSimplePutDownEntity(
            m_pEntityToHold,
            &m_vecPosition,
            m_nBoneFrameId,
            m_bBoneFlags,
            m_nAnimId,
            m_nAnimGroupId,
            false,
            m_fPutDownHeightZ
        );
}
