#include "StdInc.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleRunAnim.h"
#include "Ragdoll/IKChainManager.h"


// 0x660A30
CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 initialPause, bool playAnimAtEnd) :
    m_PedToSignalAt{ pedToSignalAt },
    m_PlayAnimAtEnd{ playAnimAtEnd },
    m_InitialPause{ notsa::bugfixes::CTaskComplexSignalAtPed_Constructor_InitialPause ? initialPause : 0 } {
}

// For 0x662140
CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o) :
    CTaskComplexSignalAtPed{ o.m_PedToSignalAt, o.m_InitialPause, o.m_PlayAnimAtEnd } {
}

// 0x660B30
CTask* CTaskComplexSignalAtPed::CreateNextSubTask(CPed* ped) {
    switch (const auto subtt = m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: {
        return new CTaskSimpleStandStill{
            m_InitialPause == -1
                ? CGeneral::GetRandomNumberInRange(0, 2'000)
                : m_InitialPause
        };
    }
    case TASK_SIMPLE_STAND_STILL:
        return new CTaskSimpleDoHandSignal{};
    case TASK_SIMPLE_DO_HAND_SIGNAL: {
        if (!m_AreAnimsReferenced || !m_PlayAnimAtEnd) {
            return nullptr;
        }
        return new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, CAnimManager::GetRandomGangTalkAnim(), 4.f, false};
    }
    case TASK_SIMPLE_ANIM:
        return nullptr;
    default:
        NOTSA_UNREACHABLE_CASE(subtt);
    }
}

// 0x660CC0
CTask* CTaskComplexSignalAtPed::CreateFirstSubTask(CPed* ped) {
    if (!m_PedToSignalAt || !CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        return nullptr;
    }
    ped->StopPlayingHandSignal();
    g_ikChainMan.LookAt(
        "TaskSignalAtPed",
        ped,
        m_PedToSignalAt,
        5'000,
        BONE_HEAD,
        nullptr,
        true,
        0.15f,
        500,
        3,
        false
    );
    return new CTaskComplexTurnToFaceEntityOrCoord{ m_PedToSignalAt };
}

// 0x660D80
CTask* CTaskComplexSignalAtPed::ControlSubTask(CPed* ped) {
    CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_AreAnimsReferenced);

    if (m_PedToSignalAt) {
        ped->m_fAimingRotation = (m_PedToSignalAt->GetPosition2D() - ped->GetPosition2D()).Heading();
    }

    return m_pSubTask;
}

void CTaskComplexSignalAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSignalAtPed, 0x86fa8c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedConstructorInstall(0x660A30, "0", {}, CPed*, int32, bool);
    RH_ScopedDestructorInstall(0x660AB0);

    RH_ScopedVMTInstall(Clone, 0x662140);
    RH_ScopedVMTInstall(GetTaskType, 0x660AA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x660B30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x660CC0);
    RH_ScopedVMTInstall(ControlSubTask, 0x660D80);
}
