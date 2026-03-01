#include "StdInc.h"
#include "TaskSimpleCarCloseDoorFromOutside.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "PedPlacement.h"

void CTaskSimpleCarCloseDoorFromOutside::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarCloseDoorFromOutside, 0x86ed2c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6464F0);
    RH_ScopedInstall(Destructor, 0x646570);

    RH_ScopedInstall(FinishAnimCarCloseDoorFromOutsideCB, 0x646680);

    RH_ScopedInstall(ComputeAnimID, 0x646600);
    RH_ScopedInstall(StartAnim, 0x64B080);

    RH_ScopedVMTInstall(Clone, 0x649B60);
    RH_ScopedVMTInstall(GetTaskType, 0x646560);
    RH_ScopedVMTInstall(MakeAbortable, 0x64B020);
    RH_ScopedVMTInstall(ProcessPed, 0x64DA90);
    RH_ScopedVMTInstall(SetPedPosition, 0x6465E0);
}

// 0x6464F0
CTaskSimpleCarCloseDoorFromOutside::CTaskSimpleCarCloseDoorFromOutside(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* lineUpTask) :
    m_veh{ veh },
    m_door{ door },
    m_lineUpTask{ lineUpTask }
{
    CEntity::SafeRegisterRef(m_veh);
}

// NOTSA
CTaskSimpleCarCloseDoorFromOutside::CTaskSimpleCarCloseDoorFromOutside(const CTaskSimpleCarCloseDoorFromOutside& o) :
    CTaskSimpleCarCloseDoorFromOutside{ o.m_veh, o.m_door, o.m_lineUpTask }
{
}

// 0x646570
CTaskSimpleCarCloseDoorFromOutside::~CTaskSimpleCarCloseDoorFromOutside() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x646680
void CTaskSimpleCarCloseDoorFromOutside::FinishAnimCarCloseDoorFromOutsideCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarCloseDoorFromOutside*>(data);

    self->m_animHasFinished = true;
    self->m_anim = nullptr;
}

// 0x646600
void CTaskSimpleCarCloseDoorFromOutside::ComputeAnimID(AssocGroupId& outGroup, AnimationId& outAnimId) {
    outAnimId = [this] {
        switch (m_door) {
        case 8u:  return ANIM_ID_CAR_CLOSE_RHS_0;
        case 9u:  return ANIM_ID_CAR_CLOSE_RHS_1;
        case 10u: return ANIM_ID_CAR_CLOSE_LHS_0;
        case 11u: return ANIM_ID_CAR_CLOSE_LHS_1;
        default:  NOTSA_UNREACHABLE();
        }
    }();
    outGroup = m_veh->GetAnimGroup().GetGroup(outAnimId);
}

// NOTSA
auto CTaskSimpleCarCloseDoorFromOutside::ComputeAnimID_Helper() -> std::tuple<AssocGroupId, AnimationId> {
    AssocGroupId grpId;
    AnimationId  animId;
    ComputeAnimID(grpId, animId);
    return { grpId, animId };
}

// 0x64B080
void CTaskSimpleCarCloseDoorFromOutside::StartAnim(CPed const* ped) {
    const auto& pedPos = ped->GetPosition();

    auto pedAdjustedPos{pedPos};
    CPedPlacement::FindZCoorForPed(pedAdjustedPos);
    
    if (pedPos.z - 1.f >= pedAdjustedPos.z) {
        m_animHasFinished = true;
        return;
    }

    m_veh->m_nFakePhysics = false;
    m_veh->m_vecMoveSpeed += CVector{ 0.001f, 0.001f, 0.001f };
    m_veh->m_vecTurnSpeed += CVector{ 0.001f, 0.001f, 0.001f };


    if ([this, ped] {
        if (ped->IsPlayer()) {
            const auto player = static_cast<const CPlayerPed*>(ped);
            const auto pad    = player->GetPadFromPlayer();

            if (!pad->DisablePlayerControls && m_veh->m_nDoorLock != CARLOCK_FORCE_SHUT_DOORS) {
                if (   pad->GetTarget()
                    || pad->GetLeftStickX() || pad->GetLeftStickY()
                    || pad->NewState.DPadUp || pad->NewState.DPadDown
                    || pad->NewState.DPadLeft || pad->NewState.DPadRight
                ) {
                    return true;
                }
            }

            if (m_veh->m_pFire || m_veh->IsAutomobile() && m_veh->AsAutomobile()->m_damageManager.GetEngineStatus() >= 225) {
                return true;
            }
        }
        return false;
    }()) {
        if (m_veh->IsAutomobile()) {
            m_veh->AsAutomobile()->m_damageManager.SetDoorOpen((eDoors)m_door);
        }
        m_animHasFinished = true;
    } else {
        const auto [grpId, animId] = ComputeAnimID_Helper();
        m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 1000.f);
        m_anim->SetFinishCallback(FinishAnimCarCloseDoorFromOutsideCB, this);
    }
}

// NOTSA
void CTaskSimpleCarCloseDoorFromOutside::ProcessDoorOpen(CPed const* ped) {
    const auto [grpId, animId] = ComputeAnimID_Helper();
    m_veh->ProcessOpenDoor(
        nullptr,
        m_door,
        grpId,
        animId,
        1.f
    );
}

// 0x64B020
bool CTaskSimpleCarCloseDoorFromOutside::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
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

// 0x64DA90
bool CTaskSimpleCarCloseDoorFromOutside::ProcessPed(CPed* ped) {
    if (m_animHasFinished) {
        return true;
    }

    if (!m_veh || m_veh->GetAnimGroup().m_specialFlags.bDontCloseDoorAfterGettingOut) {
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
        if (!m_anim) {
            return true;
        }
    }

    const auto [grpId, animId] = ComputeAnimID_Helper();
    m_veh->ProcessOpenDoor(ped, m_door, grpId, animId, m_anim->m_CurrentTime);

    return false;
}

// 0x6465E0
bool CTaskSimpleCarCloseDoorFromOutside::SetPedPosition(CPed* ped) {
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
