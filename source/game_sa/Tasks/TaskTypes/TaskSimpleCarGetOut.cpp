#include "StdInc.h"
#include "TaskSimpleCarGetOut.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarGetOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarGetOut, 0x86ee4c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6471D0);
    RH_ScopedInstall(Destructor, 0x647260);


    RH_ScopedInstall(ComputeAnimID_ToHook, 0x647330);
    RH_ScopedInstall(StartAnim, 0x64BDE0);

    RH_ScopedVMTInstall(Clone, 0x649E70);
    RH_ScopedVMTInstall(GetTaskType, 0x647250);
    RH_ScopedVMTInstall(MakeAbortable, 0x6472D0);
    RH_ScopedVMTInstall(ProcessPed, 0x64DC70);
    RH_ScopedVMTInstall(SetPedPosition, 0x6472F0);
        
    RH_ScopedGlobalInstall(FinishAnimCarGetOutCB, 0x647480);
}

// 0x6471D0
CTaskSimpleCarGetOut::CTaskSimpleCarGetOut(CVehicle* veh, uint32 door, CTaskUtilityLineUpPedWithCar* taskLineUp) :
    m_veh{veh},
    m_door{door},
    m_taskLineUp{taskLineUp}
{
    CEntity::SafeRegisterRef(m_veh);
}

// NOTSA
CTaskSimpleCarGetOut::CTaskSimpleCarGetOut(const CTaskSimpleCarGetOut& o) :
    CTaskSimpleCarGetOut{o.m_veh, o.m_door, o.m_taskLineUp}
{
}

// 0x647260
CTaskSimpleCarGetOut::~CTaskSimpleCarGetOut() {
    CEntity::SafeCleanUpRef(m_veh);

    if (m_anim) {
        m_anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x647480
void CTaskSimpleCarGetOut::FinishAnimCarGetOutCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarGetOut*>(data);
    self->m_finished = true;
    self->m_anim = nullptr;
}

// Wrapper for `0x647330`
void CTaskSimpleCarGetOut::ComputeAnimID_ToHook(AssocGroupId& animGroup, AnimationId& animId) {
    std::tie(animGroup, animId) = ComputeAnimID();
}

// Signature changed, `ComputeAnimID_Wrapper` is hooked instead.
// 0x647330
auto CTaskSimpleCarGetOut::ComputeAnimID() -> std::pair<AssocGroupId, AnimationId> {
    if (m_veh->IsAutomobile() && CUpsideDownCarCheck{}.IsCarUpsideDown(m_veh)) {
        m_isUpsideDown = true;
    }
    
    if (m_isUpsideDown) {
        m_isUpsideDown = [this] {
            const auto& right = m_veh->GetRight();
            switch (m_door) {
            case TARGET_DOOR_DRIVER:
            case TARGET_DOOR_REAR_LEFT:
                return right.z >= -0.5f;
            default:
                return right.z <= 0.5f;
            }
        }();
    }

    if (m_isUpsideDown) {
        return {
            ANIM_GROUP_DEFAULT,
            [this] {
                switch (m_door) {
                case TARGET_DOOR_FRONT_RIGHT:
                case TARGET_DOOR_REAR_RIGHT:
                    return ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_1;

                case TARGET_DOOR_DRIVER:
                case TARGET_DOOR_REAR_LEFT:
                    return ANIM_ID_DEFAULT_CAR_CRAWLOUTRHS_0;
                default:
                    NOTSA_UNREACHABLE();
                }
            }()
        };
    }

    const auto animId = [this] {
        switch (m_door) {
        case TARGET_DOOR_FRONT_RIGHT:
            return ANIM_ID_CAR_GETOUT_RHS_0;
        case TARGET_DOOR_REAR_RIGHT:
            return ANIM_ID_CAR_GETOUT_RHS_1;
        case TARGET_DOOR_DRIVER:
            return ANIM_ID_CAR_GETOUT_LHS_0;
        case TARGET_DOOR_REAR_LEFT:
            return ANIM_ID_CAR_GETOUT_LHS_1;
        default:
            NOTSA_UNREACHABLE();
        }
    }();
    return { m_veh->GetAnimGroup().GetGroup(animId), animId };
}

// 0x64BDE0
void CTaskSimpleCarGetOut::StartAnim(const CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [groupId, animId] = ComputeAnimID();
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, groupId, animId, 1000.f);
    m_anim->SetFinishCallback(FinishAnimCarGetOutCB, this);
}

// 0x6472D0
bool CTaskSimpleCarGetOut::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
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

// 0x64DC70
bool CTaskSimpleCarGetOut::ProcessPed(CPed* ped) {
    if (!m_veh) {
        return false;
    }

    if (m_finished) {
        if (CCheat::IsActive(CHEAT_SMASH_N_BOOM)) {
            m_veh->physicalFlags.bExplosionProof = m_veh->physicalFlags.bBulletProof = false;
            m_veh->vehicleFlags.bCanBeDamaged = true;
        }

        return true;
    }

    if (m_anim) {
        if (m_anim->m_BlendAmount == 1.f) {
            ped->bRenderPedInCar = true;
        }
        
        if (m_vehHasDoorToOpen && !m_isUpsideDown) {
            const auto [grpId, animId] = ComputeAnimID();
            m_veh->ProcessOpenDoor(ped, m_door, grpId, m_anim->m_AnimId, m_anim->m_CurrentTime);
        }
    } else {
        StartAnim(ped);
        ped->SetPedPositionInCar();
        ped->ReplaceWeaponWhenExitingVehicle();
        m_vehHasDoorToOpen = CCarEnterExit::CarHasDoorToOpen(m_veh, (eDoors)m_door);
    }

    return false;
}

// 0x6472F0
bool CTaskSimpleCarGetOut::SetPedPosition(CPed* ped) {
    if (!m_anim || m_anim->m_BlendAmount == 0.f) {
        ped->SetPedPositionInCar();
    } else {
        m_taskLineUp->ProcessPed(ped, m_veh, m_anim);
    }
    return true;
}
