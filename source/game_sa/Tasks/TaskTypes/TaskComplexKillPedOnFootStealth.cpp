#include "StdInc.h"
#include "TaskComplexKillPedOnFootStealth.h"
#include "TaskComplexInvestigateDisturbance.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleScratchHead.h"

void CTaskComplexKillPedOnFootStealth::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedOnFootStealth, 0x86d8c0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x620F70);
    RH_ScopedInstall(Destructor, 0x620FF0);

    RH_ScopedVMTInstall(Clone, 0x623400);
    RH_ScopedVMTInstall(GetTaskType, 0x620FE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x62B790);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62B810);
    RH_ScopedVMTInstall(ControlSubTask, 0x62B840);
}

// 0x620F70
CTaskComplexKillPedOnFootStealth::CTaskComplexKillPedOnFootStealth(CPed* target) :
    CTaskComplexKillPedOnFoot{target}
{
}

// NOTSA (For 0x623400)
CTaskComplexKillPedOnFootStealth::CTaskComplexKillPedOnFootStealth(const CTaskComplexKillPedOnFootStealth& o) :
    CTaskComplexKillPedOnFootStealth{m_target}
{
}

// 0x62B790
CTask* CTaskComplexKillPedOnFootStealth::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask && m_targetUnreachableTimeMs == (uint32)-1) {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_COMPLEX_SEQUENCE:
        case TASK_COMPLEX_INVESTIGATE_DISTURBANCE:
            return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
        }
    }
    return CreateSubTask(TASK_FINISHED, ped);
}

// 0x62B810
CTask* CTaskComplexKillPedOnFootStealth::CreateFirstSubTask(CPed* ped) {
    ped->Say(CTX_GLOBAL_STEALTH_DEF_SIGHTING);
    return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
}

// 0x62B840
CTask* CTaskComplexKillPedOnFootStealth::ControlSubTask(CPed* ped) {
    if (!m_target) {
        return CTaskComplexKillPedOnFoot::ControlSubTask(ped);
    }

    if ([&, this] {
        if (CPedGeometryAnalyser::CanPedTargetPed(*ped, *m_target, true)) {
            if (m_targetUnreachableTimeMs == (uint32)-1) { // State is changing (Target is now reachable)
                return true; // I guess the position check below is done the next frame in this case? No clue why.
            }
            if ((ped->GetPosition() - m_target->GetPosition()).Normalized().Dot(ped->GetForward()) >= CPedAcquaintanceScanner::ms_fThresholdDotProduct) {
                if (ped->GetIntelligence()->CanSeeEntityWithLights(m_target, true) > 0.f) {
                    return true; // All good, we can still reach the ped
                }
            }
        }
        return false; // Now can't reach them anymore
    }()) {
        m_targetUnreachableTimeMs = (uint32)-1;
        m_targetPos               = m_target->GetPosition();
    } else if (m_targetUnreachableTimeMs == (uint32)-1) {
        m_targetUnreachableTimeMs = CTimer::GetTimeInMS(); // State is changing (Can't target the target anymore)
    }

    if (m_targetUnreachableTimeMs == (uint32)-1) { // 0x62b98d
        if (m_pSubTask) {
            switch (m_pSubTask->GetTaskType()) {
            case TASK_COMPLEX_SEQUENCE:
            case TASK_COMPLEX_INVESTIGATE_DISTURBANCE: {
                if (m_pSubTask->MakeAbortable(ped)) {
                    return CTaskComplexKillPedOnFoot::CreateFirstSubTask(ped);
                }
                break;
            }
            }
        }
        return m_pSubTask;
    }

    if (CTimer::GetTimeInMS() - m_targetUnreachableTimeMs <= 5'000u) { // TODO: Magic number
        return m_pSubTask;
    }

    ped->Say(CTX_GLOBAL_STEALTH_NOTHING_THERE);

    if (!m_pSubTask) {
        return nullptr;
    }

    const auto sttype = m_pSubTask->GetTaskType();
    if (m_targetPos.z > -9999.9004f) {
        if (sttype != TASK_COMPLEX_INVESTIGATE_DISTURBANCE && m_pSubTask->MakeAbortable(ped)) {
            return new CTaskComplexInvestigateDisturbance{ m_targetPos, m_target };
        }
    } else if (sttype == TASK_COMPLEX_SEQUENCE && m_pSubTask->MakeAbortable(ped)) {
        return new CTaskComplexSequence{
            new CTaskSimpleStandStill{1000},
            new CTaskSimpleRunAnim{ped->m_nAnimGroup, ANIM_ID_ROADCROSS},
            new CTaskSimpleScratchHead{}
        };
    }
    
    return m_pSubTask;
}
