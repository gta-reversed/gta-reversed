#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"

#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskComplexGangFollower.h"
#include "TaskComplexGangLeader.h"
#include "PedGroup.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorRandom final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6E90
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        for (auto&& [i, tp] : rngv::enumerate(pedGroup->GetIntelligence().GetDefaultPedTaskPairs())) {
            if (!tp.Ped || ped && tp.Ped != ped) {
                continue;
            }
            assert(!tp.Task);
            tp.Task = new CTaskComplexGangFollower{
                pedGroup,
                pedGroup->GetMembership().GetLeader(),
                (uint8)i,
                CVector{CTaskComplexFollowLeaderInFormation::ms_offsets.Offsets[i]},
                10.f
            }; // 0x5F6ED7
            pedGroup->m_bMembersEnterLeadersVehicle = ped != nullptr; // 0x5F6F3D
        }
        if (const auto leader = pedGroup->GetMembership().GetLeader()) { // 0x5F6F64
            if (!ped || leader == ped) {
                auto& tp = pedGroup->GetIntelligence().GetDefaultPedTaskPairs()[CPedGroupMembership::LEADER_MEM_ID];
                tp.Task = new CTaskComplexGangLeader{ pedGroup }; // 0x5F6F76
            }
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
