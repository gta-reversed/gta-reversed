#include "StdInc.h"
#include "TaskSimpleCarOpenLockedDoorFromOutside.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarOpenLockedDoorFromOutside::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarOpenLockedDoorFromOutside, 0x86ecc0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646070);
    RH_ScopedInstall(Destructor, 0x6460F0);

    RH_ScopedInstall(FinishAnimCarOpenLockedDoorFromOutsideCB, 0x6461A0);
    RH_ScopedInstall(StartAnim, 0x64AE00);

    RH_ScopedVMTInstall(Clone, 0x649A10);
    RH_ScopedVMTInstall(GetTaskType, 0x6460E0);
    RH_ScopedVMTInstall(MakeAbortable, 0x646160);
    RH_ScopedVMTInstall(ProcessPed, 0x64D8E0);
    RH_ScopedVMTInstall(SetPedPosition, 0x646180);

}

// 0x646070
CTaskSimpleCarOpenLockedDoorFromOutside::CTaskSimpleCarOpenLockedDoorFromOutside(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpUtil) :
    m_lineUpUtil{lineUpUtil},
    m_veh{veh},
    m_door{door}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarOpenLockedDoorFromOutside::CTaskSimpleCarOpenLockedDoorFromOutside(const CTaskSimpleCarOpenLockedDoorFromOutside& o) :
    CTaskSimpleCarOpenLockedDoorFromOutside{ o.m_veh, o.m_door, o.m_lineUpUtil }
{
}

// 0x6460F0
CTaskSimpleCarOpenLockedDoorFromOutside::~CTaskSimpleCarOpenLockedDoorFromOutside() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x6461A0
void CTaskSimpleCarOpenLockedDoorFromOutside::FinishAnimCarOpenLockedDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarOpenLockedDoorFromOutside*>(data);
    self->m_anim = nullptr;
    self->m_hasAnimFinished = true;
}

// 0x64AE00
void CTaskSimpleCarOpenLockedDoorFromOutside::StartAnim(CPed* ped) {
    const auto animationId = [this] {
        switch ((eTargetDoor)m_door) {
        case TARGET_DOOR_DRIVER:
            return ANIM_ID_CAR_DOORLOCKED_LHS;
        default: // TODO: Perhaps add more cases so R/L handside anims are used properly?
            return ANIM_ID_CAR_DOORLOCKED_RHS;
        }
    }();
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_veh->GetAnimGroup().GetGroup(animationId), animationId, 4.f);
    m_anim->SetFinishCallback(FinishAnimCarOpenLockedDoorFromOutsideCB, this);
}

// 0x646160
bool CTaskSimpleCarOpenLockedDoorFromOutside::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }

    if (m_anim) {
        m_anim->m_BlendDelta = -1000.f;
    }

    return true;
}

// 0x64D8E0
bool CTaskSimpleCarOpenLockedDoorFromOutside::ProcessPed(CPed* ped) {
    if (m_hasAnimFinished) {
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
    }

    return false;
}

// 0x646180
bool CTaskSimpleCarOpenLockedDoorFromOutside::SetPedPosition(CPed* ped) {
    m_lineUpUtil->ProcessPed(ped, m_veh, m_anim);
    return true;
}
