#include "StdInc.h"
#include "TaskComplexSignalAtPed.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleDoHandSignal.h"
#include "TaskSimpleRunAnim.h"
#include "Ragdoll/IKChainManager.h"
#include "General.h"

void CTaskComplexSignalAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSignalAtPed, 0x86fa8c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660A30);
    RH_ScopedInstall(Destructor, 0x660AB0);

    RH_ScopedVMTInstall(Clone, 0x662140);
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
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_TURN_TO_FACE_ENTITY) {
        if (m_initialPause == -1) {
            m_initialPause = CGeneral::GetRandomNumber() % 2000;
        }
        return new CTaskSimpleStandStill(m_initialPause, false, false, 8.0f);
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL) {
        return new CTaskSimpleDoHandSignal{};
    }
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_DO_HAND_SIGNAL) {
        if (!m_areAnimsReferenced || !m_playAnimAtEnd) {
            return nullptr;
        }
        return new CTaskSimpleRunAnim(ANIM_GROUP_GANGS, (AnimationId)(279 - CGeneral::GetRandomNumber() % 8), 4.0f, false);
    }
    return nullptr;
}
// 0x660CC0
CTask* CTaskComplexSignalAtPed::CreateFirstSubTask(CPed* ped) {
    if (!m_pedToSignalAt) {
        return nullptr;
    }
    if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        return nullptr;
    }
    ped->StopPlayingHandSignal();
    g_ikChainMan.LookAt("SignalAtPed", ped, m_pedToSignalAt, 5000, BONE_HEAD, nullptr, false, 0.25f, 500, 3, false);
    return new CTaskComplexTurnToFaceEntityOrCoord{ m_pedToSignalAt, 0.5f, 0.2f };
}

// 0x660D80
CTask* CTaskComplexSignalAtPed::ControlSubTask(CPed* ped) {
    if (!m_areAnimsReferenced) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
            if (!CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
                CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
            } else {
                CAnimManager::AddAnimBlockRef(blk);
                m_areAnimsReferenced = true;
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_areAnimsReferenced = false;
    }

    if (m_pedToSignalAt) {
        const auto pedPos = ped->GetPosition();
        const auto targetPos = m_pedToSignalAt->GetPosition();
        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(targetPos.x - pedPos.x, targetPos.y - pedPos.y, 0.0f, 0.0f));
    }
    return m_pSubTask;
}
