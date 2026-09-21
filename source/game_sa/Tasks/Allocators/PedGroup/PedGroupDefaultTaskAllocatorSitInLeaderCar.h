#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>
#include "CarEnterExit.h"

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskSimpleCarDrive.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorSitInLeaderCar final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6560
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR; };

    // 0x5F6FC0
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        const auto leader = pedGroup->GetMembership().GetLeader();
        if (!leader) {
            return;
        }
        const auto veh = leader->m_pVehicle;
        if (!veh) {
            return;
        }
        const auto SetPedDefaultTask = [&](CPed* p, int32 i, CTask* task) {
            auto& tp = pedGroup->GetIntelligence().GetDefaultPedTaskPairs()[i];
            if (tp.Ped && (!p || tp.Ped == p)) {
                assert(!tp.Task);
                tp.Task = task;
            } else {
                delete task;
            }
        };
        SetPedDefaultTask(ped, CPedGroupMembership::LEADER_MEM_ID, new CTaskComplexSequence{ // 0x5F703E
            new CTaskComplexEnterCarAsDriver{veh}, // 0x5F703E
            new CTaskSimpleCarDrive{veh} // 0x5F7074
        });
        int32 seat{};
        for (auto&& [i, tp] : rngv::enumerate(pedGroup->GetIntelligence().GetDefaultPedTaskPairs())) {
            if (i == CPedGroupMembership::LEADER_MEM_ID) {
                continue; // Skip the leader's slot
            }
            if (!tp.Ped || seat >= veh->m_nMaxPassengers) {
                continue;
            }
            SetPedDefaultTask(ped, i, new CTaskComplexSequence{ // 0x5F714B
                new CTaskComplexEnterCarAsPassenger{veh, CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat++)}, // 0x5F714B
                new CTaskSimpleCarDrive{veh} // 0x5F7184
            });
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorSitInLeaderCar, 0x86C784, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F6FC0);
        RH_ScopedVMTInstall(GetType, 0x5F6560);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorSitInLeaderCar, sizeof(void*)); /* vtable only */
