#include "StdInc.h"

#include "EventDamage.h"
#include "TaskSimpleEvasiveDive.h"

// 0x653560
CTaskSimpleEvasiveDive::CTaskSimpleEvasiveDive(CVehicle* vehicle) :
    m_ToEvade{ vehicle } {
}

// 0x6535D0
CTaskSimpleEvasiveDive::~CTaskSimpleEvasiveDive() {
    if (m_DiveAnim) {
        m_DiveAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x6536A0
void CTaskSimpleEvasiveDive::FinishAnimEvasiveDiveCB(CAnimBlendAssociation* assoc, void* data) {
    const auto self   = notsa::cast<CTaskSimpleEvasiveDive>(static_cast<CTask*>(data));

    self->m_bFinished = true;
    self->m_DiveAnim  = nullptr;
}

// 0x653640
bool CTaskSimpleEvasiveDive::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_DiveAnim) {
            m_DiveAnim->SetBlendDelta(-1000.f);
        }
        return true;
    }
    if (const auto eDmg = notsa::dyn_cast_if_present<const CEventDamage>(event)) {
        if (eDmg->m_pSourceEntity && eDmg->m_pSourceEntity->GetIsTypeVehicle()) {
            switch (eDmg->m_weaponType) {
            case WEAPON_RAMMEDBYCAR:
            case WEAPON_RUNOVERBYCAR:
                return true;
            }
        }
    }
    return false;
}

// 0x657AC0
bool CTaskSimpleEvasiveDive::ProcessPed(CPed* ped) {
    if (m_bFinished) {
        return true;
    }
    if (!m_DiveAnim) {
        StartAnim(ped);
    }
    return false;
}

// 0x655F20
void CTaskSimpleEvasiveDive::StartAnim(CPed* ped) {
    ped->Say(CTX_GLOBAL_DODGE);

    m_DiveAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_EV_DIVE, 8.0f);
    m_DiveAnim->SetFinishCallback(FinishAnimEvasiveDiveCB, this);

    if (m_ToEvade && ped->IsCop()) {
        if (m_ToEvade->m_pDriver && m_ToEvade->m_pDriver->IsPlayer()) {
            FindPlayerWanted()->RegisterCrime_Immediately(CRIME_VEHICLE_DAMAGE, ped->GetPosition(), (uint32)ped, false);
            FindPlayerWanted()->RegisterCrime_Immediately(CRIME_SPEEDING, ped->GetPosition(), (uint32)ped, false);
        }
    }
}

void CTaskSimpleEvasiveDive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleEvasiveDive, 0x86f244, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x653560, "1", {}, CVehicle*);
    RH_ScopedDestructorInstall(0x6535D0);

    RH_ScopedGlobalInstall(FinishAnimEvasiveDiveCB, 0x6536A0);
    RH_ScopedInstall(StartAnim, 0x655F20);
    RH_ScopedVMTInstall(Clone, 0x6558E0);
    RH_ScopedVMTInstall(GetTaskType, 0x6535C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x653640);
    RH_ScopedVMTInstall(ProcessPed, 0x657AC0);
}
