#include "StdInc.h"
#include "TaskSimpleCarFallOut.h"

// 0x648D60
CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility) :
    m_Vehicle{ target },
    m_Door{ door },
    m_LineUpUtility{ lineUpUtility }
{ }

// 0x648EE0
void CTaskSimpleCarFallOut::FinishAnimFallOutCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = CTask::Cast<CTaskSimpleCarFallOut>(static_cast<CTask*>(data));
    assert(self->m_Anim == anim);

    self->m_HasFinished = true;
    self->m_Anim        = nullptr;

    if (self->m_Vehicle && self->m_Vehicle->IsAutomobile()) {
        self->m_Vehicle->AsAutomobile()->GetDamageManager().SetDoorClosed_Component((tComponent)(self->m_Door));
    }
}

// 0x648E90
auto CTaskSimpleCarFallOut::ComputeAnimID() {
    const auto animId = notsa::contains({ TARGET_DOOR_DRIVER, TARGET_DOOR_REAR_LEFT }, m_Door)
        ? ANIM_ID_CAR_FALLOUT_LHS
        : ANIM_ID_CAR_FALLOUT_RHS;
    return std::make_pair(m_Vehicle->GetAnimGroup().GetGroup(animId), animId);
}

// 0x64CCE0
void CTaskSimpleCarFallOut::StartAnim(const CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [animGrp, animId] = ComputeAnimID();
    m_Anim = CAnimManager::BlendAnimation(ped->m_pRwClump, animGrp, animId, 1000.0f);
    m_Anim->SetFinishCallback(FinishAnimFallOutCB, this);
}

// notsa
void CTaskSimpleCarFallOut::DoOpenCarDoor(CPed* ped) {
    const auto [animGrp, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_Door, animGrp, animId, 1.f);
}

// 0x64CC60
bool CTaskSimpleCarFallOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (const auto a = std::exchange(m_Anim, nullptr)) {
        a->SetBlendDelta(-1000.f);
        a->SetDefaultFinishCallback();
    }
    if (m_Vehicle) {
        DoOpenCarDoor(ped);
    }
    return true;
}

// 0x64E2C0
bool CTaskSimpleCarFallOut::ProcessPed(CPed* ped) {
    if (!m_Vehicle || m_HasFinished) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }
    if (ped->bInVehicle) {
        DoOpenCarDoor(ped);
    }
    return !ped->bInVehicle;
}

// 0x648E40
bool CTaskSimpleCarFallOut::SetPedPosition(CPed* ped) {
    if (m_Vehicle) {
        if (m_HasFinished || (m_Anim && m_Anim->GetBlendAmount() != 0.f)) {
            m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
        } else {
            ped->SetPedPositionInCar();
        }
    }
    return true;
}

void CTaskSimpleCarFallOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarFallOut, 0x86efd0, 9);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x648D60);
    RH_ScopedInstall(Destructor, 0x648DE0);

    RH_ScopedGlobalInstall(FinishAnimFallOutCB, 0x648EE0);

    //RH_ScopedInstall(ComputeAnimID, 0x648E90); // cant hook, changed signature
    RH_ScopedInstall(StartAnim, 0x64CCE0);

    RH_ScopedVMTInstall(Clone, 0x64A280);
    RH_ScopedVMTInstall(GetTaskType, 0x648DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64CC60);
    RH_ScopedVMTInstall(ProcessPed, 0x64E2C0);
    RH_ScopedVMTInstall(SetPedPosition, 0x648E40);
}
