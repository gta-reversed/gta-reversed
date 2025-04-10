#include "StdInc.h"
#include "./TaskComplexKillPedGroupOnFoot.h"
#include "./TaskComplexKillPedOnFoot.h"
#include "./TaskSimpleLookAbout.h"
#include "./TaskSimpleNone.h"

void CTaskComplexKillPedGroupOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexKillPedGroupOnFoot, 0x86da48, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x622240);
    RH_ScopedInstall(Destructor, 0x6222B0);

    RH_ScopedInstall(CreateSubTask, 0x622320);
    RH_ScopedInstall(ComputeNearestLivingGroupPed, 0x622450);

    RH_ScopedVMTInstall(Clone, 0x6237C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6222A0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x629570);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x629630);
    RH_ScopedVMTInstall(ControlSubTask, 0x622310);
}

// 0x622240
CTaskComplexKillPedGroupOnFoot::CTaskComplexKillPedGroupOnFoot(int32 groupId, CPed* ped) :
    m_grpId{groupId},
    m_currPedToKill{ped}
{
    CEntity::SafeRegisterRef(m_currPedToKill);
}

// NOTSA (For 0x6237C0)
CTaskComplexKillPedGroupOnFoot::CTaskComplexKillPedGroupOnFoot(const CTaskComplexKillPedGroupOnFoot& o) :
    CTaskComplexKillPedGroupOnFoot{o.m_grpId, o.m_currPedToKill}
{
}

// 0x6222B0
CTaskComplexKillPedGroupOnFoot::~CTaskComplexKillPedGroupOnFoot() {
    CEntity::SafeCleanUpRef(m_currPedToKill);
}

// 0x622320
CTask* CTaskComplexKillPedGroupOnFoot::CreateSubTask(eTaskType taskType, CPed* ped) {
    switch (taskType)
    {
    case TASK_FINISHED:
        return nullptr;
    case TASK_COMPLEX_KILL_PED_ON_FOOT: {
        return new CTaskComplexKillPedOnFoot{
            m_currPedToKill,
            -1,
            0,
            0,
            0,
            true
        };
    }
    case TASK_NONE:
        return new CTaskSimpleNone{};
    case TASK_SIMPLE_LOOK_ABOUT:
        return new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(3000u, 4000u) };
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x622450
CPed* CTaskComplexKillPedGroupOnFoot::ComputeNearestLivingGroupPed(CPed& ped, bool flag) {
    const auto pedToIgnore = [&]() -> CPed* {
        if (!flag) {
            if (const auto grp = ped.GetGroup()) {
                if (const auto leader = grp->GetMembership().GetLeader()) {
                    return leader->m_pTargetedObject && leader->m_pTargetedObject->IsPed()
                        ? leader->m_pTargetedObject->AsPed()
                        : ComputeNearestLivingGroupPed(ped, true);
                }
            }
        }
        return nullptr;
    }();
    const auto [closest, distSq] = CPedGroups::GetGroup(m_grpId).GetMembership().GetMemberClosestToIf(&ped, [&, this](CPed& mem) -> bool {
        if (   &mem == pedToIgnore
            || !mem.IsAlive()
            || mem.m_fHealth <= 0.f
            || CPedGroups::AreInSameGroup(&mem, &ped)
        ) {
            return false;
        }
        if (mem.IsPlayer()) {
            if (const auto wanted = mem.AsPlayer()->GetWanted()) {
                if (wanted->m_nCopsInPursuit && ped.m_nPedType != PED_TYPE_COP && ped.IsCreatedBy(PED_GAME)) {
                    return false;
                }
            }
        }
        return true;
    });
    return closest;
}

eTaskType CTaskComplexKillPedGroupOnFoot::ComputeNewTargetAndTaskType(CPed* ped) {
    CEntity::ChangeEntityReference(m_currPedToKill, ComputeNearestLivingGroupPed(*ped)); // Find new target
    return m_currPedToKill
        ? TASK_COMPLEX_KILL_PED_ON_FOOT
        : TASK_SIMPLE_LOOK_ABOUT;
}

// 0x629570
CTask* CTaskComplexKillPedGroupOnFoot::CreateNextSubTask(CPed* ped) {
    // CEntity::ClearReference(m_currPedToKill); // Not necessary, destructor will clear it
    return CreateSubTask([this, ped] {
        switch (m_pSubTask->GetTaskType()) {
        case TASK_NONE:
        case TASK_SIMPLE_LOOK_ABOUT:
            return TASK_FINISHED;
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            return ComputeNewTargetAndTaskType(ped); // Kill next or finish
        default:
            NOTSA_UNREACHABLE();
        }
    }(), ped);
}

// 0x629630
CTask* CTaskComplexKillPedGroupOnFoot::CreateFirstSubTask(CPed* ped) {
    return CreateSubTask([&, this] {
        if (m_currPedToKill) {
            return CPedGroups::AreInSameGroup(ped, m_currPedToKill)
                ? TASK_NONE
                : TASK_COMPLEX_KILL_PED_ON_FOOT;
        } else {
            return ComputeNewTargetAndTaskType(ped);
        }
    }(), ped);
}
