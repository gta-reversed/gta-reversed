#include "StdInc.h"

#include "PedGroup.h"
#include <TaskSimpleCarSetPedOut.h>
#include <TaskComplexFollowLeaderInFormation.h>

//! @returns Distance of the furthers member from the leader
float CPedGroup::FindDistanceToFurthestMember() {
    return plugin::CallMethodAndReturn<float, 0x5FB010, CPedGroup*>(this);
    /*
    const auto leader = GetMembership().GetLeader();
    for (const auto& mem : GetMembership().GetMembers(true)) {

    }*/
}

// 0x5FB0A0
float CPedGroup::FindDistanceToNearestMember(CPed** ppOutNearestMember) {
    const auto [nearest, distSq] = GetMembership().FindClosestFollowerToLeader();
    if (nearest) {
        if (ppOutNearestMember) {
            *ppOutNearestMember = nearest;
        }
        return std::sqrt(distSq);
    }
    return 1.0e10;
}

// 0x5FACD0
CPed* CPedGroup::GetClosestGroupPed(CPed* ped, float* pOutDistSq) {
    const auto [closest, distSq] = GetMembership().GetMemberClosestTo(ped);
    if (closest) {
        if (pOutDistSq) {
            *pOutDistSq = distSq;
        }
    }
    return closest;
}

// 0x5FB790
void CPedGroup::Flush() {
    m_groupMembership.Flush();
    m_groupIntelligence.Flush();
    m_bIsMissionGroup = false;
}

// 0x5F7DB0
bool CPedGroup::IsAnyoneUsingCar(const CVehicle* vehicle) {
    assert(vehicle);

    for (auto& mem : m_groupMembership.GetMembers()) {
        if (mem.GetVehicleIfInOne() == vehicle) {
            return true;
        }
        // I did a slight change here.
        // Game originally checked both EnterAsDriver and EnterAsPassenger tasks
        // but that makes no sense, as the ped can't have both tasks at the same time (I hope)
        // So the function below returns the target vehicle of the first task found
        if (mem.GetIntelligence()->GetEnteringVehicle() == vehicle) {
            return true;
        }
    }
    return false;
}

// 0x5F7CC0
void CPedGroup::PlayerGaveCommand_Attack(CPed* playerPed, CPed* target) {
    if (!m_groupIntelligence.AddEvent(CEventGroupEvent{ playerPed, new CEventPlayerCommandToGroup{PLAYER_GROUP_COMMAND_ATTACK, target} })) {
        return;
    }
    if (target && target->m_nPedType != PED_TYPE_GANG2) {
        target->Say(target->IsGangster() ? 147 : 148);
    }
}

void CPedGroup::PlayerGaveCommand_Gather(CPed* ped) {
    plugin::CallMethod<0x5FAB60, CPedGroup*, CPed*>(this, ped);
}

void CPedGroup::Process() {
    m_groupMembership.Process();
    m_groupIntelligence.Process();
}

void CPedGroup::RemoveAllFollowers() {
    GetMembership().RemoveAllFollowers(false);
}

void CPedGroup::Teleport(const CVector& pos) {
    if (const auto leader = GetMembership().GetLeader()) {
        leader->Teleport(pos, false);
    }

    if (const auto oevent = m_groupIntelligence.m_pOldEventGroupEvent) {
        if (oevent->GetEventType() == EVENT_LEADER_ENTRY_EXIT) {
            return;
        }
    }

    // Set *followers* out of the vehicle
    for (auto& flwr : GetMembership().GetMembers(false)) {
        if (!flwr.IsAlive() || !flwr.bInVehicle || flwr.IsCreatedByMission()) {
            continue;
        }
        CTaskSimpleCarSetPedOut{ flwr.m_pVehicle, (eTargetDoor)CCarEnterExit::ComputeTargetDoorToExit(flwr.m_pVehicle, &flwr), false }.ProcessPed(&flwr);
    }

    // Teleport *followers*
    const auto& offsets = CTaskComplexFollowLeaderInFormation::ms_offsets.offsets;
    for (auto&& [offsetIdx, flwr] : notsa::enumerate(GetMembership().GetMembers(false))) {
        if (!flwr.IsAlive()) {
            continue;
        }
        flwr.Teleport(pos + CVector{ offsets[offsetIdx] }, false);
        flwr.PositionAnyPedOutOfCollision();
        flwr.GetTaskManager().AbortFirstPrimaryTaskIn({ TASK_PRIMARY_PHYSICAL_RESPONSE, TASK_PRIMARY_EVENT_RESPONSE_TEMP, TASK_PRIMARY_EVENT_RESPONSE_NONTEMP }, &flwr);
    }
}

int32 CPedGroup::GetId() const {
    return CPedGroups::GetGroupId(this);
}

bool CPedGroup::IsActive() const {
    return CPedGroups::ms_activeGroups[GetId()];
}

void CPedGroup::InjectHooks() {
    RH_ScopedClass(CPedGroup);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x5FC150);
    RH_ScopedInstall(Destructor, 0x5FC190);

    RH_ScopedInstall(Teleport, 0x5F7AD0);
    RH_ScopedInstall(PlayerGaveCommand_Gather, 0x5FAB60, {.reversed = false});
    RH_ScopedInstall(PlayerGaveCommand_Attack, 0x5F7CC0);
    RH_ScopedInstall(IsAnyoneUsingCar, 0x5F7DB0);
    RH_ScopedInstall(GetClosestGroupPed, 0x5FACD0);
    RH_ScopedInstall(FindDistanceToFurthestMember, 0x5FB010, {.reversed = false});
    RH_ScopedInstall(FindDistanceToNearestMember, 0x5FB0A0);
    RH_ScopedInstall(Flush, 0x5FB790);
    RH_ScopedInstall(Process, 0x5FC7E0);
    RH_ScopedInstall(RemoveAllFollowers, 0x5FB7D0);
}
