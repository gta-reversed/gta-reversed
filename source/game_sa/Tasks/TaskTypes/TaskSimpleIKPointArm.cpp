#include "StdInc.h"
#include "TaskSimpleIKPointArm.h"
#include "IKChain_c.h"
#include "IKChainManager_c.h"

void CTaskSimpleIKPointArm::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKPointArm, 0x86E3A4, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x634150);
    RH_ScopedInstall(Destructor, 0x634240);
    RH_ScopedInstall(UpdatePointArmInfo, 0x634370);
    RH_ScopedVMTInstall(Clone, 0x634250);
    RH_ScopedVMTInstall(GetTaskType, 0x634230);
    RH_ScopedVMTInstall(CreateIKChain, 0x6342F0);
}

// 0x634150
CTaskSimpleIKPointArm::CTaskSimpleIKPointArm(const char* purpose, eIKArm arm, CEntity* targetEntity, eBoneTag offsetBoneTag, CVector offsetPos, float speed, int32 blendTime) :
    CTaskSimpleIKChain{
        purpose,
        IKArmToBoneTag(arm),
        CVector{ 0.f, 0.05f, 0.f },
        BONE_ROOT,
        targetEntity,
        offsetBoneTag,
        offsetPos,
        speed,
        999'999,
        blendTime
    },
    m_Arm{arm}
{
}

// 0x634250
CTask* CTaskSimpleIKPointArm::Clone() const {
    const auto clone = new CTaskSimpleIKPointArm("", m_Arm, m_TargetEntity, m_OffsetBone, m_OffsetPos, m_Speed, m_BlendDuration);
    if (m_IKChain) {
        clone->m_Blend       = m_Blend;
        clone->m_EndTime     = m_EndTime;
        clone->m_TargetBlend = m_TargetBlend;
        clone->m_TargetTime  = m_TargetTime;
    }
    return clone;
}

// 0x634370
void CTaskSimpleIKPointArm::UpdatePointArmInfo(const char* purpose, CEntity* pointAtEntity, eBoneTag32 offsetBoneTag, CVector offset, float speed, int32 blendDuration) {
    m_TargetEntityExisted = !!pointAtEntity;
    m_TargetEntity        = pointAtEntity;
    m_OffsetBone          = offsetBoneTag;
    m_OffsetPos           = offset;
    m_Speed               = speed;
    m_BlendDuration       = blendDuration;
    m_EndTime             = CTimer::GetTimeInMS() + 999'999;
    m_TargetBlend         = 1.f;
    m_TargetTime          = CTimer::GetTimeInMS() + blendDuration;
    m_IsBlendingOut       = false;

    if (m_IKChain) {
        m_IKChain->UpdateEntity(m_TargetEntity);
        m_IKChain->UpdateOffset(m_OffsetBone, m_OffsetPos);
        m_IKChain->UpdateTarget(true);
    }
}

// 0x6342F0
bool CTaskSimpleIKPointArm::CreateIKChain(CPed* ped) {
    m_PivotBone = BONE_NECK;
    m_IKChain      = g_ikChainMan.AddIKChain("", IKArmToIKSlot(m_Arm), ped, m_EffectorBone, m_EffectorOffset, BONE_NECK, m_TargetEntity, m_OffsetBone, m_OffsetPos, m_Speed, 3);
    return !!m_IKChain;
}
