#include "StdInc.h"

#include "TaskComplexLeaveCar.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexKillPedOnFoot.h"
#include "TaskComplexLeaveCar.h"

#include "TaskAllocatorPlayerCommandAttack.h"

// 0x69C480
CTaskAllocatorPlayerCommandAttack::CTaskAllocatorPlayerCommandAttack(CPed* target, int32 groupTargetID, ePedType pedTypeToAttack) :
    CTaskAllocatorAttack{target, groupTargetID, FindPlayerPed()},
    m_PedTypeToAttack{pedTypeToAttack}
{ }

// 0x69C4D0
void CTaskAllocatorPlayerCommandAttack::AllocateTasks(CPedGroupIntelligence* intel) {
    if (IsPedTypePlayer(m_PedTypeToAttack)) {
        if (m_Target) {
            CTaskAllocatorAttack::AllocateTasks(intel);
        }
        return;
    }

    m_NextUpdateTimer.Start(3'000);

    auto* const group = &intel->GetPedGroup();

    CPedList attackers{};
    attackers.BuildListFromGroup_NoLeader(group->GetMembership()); // 0x69C52C
    if (auto* const leader = group->GetMembership().GetLeader()) { // 0x69C537
        if (leader->IsPlayer()) {
            attackers.RemovePedsThatDontListenToPlayer();
        }
    }
    attackers.RemovePedsAttackingPedType(m_PedTypeToAttack); // 0x69C567

    CFormation::m_DestinationPeds.BuildListOfPedsOfPedType(m_PedTypeToAttack);
    CFormation::DistributeDestinations_PedsToAttack(attackers);

    for (auto* const p : attackers.GetPeds()) { // 0x69C6C8
        CTaskComplexSequence tseq{};

        // 0x69C5C9
        if (p->IsInVehicle()) {
            tseq.AddTask(new CTaskComplexLeaveCar{ p->m_pVehicle, 0, 0, true, false });
        }

        // 0x69C60F
        CPed* target;
        CFormation::ReturnTargetPedForPed(p, &target);
        tseq.AddTask(new CTaskComplexKillPedOnFoot{ target, -1, 0, 0, 0, 2 });

        // 0x69C67F
        tseq.AddTask(new CTaskSimpleStandStill{ 999'999, true });

        // 0x69C6A8
        intel->SetEventResponseTask(p, tseq);
    }
}

// 0x69D110
CTaskAllocator* CTaskAllocatorPlayerCommandAttack::ProcessGroup(CPedGroupIntelligence* intel) {
    CTaskAllocatorAttack::ProcessGroup(intel);
    return IsFinished(intel)
        ? nullptr
        : this;
}

void CTaskAllocatorPlayerCommandAttack::InjectHooks() {
    RH_ScopedVirtualClass(CTaskAllocatorPlayerCommandAttack, 0x870e78, 6);
    RH_ScopedCategory("Tasks/Allocators");

    RH_ScopedVMTInstall(AllocateTasks, 0x69C4D0);
    RH_ScopedVMTInstall(GetType, 0x69C4C0);
    RH_ScopedVMTInstall(ProcessGroup, 0x69D110);
}
