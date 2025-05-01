#include "StdInc.h"
#include "EventAreaCodes.h"


void CEventAreaCodes::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAreaCodes, 0x85B7C0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B2190);
    RH_ScopedVMTInstall(AffectsPed, 0x4B2270);
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B2350);
}

// 0x4B2190
CEventAreaCodes::CEventAreaCodes(CPed* ped)
{
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

CEventAreaCodes::~CEventAreaCodes()
{
    CEntity::SafeCleanUpRef(m_ped);
}

// 0x4B2190
CEventAreaCodes* CEventAreaCodes::Constructor(CPed* ped)
{
    this->CEventAreaCodes::CEventAreaCodes(ped);
    return this;
}

// 0x4B2270
bool CEventAreaCodes::AffectsPed(CPed* ped)
{
    if (!m_ped)
        return false;

    CPed* targetPed = nullptr;
    if (const auto killPedOnFoot = ped->GetTaskManager().Find<CTaskComplexKillPedOnFoot>()) {
        targetPed = killPedOnFoot->m_target;
        if (targetPed != m_ped) {
            if (const auto arrestPed = ped->GetTaskManager().Find<CTaskComplexArrestPed>()) {
                targetPed = arrestPed->m_PedToArrest;
            }
        }
    }

    if (targetPed == m_ped) {
        if (m_ped->GetIntelligence()->FindTaskByType(TASK_COMPLEX_GOTO_DOOR_AND_OPEN))
            return true;
        if (ped->m_pContactEntity && m_ped->m_pContactEntity) {
            if (ped->m_pContactEntity->m_nAreaCode == m_ped->m_pContactEntity->m_nAreaCode)
                return false;
        }
        if (m_ped->IsAlive()
            && ped->IsAlive()
            && (!ped->m_pContactEntity || ped->m_pContactEntity->m_nAreaCode != AREA_CODE_13)
            && (!m_ped->m_pContactEntity || m_ped->m_pContactEntity->m_nAreaCode != AREA_CODE_13))
        {
            return true;
        }
    }
    return false;
}

// 0x4B2350
bool CEventAreaCodes::TakesPriorityOver(const CEvent& refEvent)
{
    if (CEventHandler::IsTemporaryEvent(refEvent))
        return true;
    return CEvent::TakesPriorityOver(refEvent);
}