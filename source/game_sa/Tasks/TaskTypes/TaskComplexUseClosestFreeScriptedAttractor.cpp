#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractor.h"
#include "TaskComplexUseEffect.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "PedAttractorManager.h"
#include "Scripted2dEffects.h"
#include "InterestingEvents.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"

void CTaskComplexUseClosestFreeScriptedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseClosestFreeScriptedAttractor, 0x86e428, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x6346F0);
    RH_ScopedInstall(Destructor, 0x634720);

    RH_ScopedGlobalInstall(ComputeClosestFreeScriptedEffect, 0x634740);
    RH_ScopedVMTInstall(Clone, 0x636F70);
    RH_ScopedVMTInstall(GetTaskType, 0x634710);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x634730);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x639530);
    RH_ScopedVMTInstall(ControlSubTask, 0x634890);
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(eMoveState ms) :
    m_MoveState{ms}
{
}

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(const CTaskComplexUseClosestFreeScriptedAttractor&) :
    CTaskComplexUseClosestFreeScriptedAttractor{}
{
}

// 0x634740
C2dEffect* CTaskComplexUseClosestFreeScriptedAttractor::ComputeClosestFreeScriptedEffect(const CPed& ped) {
    const CVector& pedPos = ped.GetPosition();

    float closestDistSq = FLT_MAX;
    C2dEffect* closestFx = nullptr;

    // NOTSA: Original looped with raw pointers over ms_effects/ms_userLists/ms_activated (`puVar6 += 9` dwords, `pfVar7 += 0x10` floats, end at 0xC3AAFF).
    for (auto i = 0u; i < NUM_SCRIPTED_2D_EFFECTS; i++) {
        if (!CScripted2dEffects::ms_activated[i]) {
            continue;
        }
        const auto& userList = CScripted2dEffects::ms_userLists[i];
        if (userList.m_bUseList) {
            bool allowed = false;
            for (const auto modelId : userList.m_UserTypes) {
                if (modelId == (int32)ped.GetModelIndex()) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                for (auto s = 0u; s < userList.m_UserTypes.size(); s++) {
                    // 0x6347B3: entry valid only if type slot is -2 and ped-type slot matches this ped's type (6 == COP, cf. KillCriminal CreateFirstSubTask owner check)
                    if (userList.m_UserTypes[s] == -2 && userList.m_UserTypesByPedType[s] == (int32)ped.m_nPedType) {
                        allowed = true;
                        break;
                    }
                }
            }
            if (!allowed) {
                continue;
            }
        }
        auto* const fx = &CScripted2dEffects::ms_effects[i];
        if ((pedPos - fx->m_Pos).SquaredMagnitude() >= closestDistSq) {
            continue;
        }
        // NOTSA: Original wrapped the call below in a 1.0f-scaled temp matrix (CMatrix::SetScale(1.0f)/dtor pair);
        //        `GetPedAttractorManager()` + `HasEmptySlot(fx, nullptr)` is the inlined logic.
        if (!GetPedAttractorManager()->HasEmptySlot(notsa::cast<C2dEffectPedAttractor>(fx), nullptr)) {
            continue;
        }
        closestDistSq = (pedPos - fx->m_Pos).SquaredMagnitude();
        closestFx = fx;
    }
    return closestFx;
}

// 0x639530
CTask* CTaskComplexUseClosestFreeScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    auto* const fx = notsa::cast<C2dEffectPedAttractor>(ComputeClosestFreeScriptedEffect(*ped));
    if (!fx) {
        return nullptr;
    }
    g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_3, ped);
    switch (m_MoveState) {
    case PEDMOVE_RUN: // 0x6395D4: CTaskComplexUseEffectRunning ctor (0x636780 sets vtable 0x86E594)
        return new CTaskComplexUseEffectRunning{ fx, nullptr };
    case PEDMOVE_SPRINT: // 0x63959A: CTaskComplexUseEffectSprinting ctor (0x636860 sets vtable 0x86E5C0)
        return new CTaskComplexUseEffectSprinting{ fx, nullptr };
    default: // 0x639581/0x63960A: walk (4) and any other state fall into the plain CTaskComplexUseEffect ctor (0x6321F0)
        return new CTaskComplexUseEffect{ fx, nullptr };
    }
}
