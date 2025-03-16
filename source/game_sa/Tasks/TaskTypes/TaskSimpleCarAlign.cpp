#include "StdInc.h"
#include "TaskSimpleCarAlign.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarAlign::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarAlign, 0x86EC78, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x645B40);
    RH_ScopedInstall(Destructor, 0x645BE0);

    RH_ScopedGlobalInstall(FinishAnimCarAlignCB, 0x645E40);

    RH_ScopedInstall(FixHeading, 0x645CB0);
    RH_ScopedInstall(StartAnim, 0x64AC00);

    RH_ScopedVMTInstall(Clone, 0x649930);
    RH_ScopedVMTInstall(GetTaskType, 0x645BD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x645C50);
    RH_ScopedVMTInstall(ProcessPed, 0x64D6D0);
    RH_ScopedVMTInstall(SetPedPosition, 0x645C70);
}

// 0x645B40
CTaskSimpleCarAlign::CTaskSimpleCarAlign(CVehicle* veh, CVector const& doorPos, uint32 targetDoor, CTaskUtilityLineUpPedWithCar* taskLineUp) :
    m_veh{veh},
    m_doorPos{doorPos},
    m_door{targetDoor},
    m_lineUpTask{taskLineUp}
{
    CEntity::SafeRegisterRef(m_veh);
}

// NOTSA
CTaskSimpleCarAlign::CTaskSimpleCarAlign(const CTaskSimpleCarAlign& o) :
    CTaskSimpleCarAlign{o.m_veh, o.m_doorPos, o.m_door, o.m_lineUpTask}
{
}

// 0x645BE0
CTaskSimpleCarAlign::~CTaskSimpleCarAlign() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x645E40
void CTaskSimpleCarAlign::FinishAnimCarAlignCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarAlign*>(data);
    self->m_anim = nullptr;
    self->m_animHasFinished = true;
}

// 0x645CB0
void CTaskSimpleCarAlign::FixHeading(CPed* ped) {
    if (!m_veh || !m_veh->IsAutomobile()) {
        return;
    }

    const auto carLeftOrRight = [this] {
        switch (m_door) {
        case CAR_DOOR_LF:
        case CAR_DOOR_LR:
            return m_veh->GetRight() * -1.f; // Left
        default:
            return m_veh->GetRight();
        }
    }();

    const auto pedFwd = ped->GetForwardVector();
    const auto dot = DotProduct(carLeftOrRight, pedFwd);
    if (dot <= 0.f || dot >= 0.1f) {
        return;
    }

    ped->m_fAimingRotation = ped->m_fCurrentRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(pedFwd - carLeftOrRight * dot, {}));
}

// 0x64AC00
void CTaskSimpleCarAlign::StartAnim(CPed* ped) {
    const auto animId = [this, ped] {
        const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_veh, m_door);
        const auto z = doorOpenPos.z - ped->GetPosition().z;
        switch (m_door) {
        case CAR_DOOR_LF:
        case CAR_DOOR_LR:
            return (z <= 4.4f) ? ANIM_ID_CAR_ALIGN_LHS : ANIM_ID_CAR_ALIGNHI_LHS;
        default:
            return (z <= 4.4f) ? ANIM_ID_CAR_ALIGN_RHS : ANIM_ID_CAR_ALIGNHI_RHS;
        }
    }();
    m_anim = CAnimManager::BlendAnimation(
        ped->m_pRwClump,
        m_veh->GetAnimGroup().GetGroup(animId),
        animId,
        4.f
    );
    m_anim->SetFinishCallback(FinishAnimCarAlignCB, this);
}

// 0x645C50
bool CTaskSimpleCarAlign::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        return true;
    }
    default:
        return false;
    }
}

// 0x64D6D0
bool CTaskSimpleCarAlign::ProcessPed(CPed* ped) {
    if (m_animHasFinished || !m_veh) {
        return true;
    }
    if (!m_anim) {
        FixHeading(ped);
        StartAnim(ped);
    }
    return false;
}

// 0x645C70
bool CTaskSimpleCarAlign::SetPedPosition(CPed* ped) {
    //const auto doorOpenPos = CCarEnterExit::GetPositionToOpenCarDoor(m_veh, m_door); // Result unused
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
