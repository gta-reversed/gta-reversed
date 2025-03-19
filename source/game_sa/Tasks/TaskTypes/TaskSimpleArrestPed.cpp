#include "StdInc.h"

#include "TaskSimpleArrestPed.h"

void CTaskSimpleArrestPed__InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleArrestPed, 0x870984, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(MakeAbortable, 0x68B740);
    RH_ScopedVMTInstall(ProcessPed, 0x68B8A0);
    RH_ScopedInstall(StartAnim, 0x68B7E0);
}

// 0x68B620
CTaskSimpleArrestPed::CTaskSimpleArrestPed(CPed* ped) {
    m_Ped = ped;
    m_bFinished = false;
    m_Assoc = nullptr;
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x68B690
CTaskSimpleArrestPed::~CTaskSimpleArrestPed() {
    if (m_Assoc) {
        m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }

    CEntity::SafeCleanUpRef(m_Ped);

    if (m_Assoc && m_Ped && !m_Ped->IsPlayer()) {
        if (m_Ped->bInVehicle && m_Ped->m_nPedState == PEDSTATE_ARRESTED) {
            m_Ped->SetPedState(PEDSTATE_DRIVING);
        }
    }
}

// 0x68B740


bool CTaskSimpleArrestPed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Assoc) {
            m_Assoc->m_BlendDelta = -1000.0f;
            m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Assoc = nullptr;
        }
        return true;
    }

    if (event && event->GetEventType() == EVENT_DAMAGE) {
        const auto dmgEvent = static_cast<const CEventDamage*>(event);
        if (dmgEvent->m_damageResponse.m_bHealthZero) {
            if (dmgEvent->m_bAddToEventGroup) {
                m_Ped->SetPedState(m_Ped->bInVehicle ? PEDSTATE_DRIVING : PEDSTATE_NONE);
                m_Ped->bIsBeingArrested = false;
            }
        }
        return true;
    }

    return false;
}

// 0x68B8A0

// 0x0
bool CTaskSimpleArrestPed::ProcessPed(CPed* ped) {
    if (m_bFinished)
        return false;

    if (m_Assoc) {
        const auto point = m_Ped
            ? m_Ped->GetBonePosition(BONE_SPINE1, false)
            : CVector{};
        const auto& pedPos = ped->GetPosition();
        auto angle = CGeneral::GetRadianAngleBetweenPoints(point.x, point.y, pedPos.x, pedPos.y);
        ped->m_fAimingRotation = angle;
        ped->m_fCurrentRotation = angle;
        ped->SetOrientation(0.0f, 0.0f, angle);
        ped->m_pedIK.PointGunAtPosition(point, m_Assoc->m_BlendAmount);
        return false;
    }

    if (ped->GetActiveWeapon().IsTypeMelee()) {
        ped->GiveWeapon(WEAPON_PISTOL, 10, false);
        ped->SetCurrentWeapon(WEAPON_PISTOL);
    }

    StartAnim(ped);
    return false;
}

// 0x68B7E0
void CTaskSimpleArrestPed::StartAnim(CPed* ped) {
    m_Assoc = CAnimManager::BlendAnimation(ped->m_pRwClump, ANIM_GROUP_DEFAULT, ANIM_ID_ARRESTGUN, 4.0f);
    m_Assoc->SetFlag(ANIMATION_IS_PLAYING, true);
    m_Assoc->SetDeleteCallback(FinishAnimArrestPedCB, this);

    if (m_Ped->IsPlayer()) {
        ped->Say(CTX_GLOBAL_ARREST, 0, 1.0f, 1, 1);
    }

    if (m_Ped->CanSetPedState()) {
        m_Ped->SetPedState(PEDSTATE_ARRESTED);
    }

    if (ped->m_nPedType == PEDTYPE_COP && m_Ped->m_nPedType <= PEDTYPE_PLAYER2) {
        if (!m_Ped->m_pPlayerData->m_pArrestingCop) {
            m_Ped->m_pPlayerData->m_pArrestingCop = static_cast<CCopPed*>(ped);
            m_Ped->m_pPlayerData->m_pArrestingCop->RegisterReference(m_Ped->m_pPlayerData->m_pArrestingCop);
        }
    }

    m_Ped->bIsBeingArrested = true;
    ped->SetPedState(PEDSTATE_ARREST_PLAYER);
}

// 0x68B7D0
void CTaskSimpleArrestPed::FinishAnimArrestPedCB(CAnimBlendAssociation* assoc, void* data) {
    auto task = static_cast<CTaskSimpleArrestPed*>(data);
    task->m_bFinished = true;
    task->m_Assoc = nullptr;
}
