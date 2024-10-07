#include "StdInc.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "SeekEntity/TaskComplexSeekEntityRadiusAngleOffset.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"

void CTaskComplexSeekCoverUntilTargetDead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekCoverUntilTargetDead, 0x86dac4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor_Ped, 0x622BC0);
    RH_ScopedInstall(Constructor_Grp, 0x622B80);
    RH_ScopedInstall(Destructor, 0x622C20);

    RH_ScopedInstall(GetCoverPed, 0x622D50);
    RH_ScopedInstall(GetTargetPed, 0x622CC0);

    RH_ScopedVMTInstall(Clone, 0x623900);
    RH_ScopedVMTInstall(GetTaskType, 0x622BB0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x622C90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x62E6C0);
    RH_ScopedVMTInstall(ControlSubTask, 0x62E8B0);
}

// 0x622BC0
CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(CPed* ped) :
    m_targetPed{ped}
{
    CEntity::SafeRegisterRef(m_targetPed);
}

// 0x622B80
CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(PedGroupId grpId) :
    m_groupId{ grpId }
{
}

CTaskComplexSeekCoverUntilTargetDead::CTaskComplexSeekCoverUntilTargetDead(const CTaskComplexSeekCoverUntilTargetDead& o) :
    m_groupId{ o.m_groupId },
    m_targetPed{ o.m_groupId == -1 ? o.m_targetPed : nullptr }
{
    CEntity::SafeRegisterRef(m_targetPed);
}

// 0x622C20
CTaskComplexSeekCoverUntilTargetDead::~CTaskComplexSeekCoverUntilTargetDead() {
    CEntity::SafeCleanUpRef(m_targetPed);
    CEntity::SafeCleanUpRef(m_coverPed);
}

// 0x622D50
CPed* CTaskComplexSeekCoverUntilTargetDead::GetCoverPed(CPed* ped) {
    if (const auto grp = ped->GetGroup()) {
        // Find next closest, alive, ped with a weapon to to cover from the ped's group
        return std::get<CPed*>(grp->GetMembership().GetMemberClosestToIf(ped, [](CPed& mem) {
            return !mem.GetActiveWeapon().IsTypeMelee() && mem.IsAlive();
        }));
    }
    return nullptr;
}

// 0x622CC0
CPed* CTaskComplexSeekCoverUntilTargetDead::GetTargetPed() {
    if (m_groupId == -1) { // No group
        return m_targetPed && m_targetPed->IsAlive()
            ? m_targetPed
            : nullptr;
    }

    // Otherwise get a member from the group that is alive

    auto& group = CPedGroups::GetGroup(m_groupId);

    if (const auto leader = group.GetMembership().GetLeader()) {
        if (leader->IsAlive()) {
            return leader;
        }
    }

    for (auto& mem : group.GetMembership().GetMembers()) {
        if (mem.IsAlive()) {
            return &mem;
        }
    }

    return nullptr;
}

// 0x622C90
CTask* CTaskComplexSeekCoverUntilTargetDead::CreateNextSubTask(CPed* ped) {
    return CTask::IsA<TASK_SIMPLE_STAND_STILL>(m_pSubTask) // SimpleStandStill is the last task
        ? CreateFirstSubTask(ped)
        : nullptr;
}

// 0x62E6C0
CTask* CTaskComplexSeekCoverUntilTargetDead::CreateFirstSubTask(CPed* ped) {
    CEntity::ChangeEntityReference(m_coverPed, GetCoverPed(ped));
    CEntity::ChangeEntityReference(m_targetPed, GetTargetPed());

    if (!m_coverPed || !m_targetPed) {
        return nullptr;
    }

    return CreateSequence(ped);
}

// 0x62E8B0
CTask* CTaskComplexSeekCoverUntilTargetDead::ControlSubTask(CPed* ped) {
    // No target, or they're dead?
    if (!m_targetPed || !m_targetPed->IsAlive()) {
        CEntity::ChangeEntityReference(m_targetPed, GetTargetPed()); // Try finding a new one

        // If no new one is found, try aborting current subtask, and finish up
        if (!m_targetPed && m_pSubTask->MakeAbortable(ped)) {
            return new CTaskSimpleStandStill{ -1 };
        }
    }

    // No ped to cover, or they're dead?
    if (!m_coverPed || !m_coverPed->IsAlive()) { // Inverted
#ifdef FIX_BUGS
        CEntity::ChangeEntityReference(m_coverPed, GetCoverPed(ped));
#else
        CEntity::ChangeEntityReference(m_coverPed, GetTargetPed());
#endif
        // Try aborting current subtask and returning new
        if (m_pSubTask->MakeAbortable(ped)) { // Moved out as common code
            if (m_coverPed) { // Found a new ped to cover, do it all over again
                return CreateSequence(ped);
            }
            return new CTaskSimpleStandStill{ -1 }; // No new ped to cover, so finish it up
        }
    }

    return m_pSubTask;
}

CTaskComplexSequence* CTaskComplexSeekCoverUntilTargetDead::CreateSequence(CPed* ped) {
    return new CTaskComplexSequence{
        new CTaskComplexSeekEntityRadiusAngleOffset{
            m_coverPed,
            50000,
            1000,
            1.0f,
            2.0f,
            2.0f,
            true,
            true,
            {1.5f, PI}
        },
        new CTaskComplexTurnToFaceEntityOrCoord{ m_targetPed },
        new CTaskSimpleStandStill{ 2000 }
    };
}
