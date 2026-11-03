#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "StdInc.h"

#include "TaskGangHasslePed.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskSimpleRunAnim.h"

void CTaskGangHasslePed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHasslePed, 0x86FA00, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65FED0);
    RH_ScopedInstall(Destructor, 0x65FF60);

    RH_ScopedVMTInstall(CreateNextSubTask, 0x6642C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x664380);
    RH_ScopedVMTInstall(ControlSubTask, 0x65FFE0);
}

// 0x65FED0
CTaskGangHasslePed::CTaskGangHasslePed(CPed* ped, int32 hassleType, int32 rndMin, int32 rndMax) :
    m_Ped{ ped },
    m_HassleType{ (eHassleType)hassleType },
    m_RndMin{ rndMin },
    m_RndMax{ rndMax },
    m_bAnimBlockRefAdded{ false },
    m_Timer{} {
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x65FF60
CTaskGangHasslePed::~CTaskGangHasslePed() {
    CEntity::SafeCleanUpRef(m_Ped);

    if (m_bAnimBlockRefAdded) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bAnimBlockRefAdded = false;
    }
}

CTask* CTaskGangHasslePed::CreateSeekTask(float radius) const {
    return new CTaskComplexSeekEntityStandard{ m_Ped, 999'999, 1'000, radius, 2.f, 2.f, false, true };
}

// 0x6642C0
CTask* CTaskGangHasslePed::CreateNextSubTask(CPed* ped) {
    const auto radius = CGeneral::GetRandomNumberInRange(3.f, 5.f);
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_KILL_PED_ON_FOOT) {
        return nullptr;
    }
    return CreateSeekTask(radius);
}

// 0x664380
CTask* CTaskGangHasslePed::CreateFirstSubTask(CPed* ped) {
    if (!m_Ped) {
        return nullptr;
    }
    const auto radius = CGeneral::GetRandomNumberInRange(3.f, 5.f);
    m_Timer.Start(CGeneral::GetRandomNumberInRange(m_RndMin, m_RndMax));
    return CreateSeekTask(radius);
}

// 0x65FFE0
CTask* CTaskGangHasslePed::ControlSubTask(CPed* ped) {
    if (!m_Ped) {
        return nullptr;
    }

    // 0x66000B - Hassled them long enough
    if (m_Timer.IsOutOfTime()) {
        switch (m_HassleType) {
        case HASSLE_ABUSE_AND_ATTACK: {
            if (m_pSubTask->GetTaskType() != TASK_COMPLEX_KILL_PED_ON_FOOT) {
                return new CTaskComplexKillPedOnFoot{ m_Ped };
            }
            break;
        }
        case HASSLE_ABUSE:
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
            const auto animId = (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA + CGeneral::GetRandomNumberInRange(0, 8)); // ANIM_ID_PRTIAL_GNGTLK[A-H]
            tm.SetTaskSecondary(new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, animId, 4.f, false }, TASK_SECONDARY_PARTIAL_ANIM);
            break;
        }
        case 100:
            tm.SetTaskSecondary(new CTaskComplexPlayHandSignalAnim{ ANIM_ID_UNDEFINED, 4.f }, TASK_SECONDARY_PARTIAL_ANIM);
            break;
        }
    }

    // 0x66021E
    switch (m_HassleType) {
    case HASSLE_EYE:
        ped->Say(CTX_GLOBAL_EYEING_PED);
        break;
    case HASSLE_ABUSE:
    case HASSLE_ABUSE_AND_ATTACK:
        CTaskComplexGangLeader::DoGangAbuseSpeech(ped, m_Ped);
        break;
    }

    return m_pSubTask;
}
