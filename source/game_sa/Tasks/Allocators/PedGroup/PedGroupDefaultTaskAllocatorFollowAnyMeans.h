#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

#include "TaskComplexWanderGang.h"
#include "TaskComplexFollowLeaderAnyMeans.h"
#include "TaskComplexFollowLeaderInFormation.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorFollowAnyMeans final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6470
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::FOLLOW_ANY_MEANS; };

    // 0x5F6B40
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        const auto leader = pedGroup->GetMembership().GetLeader();
        for (auto&& [i, tp] : notsa::enumerate(pedGroup->GetIntelligence().GetDefaultPedTaskPairs())) {
            if (!tp.m_Ped) {
                continue;
            }
            if (ped && tp.m_Ped != ped) {
                continue;
            }
            assert(!tp.m_Task);
            if (ped == leader) {
                tp.m_Task = new CTaskComplexFollowLeaderAnyMeans{
                    pedGroup,
                    leader,
                    CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]}
                };
            } else {
                tp.m_Task = new CTaskComplexWanderGang{
                    PEDMOVE_WALK,
                    CGeneral::RandomNodeHeading(),
                    5'000,
                    true,
                    0.5f
                };
            }
            if (notsa::IsFixBugs()) {
                if (ped) {
                    break; // There are no duplicate entries in the array, so we can stop now
                }
            }
        }
    }

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorFollowAnyMeans, 0x86C75C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6B40);
        RH_ScopedVMTInstall(GetType, 0x5F6470);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorFollowAnyMeans, sizeof(void*)); /* vtable only */
