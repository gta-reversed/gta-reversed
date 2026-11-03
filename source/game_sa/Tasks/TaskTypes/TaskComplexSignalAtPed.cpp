#include "StdInc.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleRunAnim.h"
#include "Ragdoll/IKChainManager.h"

void CTaskComplexSignalAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSignalAtPed, 0x86fa8c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660A30);
    RH_ScopedInstall(Destructor, 0x660AB0);

    RH_ScopedVMTInstall(Clone, 0x662140);
    RH_ScopedVMTInstall(GetTaskType, 0x660AA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x660B30);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x660CC0);
    RH_ScopedVMTInstall(ControlSubTask, 0x660D80);
}

// 0x660A30
CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(CPed* pedToSignalAt, int32 unused1, bool playAnimAtEnd) :
    m_pedToSignalAt{pedToSignalAt},
    m_playAnimAtEnd{playAnimAtEnd}
{
    CEntity::SafeRegisterRef(m_pedToSignalAt);
}

CTaskComplexSignalAtPed::CTaskComplexSignalAtPed(const CTaskComplexSignalAtPed& o) :
    CTaskComplexSignalAtPed{o.m_pedToSignalAt, o.m_initialPause, o.m_playAnimAtEnd}
{
}

// 0x660AB0
CTaskComplexSignalAtPed::~CTaskComplexSignalAtPed() {
    CEntity::SafeCleanUpRef(m_pedToSignalAt);
}

// 0x660B30
CTask* CTaskComplexSignalAtPed::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case TASK_COMPLEX_TURN_TO_FACE_ENTITY: { // Facing the ped now, wait a little...
        // NOTE: `m_initialPause` is always `0` (The constructor ignores the argument), so this is never random
        const auto pause = m_initialPause == -1
            ? CGeneral::GetRandomNumberInRange(0, 2'000)
            : m_initialPause;
        return new CTaskSimpleStandStill{ pause, false, false, 8.f };
    }
    case TASK_SIMPLE_STAND_STILL: // ...then signal at them...
        return new CTaskSimpleDoHandSignal{};
    case TASK_SIMPLE_DO_HAND_SIGNAL: { // ...and finally (optionally) play a talk anim
        if (!m_areAnimsReferenced || !m_playAnimAtEnd) {
            return nullptr;
        }
        const auto animId = (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA + CGeneral::GetRandomNumberInRange(0, 8)); // ANIM_ID_PRTIAL_GNGTLK[A-H]
        return new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, animId, 4.f, false };
    }
    default:
        return nullptr;
    }
}

// 0x660CC0
CTask* CTaskComplexSignalAtPed::CreateFirstSubTask(CPed* ped) {
    if (!m_pedToSignalAt || !CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        return nullptr;
    }
    ped->StopPlayingHandSignal();
    g_ikChainMan.LookAt(
        "TaskSignalAtPed",
        ped,
        m_pedToSignalAt,
        5'000,
        BONE_HEAD,
        nullptr,
        true,
        0.15f,
        500,
        3,
        false
    );
    return new CTaskComplexTurnToFaceEntityOrCoord{ m_pedToSignalAt };
}

// 0x660D80
CTask* CTaskComplexSignalAtPed::ControlSubTask(CPed* ped) {
    CAnimManager::StreamAnimBlock("gangs", CTaskComplexGangLeader::ShouldLoadGangAnims(), m_areAnimsReferenced);

    if (m_pedToSignalAt) {
        const auto dir         = m_pedToSignalAt->GetPosition() - ped->GetPosition();
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.f, 0.f));
    }

    return m_pSubTask;
}
