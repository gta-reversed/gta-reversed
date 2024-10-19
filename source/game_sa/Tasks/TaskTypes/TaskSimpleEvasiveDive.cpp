#include "StdInc.h"

#include "EventDamage.h"
#include "TaskSimpleEvasiveDive.h"

// 0x653560
CTaskSimpleEvasiveDive::CTaskSimpleEvasiveDive(CVehicle* vehicle) :
    m_EvadeVeh{vehicle}
{
}

// 0x6535D0
CTaskSimpleEvasiveDive::~CTaskSimpleEvasiveDive() {
    if (m_DiveAnim) {
        m_DiveAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x653640
bool CTaskSimpleEvasiveDive::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_DiveAnim) {
            m_DiveAnim->SetBlendDelta(-1000.f);
        }
        return true;
    }
    if (const auto eDmg = CEvent::DynCast<const CEventDamage>(event)) {
        if (eDmg->m_pSourceEntity && eDmg->m_pSourceEntity->IsVehicle()) {
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

    m_DiveAnim = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_EV_DIVE, 8.0f);
    m_DiveAnim->SetFinishCallback(FinishAnimEvasiveDiveCB, this);

    if (m_EvadeVeh && ped->IsCop()) {
        if (m_EvadeVeh->m_pDriver && m_EvadeVeh->m_pDriver->IsPlayer()) {
            const auto wanted = FindPlayerWanted();
            wanted->RegisterCrime_Immediately(CRIME_VEHICLE_DAMAGE, ped->GetPosition(), ped, 0);
            wanted->RegisterCrime_Immediately(CRIME_TYPE_9, ped->GetPosition(), ped, 0);
        }
    }
}

// 0x6536A0
void CTaskSimpleEvasiveDive::FinishAnimEvasiveDiveCB(CAnimBlendAssociation* assoc, void* data) {
    const auto self = CTask::Cast<CTaskSimpleEvasiveDive>(static_cast<CTask*>(data));

    self->m_bFinished = true;
    self->m_DiveAnim  = nullptr;
}
