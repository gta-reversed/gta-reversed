#include "StdInc.h"

#include "TaskAllocatorKillThreatsBasicRandomGroup.h"
#include "TaskSimpleNone.h"
#include "TaskComplexKillPedOnFoot.h"
#include "InterestingEvents.h"

// 0x69D460
void CTaskAllocatorKillThreatsBasicRandomGroup::AllocateTasks(CPedGroupIntelligence* intel) {
    plugin::CallMethod<0x69D460, CTaskAllocatorKillThreatsBasicRandomGroup*, CPedGroupIntelligence*>(this, intel);

    /*
    CPedGroupIntelligence::FlushTasks(&intel->m_groupTasks, nullptr);
    CPedGroupIntelligence::FlushTasks(&intel->m_groupTasks[8], nullptr);
    auto memberId = intel->m_pPedGroup;
    auto groupMembership = &intel->m_pPedGroup->m_groupMembership;
    if (!m_Ped0)
        return;

    auto groups = CPedGroups::GetPedsGroup(m_Ped0);
    if (groups) {
        auto groupId = CPedGroups::GetGroupId(groups);
        auto v5 = &CPedGroups::ms_groups[groupId];
        if (v5 == memberId) {
            DEV_LOG("ComputeKillThreatsBasicResponse() - threat ped already in group");
        } else {
            CTaskAllocatorKillThreatsBasic::ComputeClosestPeds(memberId, v5, ped);
            for (auto memberIda = 0; memberIda < 8; ++memberIda) {
                auto member = groupMembership->GetMember(memberIda);
                if (!member || member->IsPlayer())
                    continue;

                if (!member->GetActiveWeapon().IsTypeMelee() || m_Ped0->GetActiveWeapon().IsTypeMelee()) {
                    new CTaskComplexKillPedGroupOnFoot(GroupId, ped[memberIda]);
                    CTaskSimpleNone v23;
                    intel->SetEventResponseTask(v8, member, 1, v28, 0, &v23, -1);
                } else {
                    new CTaskComplexSeekCoverUntilTargetDead(&v27[1], GroupId);
                    CTaskSimpleNone v25;
                    intel->SetEventResponseTask(&v25, member, 1, &v27[1], 0, &v25, -1);
                }
            }
            g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_24, groupMembership.GetLeader());
        }
    } else {
        for (auto memberIdb = 0; memberIdb < 8; ++memberIdb) {
            auto member = groupMembership->GetMember(memberIdb);
            if (!member || member->IsPlayer())
                continue;

            if (!member->GetActiveWeapon().IsTypeMelee() || m_Ped0->GetActiveWeapon().IsTypeMelee()) {
                CTaskComplexSequence sequence;
                sequence.AddTask(new CTaskComplexKillPedOnFoot(m_Ped0, -1, 0, 0, 0, 1));
                sequence.AddTask(new CTaskSimpleLookAbout(CGeneral::GetRandomNumberInRange(1000, 2000)));
                CTaskSimpleNone v27;
                intel->SetEventResponseTask(member, 1, sequence, 0, &v27, -1);
            } else {
                auto task = new CTaskComplexSeekCoverUntilTargetDead(m_Ped0);
                CTaskSimpleNone v23;
                intel->SetEventResponseTask(member, true, &task, false, &v23, -1);
            }
        }
        g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_23, groupMembership->GetLeader());
    }
    */
}
