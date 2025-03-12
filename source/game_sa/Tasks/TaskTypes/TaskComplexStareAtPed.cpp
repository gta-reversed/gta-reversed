#include "StdInc.h"
#include "IKChainManager_c.h"
#include "TaskSimpleRunAnim.h"
#include "TaskComplexStareAtPed.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexGangLeader.h"

void CTaskComplexStareAtPed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexStareAtPed, 0x86fa2c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x660270);
    RH_ScopedInstall(Destructor, 0x660310);

    RH_ScopedVMTInstall(Clone, 0x660390);
    RH_ScopedVMTInstall(GetTaskType, 0x660300);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x660420);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x660510);
    RH_ScopedVMTInstall(ControlSubTask, 0x6605C0);
}

// 0x660270
CTaskComplexStareAtPed::CTaskComplexStareAtPed(CPedGroup* pedGroip, CPed* pedToStareAt, int32 timeout) :
    m_pPedGroup{pedGroip},
    m_pPed{pedToStareAt},
    m_timeout{timeout}
{
    CEntity::SafeRegisterRef(m_pPed);
}

CTaskComplexStareAtPed::CTaskComplexStareAtPed(const CTaskComplexStareAtPed& o) :
    CTaskComplexStareAtPed{
        o.m_pPedGroup,
        o.m_pPed,
        o.m_timer.m_bStarted
            ? static_cast<int32>(o.m_timer.m_nStartTime) + o.m_timer.m_nInterval - static_cast<int32>(CTimer::GetTimeInMS()) // Either time left of timer
            : o.m_timeout // Or the original timeout
    }
{
}

// 0x660310
CTaskComplexStareAtPed::~CTaskComplexStareAtPed() {
    CEntity::SafeCleanUpRef(m_pPed);

    if (m_animsReferenced) {
        UnrefAnimBlock();
    }
}

// 0x660420
CTask* CTaskComplexStareAtPed::CreateNextSubTask(CPed* ped) {
    if (const auto turnToFace = CTask::DynCast<CTaskComplexTurnToFaceEntityOrCoord>(m_pSubTask)) {
        return new CTaskSimpleStandStill{ CGeneral::GetRandomNumberInRange(500, 1000) };
    }
    return new CTaskComplexTurnToFaceEntityOrCoord{ m_pPed };
}

// 0x660510
CTask* CTaskComplexStareAtPed::CreateFirstSubTask(CPed* ped) {
    m_timer.Start(m_timeout);
    return new CTaskSimpleStandStill{ CGeneral::GetRandomNumberInRange(500, 1000) };
}

// 0x6605C0
CTask* CTaskComplexStareAtPed::ControlSubTask(CPed* ped) {

    // Below code copied from `CTaskComplexGangLeader::ControlSubTask`
    // Make sure anmims are loaded (if they can/need to be)
    if (m_animsReferenced) { // 0x66239B
        if (!CTaskComplexGangLeader::ShouldLoadGangAnims()) {
            UnrefAnimBlock();
        }
    } else if (CTaskComplexGangLeader::ShouldLoadGangAnims()) {
        const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
        if (CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
            CAnimManager::AddAnimBlockRef(blk);
            m_animsReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
        }
    }

    CTaskComplexGangLeader::DoGangAbuseSpeech(ped, m_pPed);

    if (m_lookInitialised) {
        m_pPed = m_pPed;
        if (g_ikChainMan.GetLookAtEntity(ped) != m_pPed) {
            g_ikChainMan.LookAt(
                "TaskStareAtPed",
                ped,
                m_pPed,
                (int32)&gDefaultTaskTime,
                BONE_HEAD,
                nullptr,
                true,
                0.15f,
                500,
                3,
                false
            );
        }
    } else {
        g_ikChainMan.LookAt(
            "TaskStareAtPed",
            ped,
            m_pPed,
            (int32)&gDefaultTaskTime,
            BONE_HEAD,
            nullptr,
            true,
            0.15f,
            500,
            3,
            false
        );
        m_lookInitialised = true;
    }

    if ([this] {
        if (m_animsReferenced && m_pPed && m_pPedGroup) {
            if (const auto leader = m_pPedGroup->GetMembership().GetLeader()) {
                if (sq(8.f) >= (leader->GetPosition() - m_pPed->GetPosition()).SquaredMagnitude()) {
                    return true;
                }
            }
        }
        return false;
    }()) {
        if (!ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM) && CGeneral::RandomBool(1.f)) {
            ped->GetTaskManager().SetTaskSecondary(
                new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, CAnimManager::GetRandomGangTalkAnim(), 4.f},
                TASK_SECONDARY_PARTIAL_ANIM
            );
        }

        return m_pSubTask;
    } else {
        AbortIK(ped);
        return nullptr;
    }
}

void CTaskComplexStareAtPed::UnrefAnimBlock() {
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    m_animsReferenced = false;
}

void CTaskComplexStareAtPed::AbortIK(CPed* ped) {
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 500);
    }
}
