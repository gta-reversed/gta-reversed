#include "StdInc.h"

#include "TaskAllocatorKillOnFoot.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleLookAbout.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexKillPedGroupOnFoot.h"

// 0x69BB80
CTaskAllocatorKillOnFoot::CTaskAllocatorKillOnFoot(CPed* ped, int32 groupId) :
    m_Target{ped},
    m_GroupTargetID{groupId}
{ }

// 0x69BCA0
void CTaskAllocatorKillOnFoot::AllocateTasks(CPedGroupIntelligence* intel) {
    auto* const group = &intel->GetPedGroup();

    CPedList members{};
    members.BuildListFromGroup_NoLeader(group->GetMembership());

    if (auto* const leader = group->GetMembership().GetLeader()) { // 0x69BD07
        if (leader->IsPlayer()) {
            members.RemovePedsThatDontListenToPlayer();
        }
    }

    const auto ProcessList = [&](CPedList& list, bool withGuns) {
        for (auto* const mem : list.GetPeds()) {
            if (auto* const task = intel->GetTask(mem, intel->GetPedTaskPairs())) { // 0x69BE1B and 0x69C097
                switch (task->GetTaskType()) {
                case TASK_SIMPLE_GUN_CTRL:
                case TASK_SIMPLE_GANG_DRIVEBY:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL:
                case TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL:
                case TASK_KILL_PED_ON_FOOT_WHILE_DUCKING:
                case TASK_COMPLEX_ARREST_PED:
                case TASK_GROUP_KILL_THREATS_BASIC:
                    continue;
                }
            }

            CTaskComplexSequence tseq{};

            if (mem->IsInVehicle()) { // 0x69BEC4 and 0x69C109
                tseq.AddTask(new CTaskComplexLeaveCar{ mem->m_pVehicle, 0, 0, true, false });
            }

            if (m_GroupTargetID == -1) { // 0x69BEC9 and 0x69C185
                tseq.AddTask(new CTaskComplexKillPedOnFoot{ m_Target, -1, 0, 0, 0, withGuns ? 2u : 1u });
                if (withGuns) {
                    tseq.AddTask(new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(0u, 1000u) });
                }
            } else { // 0x69BED5 and 0x69C17C
                tseq.AddTasks(
                    new CTaskComplexKillPedGroupOnFoot{ m_GroupTargetID, m_Target }
                );
            }

            intel->SetEventResponseTask(mem, tseq); // 0x69C01E and 0x69C1DA (Yes, the former is just inlined `SetEventResponseTask`)
        }
    };

    // 0x69BD58
    if (m_Target && !m_Target->GetActiveWeapon().IsTypeMelee()) { // NOTE: Left out the check of some static variable that's always false
        CPedList membersWithGuns{};
        membersWithGuns.ExtractPedsWithGuns(members);

        CFormation::FindCoverPoints(m_Target->GetPosition(), 30.f);
        CFormation::DistributeDestinations_CoverPoints(membersWithGuns, m_Target->GetPosition());

        ProcessList(membersWithGuns, true);
    }

    // 0x69C075
    ProcessList(members, false);
}

void CTaskAllocatorKillOnFoot::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillOnFoot, 0x870e48, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69BB80);
    RH_ScopedInstall(Destructor, 0x69BBF0);

    RH_ScopedVMTInstall(GetType, 0x69BBE0);
    RH_ScopedVMTInstall(AllocateTasks, 0x69BCA0);
}
