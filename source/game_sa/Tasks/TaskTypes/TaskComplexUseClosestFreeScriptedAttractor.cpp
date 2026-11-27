#include "StdInc.h"
#include "TaskComplexUseClosestFreeScriptedAttractor.h"
#include "TaskComplexUseEffect.h"
#include "TaskComplexUseEffectRunning.h"
#include "TaskComplexUseEffectSprinting.h"
#include "Scripted2dEffects.h"
#include "PedAttractorManager.h"
#include "InterestingEvents.h"

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

CTaskComplexUseClosestFreeScriptedAttractor::CTaskComplexUseClosestFreeScriptedAttractor(const CTaskComplexUseClosestFreeScriptedAttractor& o) :
    CTaskComplexUseClosestFreeScriptedAttractor{ o.m_MoveState }
{
}

// 0x634740
C2dEffectPedAttractor* __stdcall CTaskComplexUseClosestFreeScriptedAttractor::ComputeClosestFreeScriptedEffect(const CPed& ped) {
    constexpr int32 USER_BY_PED_TYPE = -2; //!< `m_UserTypes` value: the user is given by the ped type at the same index

    const auto CanUse = [&](const tUserList& userList) {
        if (!userList.m_bUseList || rng::contains(userList.m_UserTypes, (int32)ped.GetModelIndex())) {
            return true;
        }
        return rng::any_of(rngv::zip(userList.m_UserTypes, userList.m_UserTypesByPedType), [&](const auto& user) {
            const auto& [userType, pedType] = user;
            return userType == USER_BY_PED_TYPE && pedType == ped.m_nPedType;
        });
    };

    C2dEffectPedAttractor* closestFx{};
    float                  closestDistSq{ FLT_MAX };
    for (auto&& [i, userList] : rngv::enumerate(CScripted2dEffects::ms_userLists)) {
        if (!CScripted2dEffects::ms_activated[i] || !CanUse(userList)) {
            continue;
        }
        auto* const fx     = CScripted2dEffects::GetEffect((int32)i);
        const auto  distSq = CVector::DistSqr(ped.GetPosition(), fx->m_Pos);
        if (distSq < closestDistSq && GetPedAttractorManager()->HasEmptySlot(fx, nullptr)) {
            closestDistSq = distSq;
            closestFx     = fx;
        }
    }
    return closestFx;
}

// 0x639530
CTask* CTaskComplexUseClosestFreeScriptedAttractor::CreateFirstSubTask(CPed* ped) {
    auto* const fx = ComputeClosestFreeScriptedEffect(*ped);
    if (!fx) {
        return nullptr;
    }
    g_InterestingEvents.Add(CInterestingEvents::INTERESTING_EVENT_3, ped);
    switch (m_MoveState) {
    case PEDMOVE_RUN: // 0x6395D4
        return new CTaskComplexUseEffectRunning{ fx, nullptr };
    case PEDMOVE_SPRINT: // 0x63959A
        return new CTaskComplexUseEffectSprinting{ fx, nullptr };
    case PEDMOVE_WALK: // 0x63960A
    default:           // 0x639581
        return new CTaskComplexUseEffect{ fx, nullptr };
    }
}
