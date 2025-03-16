#include "StdInc.h"
#include "TaskSimpleCarGetIn.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarGetIn::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarGetIn, 0x86ed50, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646690);
    RH_ScopedInstall(Destructor, 0x646710);

    RH_ScopedInstall(StartAnim, 0x64B2D0);
    RH_ScopedInstall(FinishAnimCarGetInCB, 0x6467F0);

    RH_ScopedVMTInstall(Clone, 0x649BD0);
    RH_ScopedVMTInstall(GetTaskType, 0x646700);
    RH_ScopedVMTInstall(MakeAbortable, 0x646780);
    RH_ScopedVMTInstall(ProcessPed, 0x64DB30);
    RH_ScopedVMTInstall(SetPedPosition, 0x6467A0);
}

// 0x646690
CTaskSimpleCarGetIn::CTaskSimpleCarGetIn(CVehicle* vehicle, uint32 door, CTaskUtilityLineUpPedWithCar* task) :
    m_veh{vehicle},
    m_door{door},
    m_lineUpTask{task}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimpleCarGetIn::CTaskSimpleCarGetIn(const CTaskSimpleCarGetIn& o) :
    CTaskSimpleCarGetIn{ o.m_veh, o.m_door, o.m_lineUpTask }
{
}

// 0x646710
CTaskSimpleCarGetIn::~CTaskSimpleCarGetIn() {
    CEntity::SafeCleanUpRef(m_veh);
}

void CTaskSimpleCarGetIn::FinishAnimCarGetInCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarGetIn*>(data);
    self->m_finished = true;
    self->m_anim = nullptr;
}

// 0x64B2D0
void CTaskSimpleCarGetIn::StartAnim(CPed const* ped) {
    const auto [grpId, animId] = [this] {
        const auto animId = [this]{
            switch (m_door) {
            case TARGET_DOOR_FRONT_RIGHT:
                return ANIM_ID_CAR_GETIN_RHS_0;
            case TARGET_DOOR_REAR_RIGHT:
                return ANIM_ID_CAR_GETIN_RHS_1;
            case TARGET_DOOR_DRIVER:
                return ANIM_ID_CAR_GETIN_LHS_0;
            case TARGET_DOOR_REAR_LEFT:
                return ANIM_ID_CAR_GETIN_LHS_1;
            case TARGET_DOOR_UNK:
                return ANIM_ID_CAR_GETIN_BIKE_FRONT;
            default:
                NOTSA_UNREACHABLE("Invalid door: {}", m_door);
            }
        }();

        const auto grpId = [this, animId]{
            const auto grpId = m_veh->GetAnimGroup().GetGroup(animId);
            if (grpId == AssocGroupId::ANIM_GROUP_CONVCARANIMS) {
                if (m_door == TARGET_DOOR_DRIVER) {
                    if (m_veh->IsDoorMissingU32(TARGET_DOOR_DRIVER) || !m_veh->IsDoorClosedU32(TARGET_DOOR_DRIVER)) {
                        return ANIM_GROUP_STDCARAMIMS;
                    }
                }
            }
            return grpId;
        }();
        
        return std::make_tuple(grpId, animId);
    }();
    
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, grpId, animId, 4.f);
    m_anim->SetFinishCallback(FinishAnimCarGetInCB, this);
    
    /*
    m_anim->SetFinishCallback([](CAnimBlendAssociation* anim, void* data) {
        const auto self = static_cast<CTaskSimpleCarGetIn*>(data);
        self->m_finished = true;
        self->m_anim = nullptr;
    }, this);
    */
}

// 0x646780
bool CTaskSimpleCarGetIn::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    switch (priority) {
    case ABORT_PRIORITY_IMMEDIATE: {
        if (m_anim) {
            m_anim->m_BlendDelta = -1000.f;
        }
        return true;
    }
    }
    return false;
}

// 0x64DB30
bool CTaskSimpleCarGetIn::ProcessPed(CPed* ped) {
    if (!m_veh) {
        return true;
    }

    if (m_finished) {
        ped->RemoveWeaponWhenEnteringVehicle(false);
        if (CCheat::IsActive(CHEAT_SMASH_N_BOOM) && ped->IsPlayer() && m_veh) { // This stuff later is de-activated in `CTaskSimpleCarGetOut`
             m_veh->physicalFlags.bBulletProof = m_veh->physicalFlags.bExplosionProof = true;
             m_veh->vehicleFlags.bCanBeDamaged = false;
        }
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
    }

    // Apply move force for BMX like vehicles
    if (m_veh && m_veh->IsSubBMX() && m_anim && (m_anim->m_Flags & ANIMATION_IS_PLAYING)) {
        switch (m_anim->m_AnimId) {
        case ANIM_ID_CAR_GETIN_LHS_0:
        case ANIM_ID_CAR_GETIN_RHS_0:
        case ANIM_ID_CAR_GETIN_LHS_1:
        case ANIM_ID_CAR_GETIN_RHS_1: {
            constexpr auto flt_8D2ED4 = 30.f; // TODO | STATICREF: 0x8D2ED4
            if (flt_8D2ED4 / 30.f < m_anim->m_CurrentTime) { 
                constexpr auto flt_8D2ED0 = 0.001f; // TODO | STATICREF: 0x8D2ED0
                m_veh->ApplyMoveForce(m_veh->GetForward() * CTimer::GetTimeStep() * m_veh->m_fMass * flt_8D2ED0);
            }
            break;
        }
        }
    }


    return false;
}

// 0x6467A0
bool CTaskSimpleCarGetIn::SetPedPosition(CPed* ped) {
    m_lineUpTask->ProcessPed(ped, m_veh, m_anim);
    return true;
}
