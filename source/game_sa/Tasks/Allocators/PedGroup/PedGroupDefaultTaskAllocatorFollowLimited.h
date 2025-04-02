#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexFollowLeaderInFormation.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorFollowLimited final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F64A0
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::FOLLOW_LIMITED; };

    // 0x5F6C70
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        const auto leader = pedGroup->GetMembership().GetLeader();
        for (auto&& [i, tp] : rngv::enumerate(pedGroup->GetIntelligence().GetDefaultPedTaskPairs())) {
            if (!tp.m_Ped) {
                continue;
            }
            if (ped && tp.m_Ped != ped) {
                continue;
            }
            assert(!tp.m_Task);
            if (ped == leader) {
                tp.m_Task = new CTaskComplexFollowLeaderInFormation{
                    pedGroup,
                    leader,
                    CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]},
                    10.f
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
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorFollowLimited, 0x86C764, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6C70);
        RH_ScopedVMTInstall(GetType, 0x5F64A0);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorFollowLimited, sizeof(void*)); /* vtable only */
