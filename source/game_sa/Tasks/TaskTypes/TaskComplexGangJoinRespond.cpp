#include "StdInc.h"

#include "TaskComplexGangJoinRespond.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexGangJoinRespond::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangJoinRespond, 0x86FB5C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6616F0);
    RH_ScopedInstall(Destructor, 0x661720);

    RH_ScopedVMTInstall(Clone, 0x662290);
    RH_ScopedVMTInstall(MakeAbortable, 0x661790);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6617A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6618D0);
    RH_ScopedVMTInstall(ControlSubTask, 0x661950);
}

// 0x6616F0
CTaskComplexGangJoinRespond::CTaskComplexGangJoinRespond(bool response) :
    m_response{response}
{
}

// 0x661720
CTaskComplexGangJoinRespond::~CTaskComplexGangJoinRespond() {
    if (m_animsReferenced) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }
}

// 0x6617A0
CTask* CTaskComplexGangJoinRespond::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() != TASK_SIMPLE_ANIM) {
        if (!m_animsReferenced) { // Gang anims not loaded yet - Keep waiting
            if (++m_attempts < 11) {
                return new CTaskSimpleStandStill{ 250, false, false, 8.0f };
            }
        } else if (m_response) {
            return new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, ANIM_ID_INVITE_YES, 4.0f, false };
        } else {
            return new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, ANIM_ID_INVITE_NO, 4.0f, false };
        }
    }
    return nullptr;
}

// 0x6618D0
CTask* CTaskComplexGangJoinRespond::CreateFirstSubTask(CPed* ped) {
    m_attempts = 0;
    return new CTaskComplexTurnToFaceEntityOrCoord{ FindPlayerPed(0), 0.5f, 0.2f };
}

// 0x661950
CTask* CTaskComplexGangJoinRespond::ControlSubTask(CPed* ped) {
    // Make the ped face the player
    {
        const auto pedPos    = ped->GetPosition();
        const auto playerPos = FindPlayerPed()->GetPosition();

        CVector dir{
            playerPos.x - pedPos.x,
            playerPos.y - pedPos.y,
            0.0f
        };
        dir.Normalise();

        ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.0f, 0.0f));
    }

    // Make sure the gang anims are loaded (if they can/need to be)
    if (!m_animsReferenced) {
        if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
            if (!CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
                CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
            } else {
                CAnimManager::AddAnimBlockRef(blk);
                m_animsReferenced = true;
            }
        }
    } else if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_animsReferenced = false;
    }

    return m_pSubTask;
}
