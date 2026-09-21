#include "StdInc.h"

#include "TaskAllocatorKillThreatsBasic.h"
#include <InterestingEvents.h>
#include <TaskComplexKillPedGroupOnFoot.h>
#include <TaskComplexSequence.h>
#include <TaskSimpleLookAbout.h>

// 0x69C710
CTaskAllocatorKillThreatsBasic::CTaskAllocatorKillThreatsBasic(CPed* threat) :
    m_Threat{threat}
{
}

// 0x69D170
void CTaskAllocatorKillThreatsBasic::AllocateTasks(CPedGroupIntelligence* intel) {
    intel->FlushTasks(intel->GetPedTaskPairs(), nullptr);
    intel->FlushTasks(intel->GetSecondaryPedTaskPairs(), nullptr);

    if (!m_Threat) {
        return;
    }

    auto* const group = &intel->GetPedGroup();
    if (auto* const threatsGroup = m_Threat->GetGroup()) {
        if (threatsGroup == group) {
            NOTSA_LOG_DEBUG("ComputeKillThreatsBasicResponse() - threat ped already in group"); // vanilla
        } else {
            CPed* closest[TOTAL_PED_GROUP_MEMBERS]{};
            ComputeClosestPeds(*group, *threatsGroup, closest);
            for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
                auto* mem = group->GetMembership().GetMember(i);
                if (!mem || mem->IsPlayer()) {
                    continue;
                }
                intel->SetEventResponseTask(
                    mem,
                    CTaskComplexKillPedGroupOnFoot{ CPedGroups::GetGroupId(threatsGroup), closest[i] }
                );
            }
            g_InterestingEvents.Add(CInterestingEvents::GANG_FIGHT, group->GetMembership().GetLeader()); // 0x69D436
        }
    } else { // 0x69D2EB
        for (auto* const mem : group->GetMembership().GetMembers()) {
            if (mem->IsPlayer()) {
                continue;
            }
            intel->SetEventResponseTask(
                mem,
                CTaskComplexSequence{
                    new CTaskComplexKillPedOnFoot{ m_Threat },
                    new CTaskSimpleLookAbout{ CGeneral::GetRandomNumberInRange(1'000u, 2'000u) },
                }
            );
        }
        g_InterestingEvents.Add(CInterestingEvents::GANG_ATTACKING_PED, group->GetMembership().GetLeader()); // 0x69D436
    }
}

// 0x69C7E0
CTaskAllocator* CTaskAllocatorKillThreatsBasic::ProcessGroup(CPedGroupIntelligence* intel) {
    m_Timer.StartIfNotAlready(0);
    if (m_Timer.IsOutOfTime()) {
        m_Timer.Start(5'000);
        AllocateTasks(intel);
    }
    return this;
}

// 0x69C850
void CTaskAllocatorKillThreatsBasic::ComputeClosestPeds(CPedGroup& group1, CPedGroup& group2, CPed** peds) {
    rng::fill(std::span{ peds, (size_t)TOTAL_PED_GROUP_MEMBERS }, nullptr);

    // Compute the distance matrix between the alive non-player members of the two groups
    float distSq[TOTAL_PED_GROUP_MEMBERS][TOTAL_PED_GROUP_MEMBERS];
    rng::fill(distSq[0], distSq[0] + TOTAL_PED_GROUP_MEMBERS * TOTAL_PED_GROUP_MEMBERS, FLT_MAX);
    for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        const auto mem1 = group1.GetMembership().GetMember(i);
        if (!mem1 || !mem1->IsAlive() || mem1->IsPlayer()) {
            continue;
        }
        for (int32 j = 0; j < TOTAL_PED_GROUP_MEMBERS; j++) {
            const auto mem2 = group2.GetMembership().GetMember(j);
            if (!mem2 || !mem2->IsAlive()) {
                continue;
            }
            distSq[i][j] = (mem1->GetPosition() - mem2->GetPosition()).SquaredMagnitude();
        }
    }

    // Greedily match closest pairs (each member matched at most once)
    for (int32 n = 0; n < TOTAL_PED_GROUP_MEMBERS; n++) {
        int32 closestI = -1, closestJ = -1;
        float closestDistSq = FLT_MAX;
        for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
            for (int32 j = 0; j < TOTAL_PED_GROUP_MEMBERS; j++) {
                if (distSq[i][j] < closestDistSq) {
                    closestDistSq = distSq[i][j];
                    closestI = i;
                    closestJ = j;
                }
            }
        }
        if (closestI < 0 || closestJ < 0) {
            break; // No more pairs to match
        }
        // Invalidate the matched row and column
        rng::fill(distSq[closestI], distSq[closestI] + TOTAL_PED_GROUP_MEMBERS, FLT_MAX);
        for (auto& row : distSq) {
            row[closestJ] = FLT_MAX;
        }
        peds[closestI] = group2.GetMembership().GetMember(closestJ);
    }

    // Any unmatched member gets assigned a fallback target: the leader if alive, otherwise any alive member of group2
    auto* fallback = group2.GetMembership().GetLeader();
    if (!fallback || !fallback->IsAlive()) {
        fallback = nullptr;
        for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS - 1; i++) {
            if (const auto mem = group2.GetMembership().GetMember(i); mem && mem->IsAlive()) {
                fallback = mem;
                break;
            }
        }
        if (!fallback) {
            return;
        }
    }
    for (int32 i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        if (group1.GetMembership().GetMember(i) && !peds[i]) {
            peds[i] = fallback;
        }
    }
}

void CTaskAllocatorKillThreatsBasic::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorKillThreatsBasic, 0x870e90, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedInstall(Constructor, 0x69C710);
    RH_ScopedInstall(Destructor, 0x69C780);

    RH_ScopedGlobalInstall(ComputeClosestPeds, 0x69C850);
    RH_ScopedVMTInstall(GetType, 0x69C770);
    RH_ScopedVMTInstall(AllocateTasks, 0x69D170);
    RH_ScopedVMTInstall(ProcessGroup, 0x69C7E0);
}
