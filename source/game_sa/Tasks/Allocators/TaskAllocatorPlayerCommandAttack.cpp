#include <TaskComplexLeaveCar.h>
#include <TaskSimpleStandStill.h>
#include <TaskComplexKillPedOnFoot.h>
#include "StdInc.h"

#include "TaskAllocatorPlayerCommandAttack.h"

// 0x69C480
CTaskAllocatorPlayerCommandAttack::CTaskAllocatorPlayerCommandAttack(CPed* ped, int32 groupId, ePedType pedType) : CTaskAllocatorAttack(ped, groupId, FindPlayerPed()) {
    m_PedType = pedType;
}

// 0x69C4D0
void CTaskAllocatorPlayerCommandAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    plugin::CallMethod<0x69C4D0, CTaskAllocatorPlayerCommandAttack*, CPedGroupIntelligence*>(this, intel);

    /*
    if (m_PedType < PED_TYPE_PLAYER1) {
        if (m_Ped0) {
            CTaskAllocatorAttack::AllocateTasks(intel);
        }
        return;
    }

    CPedList pedList;
    pedList.Empty();

    m_Time = CTimer::GetTimeInMS();
    f14 = 3000;
    f18 = 1;
    auto groupMembership = intel->m_pPedGroup->m_groupMembership;
    pedList.BuildListFromGroup_NoLeader(groupMembership);
    if (groupMembership.GetLeader()) {
        CPed* leader = groupMembership.GetLeader();
        if (leader->IsPlayer()) {
            pedList.RemovePedsThatDontListenToPlayer();
        }
    }

    pedList.RemovePedsAttackingPedType(m_PedType);
    CFormation::m_DestinationPeds.BuildListOfPedsOfPedType(m_PedType);
    CFormation::DistributeDestinations_PedsToAttack(&pedList);
    for (auto* ped : pedList.GetPeds()) {
        CTaskComplexSequence sequence;
        if (ped->bInVehicle) {
            if (ped->m_pVehicle) {
                sequence.AddTask(new CTaskComplexLeaveCar(ped->m_pVehicle, 0, 0, true, false));
            }
        }
        CPed* target;
        CFormation::ReturnTargetPedForPed(ped, &target);
        sequence.AddTask(new CTaskComplexKillPedOnFoot(target, -1, 0, 0, 0, 2));
        sequence.AddTask(new CTaskSimpleStandStill(999999, true, false, 8.0f));
        CPedGroupIntelligence::SetTask(ped, &sequence, intel->m_groupTasks, -1, false);
    }
    */
}

// 0x69D110
void CTaskAllocatorPlayerCommandAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    return plugin::CallMethod<0x69D110, CTaskAllocatorPlayerCommandAttack*, CPedGroupIntelligence*>(this, intel);
    CTaskAllocatorAttack::ProcessGroup(intel);
    // todo: delete intel or intel->vfunc();
}
