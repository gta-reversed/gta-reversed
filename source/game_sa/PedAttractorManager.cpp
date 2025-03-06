#include "StdInc.h"

#include "PedAttractorManager.h"
#include <TaskTypes/TaskComplexUseAttractor.h>
#include <TaskTypes/TaskComplexWaitAtAttractor.h>
#include "Scripted2dEffects.h"
#include "PedAttractor.h"
#include "PedAtmAttractor.h"
#include "PedSeatAttractor.h"
#include "PedStopAttractor.h"
#include "PedPizzaAttractor.h"
#include "PedShelterAttractor.h"
#include "PedTriggerScriptAttractor.h"
#include "PedLookAtAttractor.h"
#include "PedScriptedAttractor.h"
#include "PedParkAttractor.h"
#include "PedStepAttractor.h"

void CPedAttractorManager::InjectHooks() {
    RH_ScopedClass(CPedAttractorManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RestoreStuffFromMem, 0x5EF710);
    //RH_ScopedInstall(BroadcastArrival<CPedAttractor>, 0x5EF920); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(BroadcastArrival, "", 0x5EFE20, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(BroadcastDeparture<CPedAttractor>, 0x5EC660); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(BroadcastDeparture, "", 0x5EC980, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(DeRegisterPed<CPedAttractor>, 0x5EC740); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(DeRegisterPed, "", 0x5EC850, bool(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    //RH_ScopedInstall(RemoveEffect<CPedAttractor>, 0x5EB5F0); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(RemoveEffect, "", 0x5EBA30, bool(CPedAttractorManager::*)(const C2dEffectPedAttractor* fx));
    //RH_ScopedInstall(IsPedRegistered<CPedAttractor>, 0x5EB640); // Can't hook this because template function will (probably) be wrong
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "Ped", 0x5EBCB0, bool(CPedAttractorManager::*)(CPed*));
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "Effect", 0x5EBD70, bool(CPedAttractorManager::*)(CPed*, const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(IsPedRegisteredWithEffect<CPedAttractor>, 0x5EB690); // Can't hook this because template function will (probably) be wrong
    //RH_ScopedInstall(FindAssociatedAttractor<CPedAttractor>, 0x5EB6F0); // Can't hook this because template function will (probably) be wrong
    RH_ScopedInstall(HasQueueTailArrivedAtSlot, 0x5EBBA0);
    RH_ScopedInstall(HasEmptySlot, 0x5EBB00);
    //RH_ScopedInstall(GetPedUsingEffect<CPedAttractor>, 0x5EB740);
    RH_ScopedOverloadedInstall(GetPedUsingEffect, "", 0x5EBE50, CPed*(CPedAttractorManager::*)(const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(GetRelevantAttractor<CPedAttractor>, 0x5EB7B0);
    RH_ScopedOverloadedInstall(GetRelevantAttractor, "", 0x5EBF50, const CPedAttractor*(CPedAttractorManager::*)(const CPed*, const C2dEffectPedAttractor*, const CEntity*));
    //RH_ScopedInstall(ComputeEffectPos, 0x5E96C0); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectUseDir, 0x5E96E0); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectQueueDir, 0x5E9730); // Different signature, cant hook
    //RH_ScopedInstall(ComputeEffectForwardDir, 0x5E9780); // Different signature, cant hook
    //RH_ScopedInstall(RegisterPed<CPedAttractor>, 0x5EF980); // Can't hook this because template function will (probably) be wrong
    RH_ScopedInstall(RegisterPedWithAttractor, 0x5EFCA0);
    RH_ScopedInstall(IsApproachable, 0x5EA220);
    RH_ScopedGlobalInstall(GetPedAttractorManager, 0x5EE190);
}

CPedAttractorManager* GetPedAttractorManager() {
    static CPedAttractorManager p; // NOTSA: They used `new`.. Uh, and also (BUG): they never freed the pointer originally xD
    return &p;
}

// 0x? 0x5EF710 ?
void CPedAttractorManager::RestoreStuffFromMem() {
    NOTSA_UNREACHABLE("Unused function");
}

// 0x5EFE20
bool CPedAttractorManager::BroadcastArrival(CPed* ped, CPedAttractor* attractor) {
    return attractor && IsPedRegisteredWithEffect(ped) && VisitAttractorsOfType(attractor->GetType(), [&](auto&& attractors) {
        return BroadcastArrival(ped, attractor, attractors);
    });
}

// 0x5EC980
bool CPedAttractorManager::BroadcastDeparture(CPed* ped, CPedAttractor* attractor) {
    return attractor && IsPedRegisteredWithEffect(ped) && VisitAttractorsOfType(attractor->GetType(), [&](auto&& attractors) {
        return BroadcastDeparture(ped, attractor, attractors);
    });
}

// 0x5EC850
bool CPedAttractorManager::DeRegisterPed(CPed* ped, CPedAttractor* attractor) {
    return attractor && IsPedRegisteredWithEffect(ped) && VisitAttractorsOfType(attractor->GetType(), [&](auto&& attractors) {
        return DeRegisterPed(ped, attractor, attractors);
    });
}

// 0x5EBA30
// all xrefs are dead functions
bool CPedAttractorManager::RemoveEffect(const C2dEffectPedAttractor* fx) {
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return RemoveEffect(fx, attractors);
    });
}

// 0x5EBCB0
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped) {
    return IsPedRegistered(ped, m_Seats)
        || IsPedRegistered(ped, m_ATMs)
        || IsPedRegistered(ped, m_Stops)
        || IsPedRegistered(ped, m_Pizzas)
        || IsPedRegistered(ped, m_Shelters)
        || IsPedRegistered(ped, m_TriggerScripts)
        || IsPedRegistered(ped, m_LookAts)
        || IsPedRegistered(ped, m_Scripted)
        || IsPedRegistered(ped, m_Parks)
        || IsPedRegistered(ped, m_Steps);
}

// 0x5EBD70
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity) {
    return IsPedRegisteredWithEffect(ped, fx, entity, m_Seats)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_ATMs)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Stops)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Pizzas)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Shelters)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_TriggerScripts)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_LookAts)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Scripted)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Parks)
        || IsPedRegisteredWithEffect(ped, fx, entity, m_Steps);
}

// notsa
CPedAttractor* CPedAttractorManager::FindAssociatedAttractor(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return notsa::cast_if_present<CPedAttractor>(FindAssociatedAttractor(fx, entity, attractors));
    });
}

// 0x5EBBA0
bool CPedAttractorManager::HasQueueTailArrivedAtSlot(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    assert(notsa::isa<C2dEffectPedAttractor>(fx)); // NOTE: Original function just returned `false`

    const auto* const attractor = FindAssociatedAttractor(fx, entity);
    if (!attractor) {
        return true;
    }
    const auto* const pedAtQueueTail = attractor->GetTailOfQueue();
    if (!pedAtQueueTail) {
        return true;
    }
    if (attractor->GetHeadOfQueue() == attractor->GetTailOfQueue()) { // Queue has 1 ped only
        return pedAtQueueTail->GetTaskManager().Find<CTaskComplexUseAttractor>(true) != nullptr;
    }
    const auto* const tWaitAtAttractor = pedAtQueueTail->GetTaskManager().Find<CTaskComplexWaitAtAttractor>(true);
    return tWaitAtAttractor
        && tWaitAtAttractor->m_slot == attractor->GetSizeOfQueue() - 1;
}

// 0x5EBB00
bool CPedAttractorManager::HasEmptySlot(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    assert(notsa::isa<C2dEffectPedAttractor>(fx)); // NOTE: Original function just returned `false`

    const auto* const attractor = FindAssociatedAttractor(fx, entity);
    return !attractor || attractor->HasEmptySlot();
}

// 0x5EBE50
CPed* CPedAttractorManager::GetPedUsingEffect(const C2dEffectPedAttractor* fx, const CEntity* entity) {
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return GetPedUsingEffect(fx, entity, attractors);
    });
}

// 0x5EBF50
const CPedAttractor* CPedAttractorManager::GetRelevantAttractor(const CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity) {
    const auto SearchIn = [this, ped, fx, entity](auto&& attractors) {
        return GetRelevantAttractor(ped, fx, entity, attractors);
    };
    const CPedAttractor* p;
    if (   (p = SearchIn(m_Seats))
        || (p = SearchIn(m_ATMs))
        || (p = SearchIn(m_Stops))
        || (p = SearchIn(m_Pizzas))
        || (p = SearchIn(m_Shelters))
        || (p = SearchIn(m_TriggerScripts))
        || (p = SearchIn(m_LookAts))
        || (p = SearchIn(m_Scripted))
        || (p = SearchIn(m_Parks))
        || (p = SearchIn(m_Steps))
    ) {
        return p;
    }
    return nullptr;
}

// 0x5EFCA0
CPedAttractor* CPedAttractorManager::RegisterPedWithAttractor(CPed* ped, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState ms) {
    assert(fx->m_Type == EFFECT_ATTRACTOR); // NOTE: Original function just returned `false`

    if (CScripted2dEffects::GetIndex(fx) < 0 && !CEventAttractor::IsEffectActive(entity, fx)) {
        return nullptr;
    }
    if (IsPedRegisteredWithEffect(ped, fx, entity)) {
        return nullptr;
    }
    return VisitAttractorsOfType(fx->m_nAttractorType, [&](auto&& attractors) {
        return notsa::cast<CPedAttractor>(RegisterPed(ped, fx, entity, ms, attractors));
    });
}

// 0x5EA220
bool CPedAttractorManager::IsApproachable(C2dEffectPedAttractor* fx, const CMatrix& mat, int32 slot, CPed* ped) {
    const CVector pedPos = ped->GetPosition();

    if (fx->m_nAttractorType == PED_ATTRACTOR_SHELTER) {
        return CWorld::GetIsLineOfSightClear(pedPos, ComputeEffectPos(fx, mat), true, false, false, false);
    }

    const auto fxPos = ComputeEffectPos(fx, mat);
    const auto fxFwd = ComputeEffectForwardDir(fx, mat);
    return (pedPos - fxPos).Dot(fxFwd) > 0.f // note: combined the 2 dot products here
        && CVector2D::DistSqr(pedPos, fxPos) >= sq(0.5f)
        && CPedGeometryAnalyser::IsWanderPathClear(pedPos, fxPos, 2.f, 0) == CPedGeometryAnalyser::WanderPathClearness::CLEAR;
}
