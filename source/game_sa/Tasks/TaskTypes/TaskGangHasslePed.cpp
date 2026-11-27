#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "StdInc.h"

#include "TaskGangHasslePed.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskSimpleRunAnim.h"

// 0x65FED0
CTaskGangHasslePed::CTaskGangHasslePed(CPed* ped, eHassleType hassleType, int32 rndMin, int32 rndMax) :
    m_PedToHassle{ ped },
    m_HassleType{ hassleType },
    m_RndMin{ rndMin },
    m_RndMax{ rndMax } {
}

//
CTaskGangHasslePed::CTaskGangHasslePed(const CTaskGangHasslePed& o) :
    CTaskGangHasslePed{
        o.m_PedToHassle,
        o.m_HassleType,
        o.m_RndMin,
        o.m_RndMax
    } {
}

// 0x65FF60
CTaskGangHasslePed::~CTaskGangHasslePed() {
    if (std::exchange(m_bAnimBlockRefAdded, false)) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    }
}

// 0x6642C0
CTask* CTaskGangHasslePed::CreateNextSubTask(CPed* ped) {
    if (notsa::isa<CTaskComplexKillPedOnFoot>(m_pSubTask)) {
        return nullptr;
    }
    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x664380
CTask* CTaskGangHasslePed::CreateFirstSubTask(CPed* ped) {
    if (!m_PedToHassle) {
        return nullptr;
    }
    m_HassleTimer.Start(CGeneral::GetRandomNumberInRange(m_RndMin, m_RndMax));
    return CreateSubTask(TASK_COMPLEX_SEEK_ENTITY, ped);
}

// 0x65FFE0
CTask* CTaskGangHasslePed::ControlSubTask(CPed* ped) {
    if (!m_PedToHassle) {
        return nullptr;
    }

    // 0x66000B - Hassled them long enough
    if (m_HassleTimer.IsOutOfTime()) {
        switch (m_HassleType) {
        case eHassleType::ABUSE_AND_ATTACK: {
            if (!notsa::isa<CTaskComplexKillPedOnFoot>(m_pSubTask)) {
                return CreateSubTask(TASK_COMPLEX_KILL_PED_ON_FOOT, ped);
            }
            break;
        }
        case eHassleType::ABUSE:
            break;
        default:
            return nullptr;
        }
    }

    CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_bAnimBlockRefAdded);

    // 0x660145 - Randomly play a talk anim or a hand signal
    auto& tm = ped->GetTaskManager();
    if (m_bAnimBlockRefAdded && !ped->IsPlayingHandSignal() && !tm.GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
        switch (CGeneral::GetRandomNumberInRange(0, 200)) {
        case 51:
        case 52:
        case 53:
        case 54:
        case 55: {
            tm.SetTaskSecondary(new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, CAnimManager::GetRandomGangTalkAnim(), 4.f, false }, TASK_SECONDARY_PARTIAL_ANIM);
            break;
        }
        case 100:
            tm.SetTaskSecondary(new CTaskComplexPlayHandSignalAnim{ ANIM_ID_UNDEFINED, 4.f }, TASK_SECONDARY_PARTIAL_ANIM);
            break;
        }
    }

    // 0x66021E
    switch (m_HassleType) {
    case eHassleType::EYE:
        ped->Say(CTX_GLOBAL_EYEING_PED);
        break;
    case eHassleType::ABUSE:
    case eHassleType::ABUSE_AND_ATTACK:
        CTaskComplexGangLeader::DoGangAbuseSpeech(ped, m_PedToHassle);
        break;
    }

    return m_pSubTask;
}

// notsa
CTask* CTaskGangHasslePed::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType) {
    case TASK_COMPLEX_KILL_PED_ON_FOOT:
        return new CTaskComplexKillPedOnFoot{ m_PedToHassle };
    case TASK_COMPLEX_SEEK_ENTITY:
        return new CTaskComplexSeekEntityStandard{
            m_PedToHassle,
            999'999,
            1'000,
            CGeneral::GetRandomNumberInRange(3.f, 5.f),
            2.f,
            2.f,
            false,
            true
        };
    default:
        NOTSA_UNREACHABLE_CASE(taskType);
    }
}

void CTaskGangHasslePed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHasslePed, 0x86FA00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x65FED0, "1", {}, CPed*, eHassleType, int32, int32);
    RH_ScopedDestructorInstall(0x65FF60);

    RH_ScopedVMTInstall(Clone, 0x6620D0);
    RH_ScopedVMTInstall(GetTaskType, 0x65FF50);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6642C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664380);
    RH_ScopedVMTInstall(ControlSubTask, 0x65FFE0);
}
