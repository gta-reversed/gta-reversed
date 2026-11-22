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
// 0x69C850
void CTaskAllocatorKillThreatsBasic::ComputeClosestPeds(CPedGroup& group1, CPedGroup& group2, CPed** peds) {
    auto& ms1 = group1.GetMembership();
    auto& ms2 = group2.GetMembership();

    std::fill_n(peds, TOTAL_PED_GROUP_MEMBERS, nullptr);

    // Squared distance of every (alive, non-player) member of group1 to every alive member of group2
    float distSq[TOTAL_PED_GROUP_MEMBERS][TOTAL_PED_GROUP_MEMBERS];
    std::fill_n(&distSq[0][0], TOTAL_PED_GROUP_MEMBERS * TOTAL_PED_GROUP_MEMBERS, FLT_MAX);
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        auto* const m1 = ms1.GetMember(i);
        if (!m1 || !m1->IsAlive() || m1->IsPlayer()) {
            continue;
        }
        for (auto j = 0; j < TOTAL_PED_GROUP_MEMBERS; j++) {
            auto* const m2 = ms2.GetMember(j);
            if (!m2 || !m2->IsAlive()) {
                continue;
            }
            const auto d = m1->GetPosition() - m2->GetPosition();
            distSq[i][j] = d.z * d.z + d.x * d.x + d.y * d.y; // Same summation order as the original
        }
    }

    // Greedily pair the closest remaining (member, threat) pairs - each member and each threat is used at most once
    for (auto n = 0; n < TOTAL_PED_GROUP_MEMBERS; n++) {
        auto closestDistSq = FLT_MAX;
        auto ci = -1, cj = -1;
        for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
            for (auto j = 0; j < TOTAL_PED_GROUP_MEMBERS; j++) {
                if (distSq[i][j] < closestDistSq) {
                    closestDistSq = distSq[i][j];
                    ci            = i;
                    cj            = j;
                }
            }
        }
        if (ci < 0 || cj < 0) {
            continue;
        }
        for (auto k = 0; k < TOTAL_PED_GROUP_MEMBERS; k++) {
            distSq[ci][k] = FLT_MAX;
            distSq[k][cj] = FLT_MAX;
        }
        peds[ci] = ms2.GetMember(cj);
    }

    // Members left without a target attack the threat group's leader, or if it's dead, its first alive follower
    CPed* fallback = ms2.GetLeader();
    if (!fallback || !fallback->IsAlive()) {
        fallback = nullptr;
        for (auto i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            auto* const m = ms2.GetMember(i);
            if (m && m->IsAlive()) {
                fallback = m;
                break;
            }
        }
        if (!fallback) {
            return;
        }
    }
    for (auto i = 0; i < TOTAL_PED_GROUP_MEMBERS; i++) {
        if (ms1.GetMember(i) && !peds[i]) {
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
