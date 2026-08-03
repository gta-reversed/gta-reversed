#pragma once

#include <Base.h>
#include <PluginBase.h>
#include <reversiblehooks/ReversibleHooks.h>

#include "./PedGroupDefaultTaskAllocator.h"
#include "TaskComplexSequence.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskSimpleRunTimedAnim.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleChat.h"

class NOTSA_EXPORT_VTABLE CPedGroupDefaultTaskAllocatorChat final : public CPedGroupDefaultTaskAllocator {
public:
    /* no virtual destructor */

    // 0x5F6500
    ePedGroupDefaultTaskAllocatorType GetType() const override { return ePedGroupDefaultTaskAllocatorType::CHAT; };

    // 0x5F8180
    // This function was quite heavily refactored, but the logic should be the same
    void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {
        const auto members = rng::to<std::vector>(pedGroup->GetMembership().GetMembers());
        for (auto* const mem : members) {
            CTaskComplexSequence tseq{
                new CTaskComplexGoToPointAndStandStill{ // 0x5F82B8
                    PEDMOVE_WALK,
                    mem->GetPosition(),
                    0.5f,
                },
                new CTaskSimpleAchieveHeading{ // 0x5F8311
                    mem->m_fCurrentRotation
                }
            };
            tseq.SetRepeatMode(true); // 0x5F8338 (I assume this is what that is?)

            const auto  time = CGeneral::GetRandomNumberInRange(3000, 6000);
            auto* const partner = CGeneral::RandomChoice(members);
            if (notsa::IsFixBugs() ? mem != partner : mem == partner) { // BUGFIX: I mean, it makes more sense to chat with someone else than with yourself?
                tseq.AddTask(new CTaskSimpleChat{ (uint32)(time) });
            } else {
                tseq.AddTask(new CTaskSimpleStandStill{ time });
            }

            pedGroup->GetIntelligence().SetDefaultTask(ped, tseq); // Originally added after the loop, but I see no reason to do that
        }
    };

public:
    static inline void InjectHooks() {
        RH_ScopedVirtualClass(CPedGroupDefaultTaskAllocatorChat, 0x86C774, 2);
        RH_ScopedCategory("Tasks/Allocators/PedGroup");

        RH_ScopedVMTInstall(AllocateDefaultTasks, 0x5F8180);
        RH_ScopedVMTInstall(GetType, 0x5F6500);
    }
};
VALIDATE_SIZE(CPedGroupDefaultTaskAllocatorChat, sizeof(void*)); /* vtable only */
