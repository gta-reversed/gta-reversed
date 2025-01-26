#include "StdInc.h"

#include "TaskAllocatorKillThreatsDriveby.h"
#include "TaskAllocatorKillThreatsBasic.h"

#include "TaskComplexBeInGroup.h"
#include "TaskSimpleGangDriveBy.h"
#include "TaskComplexKillPedGroupOnFoot.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexSequence.h"
#include "TaskSimpleLookAbout.h"

#include <InterestingEvents.h>

// 0x69CC30
void CTaskAllocatorKillThreatsDriveby::AllocateTasks(CPedGroupIntelligence* intel) {
    intel->FlushTasks(intel->GetPedTaskPairs(), nullptr);
    intel->FlushTasks(intel->GetSecondaryPedTaskPairs(), nullptr);

    if (!m_Threat) {
        return;
    }

    auto* const group = &intel->GetPedGroup();
    if (auto* const tPrimaryBeInGroup = CTask::Cast<CTaskComplexBeInGroup>(m_Threat->GetTaskManager().FindTaskByType(TASK_PRIMARY_PRIMARY, TASK_COMPLEX_BE_IN_GROUP))) {
        const auto threatGroup = &tPrimaryBeInGroup->GetGroup();
        if (group == threatGroup) { // 0x69CCB2
            NOTSA_LOG_DEBUG("ComputeKillThreatsDriveby() - threat ped already in group"); // vanilla
        } else { // 0x69CCEC
            CPed* closest[TOTAL_PED_GROUP_MEMBERS]{};
            ComputeClosestPeds(*group, *threatGroup, closest);
            for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
                auto* mem = group->GetMembership().GetMember(i);
                if (!mem || mem->IsPlayer()) { // 0x69CD04
                    continue;
                }
                if (mem->bInVehicle && mem->m_pVehicle->IsDriver(mem)) { // 0x69CD19
                    continue;
                }
                auto* const target = closest[i];
                if (mem->bInVehicle) {
                    intel->SetEventResponseTask(mem, CTaskSimpleGangDriveBy{ target, nullptr, 60.f, 90, eDrivebyStyle::AI_ALL_DIRN, false }); // 0x69CD5A
                } else {
                    intel->SetEventResponseTask(mem, CTaskComplexKillPedGroupOnFoot{ threatGroup->GetId(), target }); // 0x69CDD4
                }
            }
            g_InterestingEvents.Add(CInterestingEvents::GANG_FIGHT, group->GetMembership().GetLeader()); // 0x69D070
        }
    } else {
        for (auto* const mem : group->GetMembership().GetMembers()) {
            if (mem->IsPlayer()) {
                continue;
            }
            if (mem->bInVehicle && mem->m_pVehicle->IsDriver(mem)) { // 0x69CD19
                continue;
            }
            if (mem->bInVehicle) {
                intel->SetEventResponseTask(mem, CTaskSimpleGangDriveBy{ m_Threat, nullptr, 60.f, 90, eDrivebyStyle::AI_ALL_DIRN, false }); // 0x69CEB4
            } else {
                intel->SetEventResponseTask(mem, CTaskComplexSequence{
                    new CTaskComplexKillPedOnFoot{ m_Threat }, // 0x69CF80
                    new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(1'000u, 2'000u) } // 0x69CFCD
                });
            }
        }
        g_InterestingEvents.Add(CInterestingEvents::GANG_ATTACKING_PED, group->GetMembership().GetLeader()); // 0x69D070
    }
}

void CTaskAllocatorKillThreatsDriveby::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillThreatsDriveby, 0x870ea8, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69CAF0);
    RH_ScopedInstall(Destructor, 0x69CB60);

    RH_ScopedVMTInstall(GetType, 0x69CB50);
    RH_ScopedVMTInstall(AllocateTasks, 0x69CC30);
    RH_ScopedVMTInstall(ProcessGroup, 0x69CBC0);
}
