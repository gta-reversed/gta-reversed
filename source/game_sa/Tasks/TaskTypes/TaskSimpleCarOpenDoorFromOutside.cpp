#include "StdInc.h"
#include "TaskSimpleCarOpenDoorFromOutside.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarOpenDoorFromOutside::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarOpenDoorFromOutside, 0x86ec9c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x645E50);
    RH_ScopedInstall(Destructor, 0x645EE0);

    RH_ScopedInstall(FinishAnimCarOpenDoorFromOutsideCB, 0x646020);
    RH_ScopedInstall(ComputeAnimID, 0x645FA0);
    RH_ScopedInstall(StartAnim, 0x64AD90);

    RH_ScopedVMTInstall(Clone, 0x6499A0);
    RH_ScopedVMTInstall(GetTaskType, 0x645ED0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64AD00);
    RH_ScopedVMTInstall(ProcessPed, 0x64D710);
    RH_ScopedVMTInstall(SetPedPosition, 0x645F80);
}

// 0x645E50
CTaskSimpleCarOpenDoorFromOutside::CTaskSimpleCarOpenDoorFromOutside(CVehicle* veh, int32 door, CTaskUtilityLineUpPedWithCar* lineUpTask, bool disallowPlayerDriverToExitCar) :
    m_veh{veh},
    m_door{door},
    m_disallowPlayerDriverToExitCar{ disallowPlayerDriverToExitCar },
    m_lineUpTask{lineUpTask}
{
    CEntity::SafeRegisterRef(veh);
}

// NOTSA
CTaskSimpleCarOpenDoorFromOutside::CTaskSimpleCarOpenDoorFromOutside(const CTaskSimpleCarOpenDoorFromOutside& o) :
    CTaskSimpleCarOpenDoorFromOutside{o.m_veh, o.m_door, o.m_lineUpTask, o.m_disallowPlayerDriverToExitCar}
{
}

// 0x645EE0
CTaskSimpleCarOpenDoorFromOutside::~CTaskSimpleCarOpenDoorFromOutside() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
    IfNotAlreadySetPlayerCanExitCarFlag();
}

// NOTSA?
void CTaskSimpleCarOpenDoorFromOutside::IfNotAlreadySetPlayerCanExitCarFlag() {
    if (m_hasSetPlayerCanExitCarFlag) {
        m_hasSetPlayerCanExitCarFlag = false;
        FindPlayerPed()->bCanExitCar = true;
    }
}

// 0x646020
void CTaskSimpleCarOpenDoorFromOutside::FinishAnimCarOpenDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarOpenDoorFromOutside*>(data);
    self->m_anim = nullptr;
    self->m_finished = true;
    
}

// Originally @ 0x645FA0, but we've modified it a little
auto CTaskSimpleCarOpenDoorFromOutside::ComputeAnimID() -> std::pair<AssocGroupId, AnimationId> {
    const auto animId = [this] {
        switch (m_door) {
        case TARGET_DOOR_FRONT_RIGHT:
            return ANIM_ID_CAR_OPEN_RHS;
        case TARGET_DOOR_REAR_RIGHT:
            return ANIM_ID_CAR_OPEN_RHS_1;
        case TARGET_DOOR_DRIVER:
            return ANIM_ID_CAR_OPEN_LHS;
        case TARGET_DOOR_REAR_LEFT:
            return ANIM_ID_CAR_OPEN_LHS_1;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
    return { m_veh->GetAnimGroup().GetGroup(animId), animId };
}

// 0x645FA0
void CTaskSimpleCarOpenDoorFromOutside::ComputeAnimID_ToHook(AssocGroupId& grp, AnimationId& id) {
    std::tie(grp, id) = ComputeAnimID();
}

// 0x64AD90
void CTaskSimpleCarOpenDoorFromOutside::StartAnim(CPed* ped) {
    const auto [groupId, animId] = ComputeAnimID();
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, groupId, animId, 1000.f);
    m_anim->SetFinishCallback(FinishAnimCarOpenDoorFromOutsideCB, this);
}

// 0x64AD00
bool CTaskSimpleCarOpenDoorFromOutside::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        if (m_veh) {
            const auto [groupId, animId] = ComputeAnimID();
            m_veh->ProcessOpenDoor(ped, m_door, groupId, animId, 1.f);
        }
        IfNotAlreadySetPlayerCanExitCarFlag();
        return true;
    }
    default:
        return false;
    }
}

// 0x64D710
bool CTaskSimpleCarOpenDoorFromOutside::ProcessPed(CPed* ped) {
    if (m_finished) {
        return true;
    }

    if (!m_veh) {
        return true;
    }

    if (m_veh->GetAnimGroup().m_specialFlags.bDontOpenDoorWhenGettingIn) {
        const auto [groupId, animId] = ComputeAnimID();
        m_veh->ProcessOpenDoor(ped, m_door, groupId, animId, 1.f);
        return true;
    }

    if (!m_anim) {
        m_doorOpenAngleRatio = m_veh->GetDooorAngleOpenRatioU32(m_door);
        StartAnim(ped);

        if (m_veh && m_veh->IsDriverAPlayer() && m_disallowPlayerDriverToExitCar) { // NOTE/TODO: Inlined? Double check if `m_veh`
            if (ped && ped->IsCop()) {
#ifdef FIX_BUGS
                FindPlayerPed()->bCanExitCar = false; // See `IfNotAlreadySetPlayerCanExitCarFlag`
#else
                m_veh->m_pDriver->bCanExitCar = false; 
#endif
                m_hasSetPlayerCanExitCarFlag = true;
            }
        }
    }

    const auto doorOpenTiming = m_veh->GetAnimGroup().GetInOutTiming(TIMING_START)[OPEN_START];
    if (m_veh->IsAutomobile() || m_anim->m_CurrentTime >= doorOpenTiming) {
        const auto [groupId, animId] = ComputeAnimID();
        m_veh->ProcessOpenDoor(ped, m_door, groupId, m_anim->m_AnimId, m_anim->m_CurrentTime);        
    } else {
        // Open the door visually
        const auto doorId = [this] {
            switch (m_door)
            {
            case TARGET_DOOR_FRONT_RIGHT:
                return DOOR_RIGHT_FRONT;
            case TARGET_DOOR_REAR_RIGHT:
                return DOOR_RIGHT_REAR;
            case TARGET_DOOR_DRIVER:
                return DOOR_LEFT_FRONT;
            case TARGET_DOOR_REAR_LEFT:
                return DOOR_LEFT_REAR;
            default:
                NOTSA_UNREACHABLE();
            }
        }();
        m_veh->OpenDoor(ped, m_door, doorId, (1.f - m_anim->m_CurrentTime / doorOpenTiming) * m_doorOpenAngleRatio, false);
    }

    return false;
}

// 0x645F80
bool CTaskSimpleCarOpenDoorFromOutside::SetPedPosition(CPed* ped) {
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
