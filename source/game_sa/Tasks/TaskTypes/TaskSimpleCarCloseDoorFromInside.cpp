#include "StdInc.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarCloseDoorFromInside::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarCloseDoorFromInside, 0x86ed08, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646300);
    RH_ScopedInstall(Destructor, 0x646380);

    RH_ScopedGlobalInstall(FinishAnimCarCloseDoorFromInsideCB, 0x646490);

    RH_ScopedInstall(ComputeAnimID, 0x646410);
    RH_ScopedInstall(StartAnim, 0x64AFB0);

    RH_ScopedVMTInstall(Clone, 0x649AF0);
    RH_ScopedVMTInstall(GetTaskType, 0x646370);
    RH_ScopedVMTInstall(MakeAbortable, 0x64AF50);
    RH_ScopedVMTInstall(ProcessPed, 0x64D9A0);
    RH_ScopedVMTInstall(SetPedPosition, 0x6463F0);
}

// 0x646300
CTaskSimpleCarCloseDoorFromInside::CTaskSimpleCarCloseDoorFromInside(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) :
    m_veh{veh},
    m_door{door},
    m_lineUpTask{lineUpTask}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarCloseDoorFromInside::CTaskSimpleCarCloseDoorFromInside(const CTaskSimpleCarCloseDoorFromInside& o) :
    CTaskSimpleCarCloseDoorFromInside{o.m_veh, o.m_door, o.m_lineUpTask}
{
}

// 0x646380
CTaskSimpleCarCloseDoorFromInside::~CTaskSimpleCarCloseDoorFromInside() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x646490
void CTaskSimpleCarCloseDoorFromInside::FinishAnimCarCloseDoorFromInsideCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarCloseDoorFromInside*>(data);

    self->m_animHasFinished = true;

    if (self->m_veh) {
        self->ProcessDoorOpen(nullptr);
    }

    // The anim has just finished and they do this? Maybe inlined?
    if (self->m_anim) {
        self->m_anim->m_BlendDelta = -1000.f;
    }
    self->m_anim = nullptr;
}

// 0x646410
void CTaskSimpleCarCloseDoorFromInside::ComputeAnimID(AssocGroupId& outGroup, AnimationId& outAnimId) {
    outAnimId = [this] {
        switch (m_door) {
        case 8u:  return ANIM_ID_CAR_CLOSEDOOR_RHS_0;
        case 9u:  return ANIM_ID_CAR_CLOSEDOOR_RHS_1;
        case 10u: return ANIM_ID_CAR_CLOSEDOOR_LHS_0;
        case 11u: return ANIM_ID_CAR_CLOSEDOOR_LHS_1;
        default:  NOTSA_UNREACHABLE();
        }
    }();
    outGroup = m_veh->GetAnimGroup().GetGroup(outAnimId);
}

// NOTSA
auto CTaskSimpleCarCloseDoorFromInside::ComputeAnimID_Helper() -> std::tuple<AssocGroupId, AnimationId> {
    AssocGroupId grpId;
    AnimationId  animId;
    ComputeAnimID(grpId, animId);
    return { grpId, animId };
}

// 0x64AFB0
void CTaskSimpleCarCloseDoorFromInside::StartAnim(CPed const* ped) {
    const auto [grpId, animId] = ComputeAnimID_Helper();
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 1000.f);
    m_anim->SetFinishCallback(FinishAnimCarCloseDoorFromInsideCB, this);
}

// NOTSA
void CTaskSimpleCarCloseDoorFromInside::ProcessDoorOpen(CPed const* ped) {
    const auto [grpId, animId] = ComputeAnimID_Helper();
    m_veh->ProcessOpenDoor(
        nullptr,
        m_door,
        grpId,
        animId,
        1.f
    );
}

// 0x64AF50
bool CTaskSimpleCarCloseDoorFromInside::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        if (m_veh) {
            ProcessDoorOpen(ped);
        }
        return true;
    }
    default:
        return false;
    }
}

// 0x64D9A0
bool CTaskSimpleCarCloseDoorFromInside::ProcessPed(CPed* ped) {
    if (m_animHasFinished) {
        return true;
    }

    if (!m_veh) {
        return true;
    }

    if (m_veh->GetAnimGroup().m_specialFlags.bDontCloseDoorAfterGettingIn) {
        ProcessDoorOpen(ped);
        return true;
    }

    if (!m_anim) {
        if (ped->m_nPedState == PEDSTATE_ARRESTED) {
            return true;
        }
        StartAnim(ped);
    }

    const auto animId = (AnimationId)m_anim->m_AnimId;
    m_veh->ProcessOpenDoor(ped, m_door, m_veh->GetAnimGroup().GetGroup(animId), animId, m_anim->m_CurrentTime);

    return false;
}

// 0x6463F0
bool CTaskSimpleCarCloseDoorFromInside::SetPedPosition(CPed* ped) {
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
