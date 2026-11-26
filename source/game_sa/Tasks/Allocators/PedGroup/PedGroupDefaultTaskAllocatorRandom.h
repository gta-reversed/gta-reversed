#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangLeader.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorRandom final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6E90
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        auto& taskPairs = pedGroup->GetIntelligence().GetDefaultPedTaskPairs();

        // Followers. The original doesn't stop at the first match, nor does it check for an existing task.
        for (auto&& [i, tp] : rngv::enumerate(taskPairs | rngv::take(TOTAL_PED_GROUP_FOLLOWERS))) {
            if (!tp.Ped) {
                continue;
            }
            if (ped && tp.Ped != ped) {
                continue;
            }
            const auto task = new CTaskComplexGangFollower{
                pedGroup,
                pedGroup->GetMembership().GetLeader(),
                (uint8)i,
                CVector{ CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i] },
                10.f
            };

            // 0x65EE90 (inlined): bit 2 of the flags follows the group's `m_bMembersEnterLeadersVehicle`
            if (pedGroup->m_bMembersEnterLeadersVehicle) {
                task->m_Flags |= 4;
            } else {
                task->m_Flags &= ~4;
            }
            tp.Task = task;
        }

        // Leader
        auto& leaderTp = taskPairs[CPedGroupMembership::LEADER_MEM_ID];
        if (leaderTp.Ped && (!ped || leaderTp.Ped == ped)) {
            leaderTp.Task = new CTaskComplexGangLeader{ pedGroup };
        }
    }
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::RANDOM; }; // 0x5F6530

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorRandom, 0x86C77C, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6E90);
        RH_ScopedVMTInstall(GetType, 0x5F6530);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorRandom, sizeof(void*)); /* vtable only */
