#include "StdInc.h"
#include "TaskSimpleCarSlowBeDraggedOut.h"

// 0x648230
CTaskSimpleCarSlowBeDraggedOut::CTaskSimpleCarSlowBeDraggedOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility, bool isDraggedBySelf) :
    m_Vehicle{ target },
    m_Door{ door },
    m_LineUpUtility{ lineUpUtility },
    m_IsDraggedBySelf{ isDraggedBySelf }
{ }

// 0x6482C0
CTaskSimpleCarSlowBeDraggedOut::~CTaskSimpleCarSlowBeDraggedOut() {
    if (const auto a = std::exchange(m_Anim, nullptr)) {
        a->SetDefaultFinishCallback();
    }
}

// 0x6483A0
void CTaskSimpleCarSlowBeDraggedOut::FinishAnimCarSlowBeDraggedOutCB(CAnimBlendAssociation* pAnim, void* data) {
    const auto self = CTask::Cast<CTaskSimpleCarSlowBeDraggedOut>(static_cast<CTask*>(data));

    self->m_HasAnimFinished = true;
    self->m_Anim            = nullptr;
}

// 0x648330
auto CTaskSimpleCarSlowBeDraggedOut::ComputeAnimID() {
    const auto animId = [&] {
        switch ( m_Door ) {
        case TARGET_DOOR_FRONT_RIGHT:
        case TARGET_DOOR_REAR_RIGHT: return ANIM_ID_CAR_JACKEDRHS;
        case TARGET_DOOR_DRIVER:
        case TARGET_DOOR_REAR_LEFT:  return ANIM_ID_CAR_JACKEDLHS;
        default:                     NOTSA_UNREACHABLE();
        }
    }();
    return std::make_tuple(m_Vehicle->GetAnimGroup().GetGroup(animId), animId);
}

// notsa
void CTaskSimpleCarSlowBeDraggedOut::DoOpenCarDoor(CPed* ped) {
    const auto [animGrp, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_Door, animGrp, animId, 1.f);
}

// 0x64C190
void CTaskSimpleCarSlowBeDraggedOut::StartAnim(CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [animGroup, animId] = ComputeAnimID();
    m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, animGroup, animId, 1000.f);
    m_Anim->SetFinishCallback(FinishAnimCarSlowBeDraggedOutCB, this);
    m_FrontAnim = m_Anim->HasFlag(ANIMATION_IS_FRONT);
}

// 0x64C080
bool CTaskSimpleCarSlowBeDraggedOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (m_Anim) {
        m_Anim->SetBlendDelta(-1000.f);
    }
    if (m_Vehicle) {
        DoOpenCarDoor(ped);
    }
    return true;
}

// 0x64E180
bool CTaskSimpleCarSlowBeDraggedOut::ProcessPed(CPed* ped) {
    if (!m_Vehicle) {
        return true;
    }
    if (m_HasAnimFinished) {
        if (!RpAnimBlendClumpGetAssociation(ped->m_pRwClump, { ANIM_ID_FLOOR_HIT_F, ANIM_ID_FLOOR_HIT })) {
            CAnimManager::BlendAnimation(
                ped->m_pRwClump,
                ANIM_GROUP_DEFAULT,
                m_FrontAnim
                    ? ANIM_ID_FLOOR_HIT_F
                    : ANIM_ID_FLOOR_HIT,
                1000.f
            );
            return false;
        }
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }
    if (!ped->bInVehicle) {
        return true;
    }
    if (m_Anim->GetCurrentTime() > 1.7f) {
        m_LineUpUtility->m_nDoorOpenPosType = 2;
    }
    if (m_IsDraggedBySelf) {
        DoOpenCarDoor(ped);
    }
    return false;
}

// 0x64C0E0
bool CTaskSimpleCarSlowBeDraggedOut::SetPedPosition(CPed* ped) {
    if (m_Vehicle) {
        if (!m_HasAnimFinished && (!m_Anim || m_Anim->GetBlendAmount() == 0.f)) {
            ped->SetPedPositionInCar();
        } else if (m_Anim) {
            m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
        } else { // NOTE: I removed some weird assignment to `m_Anim` that was nulled afterwards... Maybe it was important.
            const auto [animGrp, animId] = ComputeAnimID();
            if (const auto anim = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, animId)) {
                m_LineUpUtility->ProcessPed(ped, m_Vehicle, anim);
            }
        }
    }
    return true;
}

void CTaskSimpleCarSlowBeDraggedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSlowBeDraggedOut, 0x86ef0c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x648230);
    RH_ScopedInstall(Destructor, 0x6482C0);

    RH_ScopedGlobalInstall(FinishAnimCarSlowBeDraggedOutCB, 0x6483A0);

    //RH_ScopedInstall(ComputeAnimID, 0x648330); // cant hook, I've changed the signature
    RH_ScopedInstall(StartAnim, 0x64C190);

    RH_ScopedVMTInstall(Clone, 0x64A040);
    RH_ScopedVMTInstall(GetTaskType, 0x6482B0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64C080);
    RH_ScopedVMTInstall(ProcessPed, 0x64E180);
    RH_ScopedVMTInstall(SetPedPosition, 0x64C0E0);
}
