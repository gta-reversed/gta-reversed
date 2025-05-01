#include "StdInc.h"
#include "TaskSimplePickUpBike.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimplePickUpBike::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimplePickUpBike, 0x86ece4, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6461B0);
    RH_ScopedInstall(Destructor, 0x646230);

    RH_ScopedInstall(FinishAnimPickUpBikeCB, 0x6462F0);
    RH_ScopedInstall(StartAnim, 0x64AE90);

    RH_ScopedVMTInstall(Clone, 0x649A80);
    RH_ScopedVMTInstall(GetTaskType, 0x646220);
    RH_ScopedVMTInstall(MakeAbortable, 0x6462A0);
    RH_ScopedVMTInstall(ProcessPed, 0x64D910);
    RH_ScopedVMTInstall(SetPedPosition, 0x6462D0);
}

// 0x6461B0
CTaskSimplePickUpBike::CTaskSimplePickUpBike(CVehicle* veh, int32 targetDoor, CTaskUtilityLineUpPedWithCar* lineUpUtil) :
    m_targetDoor{targetDoor},
    m_veh{veh},
    m_lineUpUtil{lineUpUtil}
{
    CEntity::SafeRegisterRef(m_veh);
}

CTaskSimplePickUpBike::CTaskSimplePickUpBike(const CTaskSimplePickUpBike& o) :
    CTaskSimplePickUpBike{o.m_veh, o.m_targetDoor, o.m_lineUpUtil}
{
}

// 0x646230
CTaskSimplePickUpBike::~CTaskSimplePickUpBike() {
    CEntity::SafeCleanUpRef(m_veh);
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

// 0x6462F0
void CTaskSimplePickUpBike::FinishAnimPickUpBikeCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimplePickUpBike*>(data);
    self->m_anim = nullptr;
    self->m_hasAnimFinished = true;
}

// 0x64AE90
void CTaskSimplePickUpBike::StartAnim(CPed const* ped) {
    const auto animationId = [this] {
        const auto rightZ = m_veh->GetRight().z;
        switch ((eTargetDoor)m_targetDoor) {
        case TARGET_DOOR_DRIVER:
        case TARGET_DOOR_REAR_LEFT:
            return rightZ <= 0.f ? ANIM_ID_CAR_ALIGNHI_LHS : ANIM_ID_CAR_ALIGN_LHS;
        default:
            return rightZ >= 0.f ? ANIM_ID_CAR_ALIGNHI_RHS : ANIM_ID_CAR_ALIGN_RHS;
        }
    }();
    m_anim = CAnimManager::BlendAnimation(ped->m_pRwClump, m_veh->GetAnimGroup().GetGroup(animationId), animationId);
    m_anim->SetFinishCallback(FinishAnimPickUpBikeCB, this);
}

// 0x6462A0
bool CTaskSimplePickUpBike::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }

    if (m_anim) {
        m_anim->m_BlendDelta = -1000.f;
    }
    m_veh->AsBike()->bikeFlags.bGettingPickedUp = false;
    return true;
}

// 0x64D910
bool CTaskSimplePickUpBike::ProcessPed(CPed* ped) {
    if (m_hasAnimFinished) {
        return true;
    }

    if (!m_anim) {
        StartAnim(ped);
    }

    if (const auto bike = m_veh->AsBike(); !bike->bikeFlags.bGettingPickedUp) {
        if (m_anim) {
            if (m_anim->m_CurrentTime > 0.47f) { // Simplified...
                bike->bikeFlags.bGettingPickedUp = true;
            }        
        }        
    }

    return false;
}

// 0x6462D0
bool CTaskSimplePickUpBike::SetPedPosition(CPed* ped) {
    m_lineUpUtil->ProcessPed(ped, m_veh, m_anim);
    return true;
}
