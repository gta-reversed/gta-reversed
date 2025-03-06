#include "StdInc.h"

#include "TaskComplexUseEffect.h"
#include "TaskComplexGoToAttractor.h"
#include "TaskSimpleNone.h"

#include "EventPedCollisionWithPed.h"
#include "Scripted2dEffects.h"

void CTaskComplexUseEffect::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffect, 0x86e0d8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6321F0);
    RH_ScopedInstall(Destructor, 0x632270);

    RH_ScopedVMTInstall(Clone, 0x636710);
    RH_ScopedVMTInstall(GetTaskType, 0x632260);
    RH_ScopedVMTInstall(MakeAbortable, 0x6322F0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632420);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632460);
    RH_ScopedVMTInstall(ControlSubTask, 0x632570);
}

// 0x6321F0
CTaskComplexUseEffect::CTaskComplexUseEffect(C2dEffectPedAttractor* attractor, CEntity* entity) :
    m_2dFx{attractor},
    m_Entity{entity}
{
    CEntity::SafeRegisterRef(m_Entity);
}

// For 0x636710
CTaskComplexUseEffect::CTaskComplexUseEffect(const CTaskComplexUseEffect& o) :
    CTaskComplexUseEffect{o.m_2dFx, o.m_Entity}
{
}

// 0x632270
CTaskComplexUseEffect::~CTaskComplexUseEffect() {
    if (m_Ped && m_PedAttractor) {
        GetPedAttractorManager()->DeRegisterPed(m_Ped, m_PedAttractor);
    }
    CEntity::SafeCleanUpRef(m_Entity);
}

// 0x6322F0
bool CTaskComplexUseEffect::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (!m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }

    m_Ped          = nullptr;
    m_PedAttractor = nullptr;

    if (event && notsa::contains({EVENT_PED_COLLISION_WITH_PED, EVENT_PED_COLLISION_WITH_PLAYER}, event->GetEventType())) {
        if (const auto collisionVictim = static_cast<const CEventPedCollisionWithPed*>(event)->m_victim) {
            if (const auto t = CTask::DynCast<CTaskComplexGoToAttractor>(m_pSubTask)) {
                if ((ped->GetPosition() - t->m_vecAttrPosn).SquaredMagnitude2D() <= sq(1.5f)) {
                    if (m_MoveState == PEDMOVE_STILL && (collisionVictim->GetPosition() - t->m_vecAttrPosn).SquaredMagnitude2D() <= sq(1.5f)) {
                        m_bAbort = true;
                    }
                }
            }
        }
    }

    return true;
}

// 0x632420
CTask* CTaskComplexUseEffect::CreateNextSubTask(CPed* ped) {
    if (m_PedAttractor && GetPedAttractorManager()->GetRelevantAttractor(ped, m_2dFx, m_Entity) == m_PedAttractor) {
        return m_PedAttractor->GetTaskForPed(ped);
    }
    return nullptr;
}

// 0x632460
CTask* CTaskComplexUseEffect::CreateFirstSubTask(CPed* ped) {
    CEntity::ChangeEntityReference(m_Ped, ped);
    m_PedAttractor = !m_bAbort && m_Entity && CScripted2dEffects::GetIndex(m_2dFx) != -1
        ? GetPedAttractorManager()->RegisterPedWithAttractor(ped, m_2dFx, m_Entity, m_MoveState)
        : nullptr;
    return m_PedAttractor
        ? m_PedAttractor->GetTaskForPed(ped)
        : new CTaskSimpleNone{};
}

// 0x632570
CTask* CTaskComplexUseEffect::ControlSubTask(CPed* ped) {
    if (ped->IsCop()) {
        if (const auto w = FindPlayerWanted(); w->m_nWantedLevel && w->CanCopJoinPursuit(ped->AsCop())) {
            if (m_pSubTask->MakeAbortable(ped)) {
                GetPedAttractorManager()->DeRegisterPed(m_Ped, m_PedAttractor);
                return nullptr;
            }
        }
    }
    return m_PedAttractor
        ? m_PedAttractor->GetTaskForPed(ped)
        : CreateFirstSubTask(ped);
}
