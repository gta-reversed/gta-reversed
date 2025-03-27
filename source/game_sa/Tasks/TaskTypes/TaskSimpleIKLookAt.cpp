#include "StdInc.h"
#include "IKChainManager_c.h"
#include "TaskSimpleIKLookAt.h"

void CTaskSimpleIKLookAt::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleIKLookAt, 0x86E37C, 10);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x633E00);
    RH_ScopedOverloadedInstall(Destructor, "", 0x633EF0, CTaskSimpleIKLookAt * (CTaskSimpleIKLookAt::*)());

    RH_ScopedInstall(UpdateLookAtInfo, 0x634050);
    RH_ScopedInstall(GetLookAtEntity, 0x634120);
    RH_ScopedInstall(GetLookAtOffset, 0x634130);
    RH_ScopedVMTInstall(Clone, 0x633F00);
    RH_ScopedVMTInstall(GetTaskType, 0x633EE0);
    RH_ScopedVMTInstall(CreateIKChain, 0x633FC0);
}

// 0x633E00
CTaskSimpleIKLookAt::CTaskSimpleIKLookAt(Const char* name, CEntity* lookAtEntity, int32 time, eBoneTag pedBoneID, CVector lookAtOffset, bool useTorso, float speed,
                                         int32 blendTime, int8 priority
)
    : CTaskSimpleIKChain{name, BONE_HEAD, { 0.f, 0.05f, 0.f }, BONE_ROOT, lookAtEntity, pedBoneID, lookAtOffset, speed, time, blendTime}
{
    m_nPriority = priority;
    m_bUseTorso = useTorso;
}

// 0x633F00


CTaskSimpleIKLookAt* CTaskSimpleIKLookAt::Clone() const {
    auto* task = new CTaskSimpleIKLookAt("", m_TargetEntity, m_Duration, m_OffsetBone, m_OffsetPos, m_bUseTorso, m_Speed, m_BlendDuration, m_nPriority);
    if (m_IKChain) {
        task->m_Blend        = m_Blend;
        task->m_EndTime      = m_EndTime;
        task->m_TargetBlend  = m_TargetBlend;
        task->m_TargetTime   = m_TargetTime;
        task->m_PivotBone = m_PivotBone;
    }
    return task;
}

// 0x634050
void CTaskSimpleIKLookAt::UpdateLookAtInfo(const char* strPurpose, CPed* ped, CEntity* targetPed, int32 time, eBoneTag pedBoneID, RwV3d lookAtOffset, bool useTorso, float fSpeed,
                                           int32 blendTime, int32 unused) {
    m_TargetEntityExisted = !!targetPed;
    m_TargetEntity        = targetPed;
    m_OffsetBone          = pedBoneID;
    m_OffsetPos           = lookAtOffset;
    m_bUseTorso           = useTorso;
    m_Duration            = time;
    m_Speed               = fSpeed;
    m_BlendDuration       = blendTime;
    m_EndTime             = CTimer::GetTimeInMS() + time;
    m_TargetBlend         = 1.0f;
    m_TargetTime          = CTimer::GetTimeInMS() + blendTime;
    m_IsBlendingOut       = false;

    if (m_IKChain) {
        m_IKChain->UpdateEntity(m_TargetEntity);
        m_IKChain->UpdateOffset(m_OffsetBone, m_OffsetPos);
        m_IKChain->UpdateTarget(true);
    }
}

// 0x634120
CEntity* CTaskSimpleIKLookAt::GetLookAtEntity() {
    return m_TargetEntity;
}

// 0x634130
CVector CTaskSimpleIKLookAt::GetLookAtOffset() { // We return a vector here.. Hopefully this is ABI compatible.
    return m_OffsetPos;
}

// 0x633FC0

// 0x0
bool CTaskSimpleIKLookAt::CreateIKChain(CPed* ped) {
    m_PivotBone = BONE_NECK;
    if (m_IKChain = g_ikChainMan.AddIKChain(
        "",
        eIKChainSlot::LOOK_AT,
        ped,
        m_EffectorBone,
        m_EffectorOffset,
        BONE_NECK,
        m_TargetEntity,
        m_OffsetBone,
        m_OffsetPos,
        m_Speed,
        m_nPriority)
    ) {
        m_IKChain->ClampLimits(BONE_HEAD, false, true, false, true);
        return true;
    }
    return false;
}

