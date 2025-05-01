#include "StdInc.h"
#include "TaskSimpleCarSlowDragPedOut.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarSlowDragPedOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarSlowDragPedOut, 0x86EEE8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x647FE0);
    RH_ScopedInstall(Destructor, 0x648070);

    RH_ScopedGlobalInstall(FinishAnimCarSlowDragPedOutCB, 0x648180);
    RH_ScopedInstall(ComputeAnimID_Wrapper, 0x648100);
    RH_ScopedInstall(StartAnim, 0x64C010);
    RH_ScopedVMTInstall(Clone, 0x649FD0);
    RH_ScopedVMTInstall(GetTaskType, 0x648060);
    RH_ScopedVMTInstall(MakeAbortable, 0x64BFB0);
    RH_ScopedVMTInstall(ProcessPed, 0x64E060);
    RH_ScopedVMTInstall(SetPedPosition, 0x6480E0);
}

// 0x647FE0
CTaskSimpleCarSlowDragPedOut::CTaskSimpleCarSlowDragPedOut(CVehicle* vehicle, eTargetDoor targetDoor, CTaskUtilityLineUpPedWithCar* lineUpPedWithCarTask, bool isPedStatic) : CTaskSimple() {
    m_bAnimFinished        = false;
    m_AnimAssoc            = nullptr;
    m_Vehicle              = vehicle;
    m_TargetDoor           = targetDoor;
    m_bWasPedStatic        = isPedStatic;
    m_LineUpPedWithCarTask = lineUpPedWithCarTask;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x648070
CTaskSimpleCarSlowDragPedOut::~CTaskSimpleCarSlowDragPedOut() {
    CEntity::SafeCleanUpRef(m_Vehicle);

    if (m_AnimAssoc) {
        m_AnimAssoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x648180
void CTaskSimpleCarSlowDragPedOut::FinishAnimCarSlowDragPedOutCB(CAnimBlendAssociation* anim, void* taskPtr) {
    auto self = (CTaskSimpleCarSlowDragPedOut*)taskPtr; // `this` ptr of the instance that this anim belongs to
    self->m_bAnimFinished = true;
    self->m_AnimAssoc = nullptr;
    if (self->m_Vehicle) {
        // Open door
        const auto [groupId, animId] = self->ComputeAnimID();
        self->m_Vehicle->ProcessOpenDoor(nullptr, self->m_TargetDoor, groupId, animId, 1.f);

        // Mark door as opened (Not sure why it's done here, not in `ProcessOpenDoor`...)
        if (self->m_Vehicle->IsAutomobile()) {
            self->m_Vehicle->AsAutomobile()->m_damageManager.SetDoorOpen((eDoors)self->m_TargetDoor);
        }
    }
}

// 0x648100
void CTaskSimpleCarSlowDragPedOut::ComputeAnimID_Wrapper(AssocGroupId& animGroup, AnimationId& animId) {
    std::tie(animGroup, animId) = ComputeAnimID();
}

// Signature changed
// 0x648100
std::pair<AssocGroupId, AnimationId> CTaskSimpleCarSlowDragPedOut::ComputeAnimID() {
    const auto animationId = [this] {
        switch (m_TargetDoor) {
        case TARGET_DOOR_FRONT_RIGHT:
        case TARGET_DOOR_REAR_RIGHT:
            return ANIM_ID_CAR_PULLOUT_RHS;

        case TARGET_DOOR_DRIVER:
        case TARGET_DOOR_REAR_LEFT:
            return ANIM_ID_CAR_PULLOUT_LHS;

        case TARGET_DOOR_UNK: // TODO: Figure this out, see CVehicleAnimGroup::GetGroup
            return ANIM_ID_UNKNOWN_15;

        default:
            assert(0); // Not reachable
            return ANIM_ID_UNDEFINED;
        }
    }();
    return { m_Vehicle->GetAnimGroup().GetGroup(animationId), animationId};
}

// 0x64C010
void CTaskSimpleCarSlowDragPedOut::StartAnim(const CPed* ped) {
    const auto [groupId, animId] = ComputeAnimID();
    m_AnimAssoc = CAnimManager::BlendAnimation(ped->m_pRwClump, groupId, animId, 1000.f);
    m_AnimAssoc->SetFinishCallback(FinishAnimCarSlowDragPedOutCB, this);
}

/*!
* @notsa
* @brief Return ped in the given seat (basically the ped we'll drag out)
*/
CPed* CTaskSimpleCarSlowDragPedOut::GetJackedPed() const {
    if (m_TargetDoor == TARGET_DOOR_DRIVER) {
        return m_Vehicle->m_pDriver;
    } else {
        return m_Vehicle->m_apPassengers[CCarEnterExit::ComputePassengerIndexFromCarDoor(m_Vehicle, (int32)m_TargetDoor)];
    }
}

// 0x64BFB0


bool CTaskSimpleCarSlowDragPedOut::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const*) {
    if (priority == eAbortPriority::ABORT_PRIORITY_IMMEDIATE) {
        if (m_AnimAssoc) {
            m_AnimAssoc->m_BlendDelta = -1000.f;
        }
        if (m_Vehicle) {
            const auto [groupId, animId] = ComputeAnimID();
            m_Vehicle->ProcessOpenDoor(ped, m_TargetDoor, groupId, animId, 1.f);
        }
        return true;
    }
    return false;
}

// 0x64E060


bool CTaskSimpleCarSlowDragPedOut::ProcessPed(CPed* ped) {
    if (m_bAnimFinished || !m_Vehicle) {
        return true;
    }

    // Make sure we actually need to drag the ped out
    switch (m_Vehicle->GetAnimGroupId()) {
    case ANIM_GROUP_COACHCARANIMS:
    case ANIM_GROUP_TANKCARANIMS:
        return true;
    case ANIM_GROUP_BUSCARANIMS: {
        if (m_TargetDoor == TARGET_DOOR_FRONT_RIGHT) {
            return true;
        }
        break;
    }
    }

    // Play anim and make peds talk (if not already playing)
    if (!m_AnimAssoc) {
        StartAnim(ped);

        // Make peds say something
        if (m_Vehicle) { // Redundant check, maybe inlined?
            if (const auto jackedPed = GetJackedPed()) {
                const auto DoTalk = [this](auto ped1, auto ped2) {
                    if (!SayJacked(ped1, m_Vehicle)) {
                        SayJacking(ped2, ped1, m_Vehicle);
                    }
                };
                if (CGeneral::GetRandomNumber() % 1024 <= 512) {
                    DoTalk(jackedPed, ped);
                } else {
                    DoTalk(ped, jackedPed);
                }
            }
        }
    }

    // Process door open ratio according to animation's current time
    const auto [groupId, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_TargetDoor, groupId, animId, m_AnimAssoc->m_CurrentTime);

    return false;
}

// 0x6480E0

// 0x0
bool CTaskSimpleCarSlowDragPedOut::SetPedPosition(CPed* ped) {
    m_LineUpPedWithCarTask->ProcessPed(ped, m_Vehicle, m_AnimAssoc);
    return true;
}

