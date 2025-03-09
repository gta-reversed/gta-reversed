#include "StdInc.h"

#include "EventAttractor.h"

#include "IKChainManager_c.h"

void CEventAttractor::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAttractor, 0x85B1B8, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4AF350);
    RH_ScopedVMTInstall(AffectsPed, 0x4AF4B0);
    RH_ScopedVMTInstall(CloneEditable, 0x4B7440);
    RH_ScopedInstall(IsEffectActive, 0x4AF460);
}

CEventAttractor::CEventAttractor(C2dEffectPedAttractor* effect, CEntity* entity, bool bAvoidLookingAtAttractor, eTaskType taskType) :
    CEventEditableResponse{taskType}
{
    m_2dEffect = effect;
    m_entity   = entity;
    m_bAvoidLookingAtAttractor = bAvoidLookingAtAttractor;
    CEntity::SafeRegisterRef(m_entity);
}

CEventAttractor::~CEventAttractor() {
    CEntity::SafeCleanUpRef(m_entity);
}

// 0x4AF4B0
bool CEventAttractor::AffectsPed(CPed* ped) {
    if (   ped->IsAlive()
        && (GetEventType() != EVENT_ATTRACTOR || m_entity)
        && (GetEventType() != EVENT_ATTRACTOR || IsEffectActive(m_entity, m_2dEffect))
        && !ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_BE_IN_COUPLE))
    {
        if (ped->m_nPedType != PED_TYPE_COP
            || GetEventType() != EVENT_ATTRACTOR
            || !FindPlayerWanted()->m_nWantedLevel
            && m_2dEffect->m_nAttractorType == PED_ATTRACTOR_TRIGGER_SCRIPT
            && CPopulation::PedMICanBeCreatedAtThisAttractor(ped->GetModelID(), m_2dEffect->m_szScriptName))
        {
            CTask* activeTask = ped->GetTaskManager().GetActiveTask();
            if (!activeTask
                || m_bAvoidLookingAtAttractor
                ||
                activeTask->GetTaskType() != TASK_COMPLEX_USE_EFFECT
                && (m_2dEffect->m_nAttractorType != PED_ATTRACTOR_SHELTER || CWeather::IsRainy()))
            {
                if (m_bAvoidLookingAtAttractor)
                    return true;
                if (CGeneral::GetRandomNumberInRange(0, 100) >= m_2dEffect->m_nPedExistingProbability)
                    return false;
                if (CGeneral::GetRandomNumberInRange(0, 100) >= m_2dEffect->field_36)
                    return true;
                if (!g_ikChainMan.IsLooking(ped)) {
                    uint32 time = CGeneral::GetRandomNumberInRange(2000, 4000);
                    CVector point = m_entity->GetMatrix().TransformPoint(m_2dEffect->m_Pos);
                    g_ikChainMan.LookAt("CEventAttractor", ped, 0, time, BONE_UNKNOWN, &point, false, 0.25f, 500, 3, false);
                }
            }     
        }
    }
    return false;
}

// 0x4B7440
CEventEditableResponse* CEventAttractor::CloneEditable()
{
    return new CEventAttractor(m_2dEffect, m_entity, m_bAvoidLookingAtAttractor);
}

// 0x4AF460
bool CEventAttractor::IsEffectActive(CEntity* entity, const C2dEffectPedAttractor* effect) {
    auto mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    for (int32 i = 0; i < mi->m_n2dfxCount; i++) {
        if (effect == notsa::dyn_cast_if_present<C2dEffectPedAttractor>(mi->Get2dEffect(i))) {
            return true;
        }
    }
    return false;
}
