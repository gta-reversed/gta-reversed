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
        CPed* const leader = pedGroup->GetMembership().GetLeader();
        if (!leader) {
            return;
        }
        CVehicle* const veh = leader->m_pVehicle; // NOTE: Not `GetVehicleIfInOne()`, the original doesn't check `bInVehicle`
        if (!veh) {
            return;
        }
        auto&      taskPairs      = pedGroup->GetIntelligence().GetDefaultPedTaskPairs();
        const auto SetDefaultTask = [&](int32 memId, CTask* task) {
            auto& tp = taskPairs[memId];
            if (tp.Ped && (!ped || tp.Ped == ped)) {
                tp.Task = task;
            } else {
                delete task;
            }
        };
        SetDefaultTask(CPedGroupMembership::LEADER_MEM_ID, new CTaskComplexSequence{
            new CTaskComplexEnterCarAsDriver{ veh },
            new CTaskSimpleCarDrive{ veh }
        });
        int32 seat{};
        for (int32 memId = 0; memId < CPedGroupMembership::LEADER_MEM_ID; memId++) { // 0x5F70E0
            if (!pedGroup->GetMembership().GetMember(memId) || seat >= veh->m_nMaxPassengers) {
                continue;
            }
            SetDefaultTask(memId, new CTaskComplexSequence{
                new CTaskComplexEnterCarAsPassenger{ veh, CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(veh, seat) },
                new CTaskSimpleCarDrive{ veh }
            });
            seat++;
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
