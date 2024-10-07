#include "StdInc.h"

#include "PedAttractorManager.h"

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

CPedAttractorManager* GetPedAttractorManager() {
    return plugin::CallAndReturn<CPedAttractorManager*, 0x5EE190>();
}

void CPedAttractorManager::InjectHooks() {
    RH_ScopedClass(CPedAttractorManager);
    RH_ScopedCategoryGlobal();

    // RH_ScopedOverloadedInstall(BroadcastArrival, "array", 0x5EF920, void(CPedAttractorManager::*)(CPed*, CPedAttractor*, SArray<CPedAttractor*>&));
    RH_ScopedOverloadedInstall(BroadcastArrival, "", 0x5EFE20, void(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    // RH_ScopedOverloadedInstall(BroadcastDeparture, "array", 0x5EC660, void(CPedAttractorManager::*)(CPed*, CPedAttractor*, SArray<CPedAttractor*>&));
    RH_ScopedOverloadedInstall(BroadcastDeparture, "", 0x5EC980, void(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    // RH_ScopedOverloadedInstall(DeRegisterPed, "array", 0x5EC740, void(CPedAttractorManager::*)(CPed*, CPedAttractor*, SArray<CPedAttractor*>&));
    RH_ScopedOverloadedInstall(DeRegisterPed, "", 0x5EC850, void(CPedAttractorManager::*)(CPed*, CPedAttractor*));
    // RH_ScopedOverloadedInstall(RemoveEffect, "array", 0x5EB5F0, void(CPedAttractorManager::*)(const C2dEffect* effect, const SArray<CPedAttractor*>& array));
    // RH_ScopedOverloadedInstall(RemoveEffect, "", 0x5EBA30, void(CPedAttractorManager::*)(const C2dEffect* effect));
    RH_ScopedInstall(IsPedRegistered, 0x5EB640, { .reversed = false });
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "ped", 0x5EBCB0, bool(CPedAttractorManager::*)(CPed*));
    RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "+effect", 0x5EBD70, bool(CPedAttractorManager::*)(CPed*, const C2dEffect*, const CEntity*));
    // RH_ScopedOverloadedInstall(IsPedRegisteredWithEffect, "+array", 0x5EB690, bool(CPedAttractorManager::*)(CPed*, const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&));
    RH_ScopedInstall(FindAssociatedAttractor, 0x5EB6F0, { .reversed = false });
    RH_ScopedInstall(HasQueueTailArrivedAtSlot, 0x5EBBA0, { .reversed = false });
    RH_ScopedInstall(HasEmptySlot, 0x5EBB00, { .reversed = false });
    // RH_ScopedOverloadedInstall(GetPedUsingEffect, "array", 0x5EB740, void*(CPedAttractorManager::*)(const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&));
    // RH_ScopedOverloadedInstall(GetPedUsingEffect, "", 0x5EBE50, void*(CPedAttractorManager::*)(const C2dEffect*, const CEntity*));
    // RH_ScopedOverloadedInstall(GetRelevantAttractor, "array", 0x5EB7B0, void*(CPedAttractorManager::*)(const CPed*, const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&));
    // RH_ScopedOverloadedInstall(GetRelevantAttractor, "", 0x5EBF50, void*(CPedAttractorManager::*)(const CPed*, const C2dEffect*, const CEntity*));
    RH_ScopedInstall(ComputeEffectPos, 0x5E96C0);
    RH_ScopedInstall(ComputeEffectUseDir, 0x5E96E0, { .reversed = false });
    RH_ScopedInstall(ComputeEffectQueueDir, 0x5E9730, { .reversed = false });
    RH_ScopedInstall(ComputeEffectForwardDir, 0x5E9780, { .reversed = false });
    RH_ScopedInstall(RegisterPed, 0x5EF980, { .reversed = false });
    RH_ScopedInstall(RegisterPedWithAttractor, 0x5EFCA0, { .reversed = false });
    RH_ScopedInstall(IsApproachable, 0x5EA220, { .reversed = false });
}

// 0x? 0x5EF710 ?
void CPedAttractorManager::RestoreStuffFromMem() {
    assert(false);
}

// 0x5EF920
void CPedAttractorManager::BroadcastArrival(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array) {
    plugin::CallMethod<0x5EF920, CPedAttractorManager*, CPed*, CPedAttractor*, SArray<CPedAttractor*>&>(this, ped, attractor, array);
}

// 0x5EFE20
void CPedAttractorManager::BroadcastArrival(CPed* ped, CPedAttractor* attractor) {
    if (!attractor)
        return;

    if (!IsPedRegisteredWithEffect(ped))
        return;

    switch (attractor->GetType()) {
    case PED_ATTRACTOR_ATM:            return BroadcastArrival(ped, attractor, m_Attractors[1]);
    case PED_ATTRACTOR_SEAT:           return BroadcastArrival(ped, attractor, m_Attractors[0]); // ?
    case PED_ATTRACTOR_STOP:           return BroadcastArrival(ped, attractor, m_Attractors[2]);
    case PED_ATTRACTOR_PIZZA:          return BroadcastArrival(ped, attractor, m_Attractors[3]);
    case PED_ATTRACTOR_SHELTER:        return BroadcastArrival(ped, attractor, m_Attractors[4]);
    case PED_ATTRACTOR_TRIGGER_SCRIPT: return BroadcastArrival(ped, attractor, m_Attractors[5]);
    case PED_ATTRACTOR_LOOK_AT:        return BroadcastArrival(ped, attractor, m_Attractors[6]);
    case PED_ATTRACTOR_SCRIPTED:       return BroadcastArrival(ped, attractor, m_Attractors[7]);
    case PED_ATTRACTOR_PARK:           return BroadcastArrival(ped, attractor, m_Attractors[8]);
    case PED_ATTRACTOR_STEP:           return BroadcastArrival(ped, attractor, m_Attractors[9]);
    default:                           return;
    }
}

// 0x5EC660
void CPedAttractorManager::BroadcastDeparture(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array) {
    plugin::CallMethod<0x5EC660, CPedAttractorManager*, CPed*, CPedAttractor*, SArray<CPedAttractor*>&>(this, ped, attractor, array);
}

// 0x5EC980
void CPedAttractorManager::BroadcastDeparture(CPed* ped, CPedAttractor* attractor) {
    if (!attractor)
        return;

    if (!IsPedRegisteredWithEffect(ped))
        return;

    switch (attractor->GetType()) {
    case PED_ATTRACTOR_ATM:            return BroadcastDeparture(ped, attractor, m_Attractors[1]);
    case PED_ATTRACTOR_SEAT:           return BroadcastDeparture(ped, attractor, m_Attractors[0]); // ?
    case PED_ATTRACTOR_STOP:           return BroadcastDeparture(ped, attractor, m_Attractors[2]);
    case PED_ATTRACTOR_PIZZA:          return BroadcastDeparture(ped, attractor, m_Attractors[3]);
    case PED_ATTRACTOR_SHELTER:        return BroadcastDeparture(ped, attractor, m_Attractors[4]);
    case PED_ATTRACTOR_TRIGGER_SCRIPT: return BroadcastDeparture(ped, attractor, m_Attractors[5]);
    case PED_ATTRACTOR_LOOK_AT:        return BroadcastDeparture(ped, attractor, m_Attractors[6]);
    case PED_ATTRACTOR_SCRIPTED:       return BroadcastDeparture(ped, attractor, m_Attractors[7]);
    case PED_ATTRACTOR_PARK:           return BroadcastDeparture(ped, attractor, m_Attractors[8]);
    case PED_ATTRACTOR_STEP:           return BroadcastDeparture(ped, attractor, m_Attractors[9]);
    default:                           return;
    }
}

// 0x5EC740
void CPedAttractorManager::DeRegisterPed(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array) {
    plugin::CallMethod<0x5EC740, CPedAttractorManager*, CPed*, CPedAttractor*, SArray<CPedAttractor*>&>(this, ped, attractor, array);
}

// 0x5EC850
void CPedAttractorManager::DeRegisterPed(CPed* ped, CPedAttractor* attractor) {
    if (!attractor)
        return;

    if (!IsPedRegisteredWithEffect(ped))
        return;

    switch (attractor->GetType()) {
    case PED_ATTRACTOR_ATM:            return DeRegisterPed(ped, attractor, m_Attractors[1]);
    case PED_ATTRACTOR_SEAT:           return DeRegisterPed(ped, attractor, m_Attractors[0]); // ?
    case PED_ATTRACTOR_STOP:           return DeRegisterPed(ped, attractor, m_Attractors[2]);
    case PED_ATTRACTOR_PIZZA:          return DeRegisterPed(ped, attractor, m_Attractors[3]);
    case PED_ATTRACTOR_SHELTER:        return DeRegisterPed(ped, attractor, m_Attractors[4]);
    case PED_ATTRACTOR_TRIGGER_SCRIPT: return DeRegisterPed(ped, attractor, m_Attractors[5]);
    case PED_ATTRACTOR_LOOK_AT:        return DeRegisterPed(ped, attractor, m_Attractors[6]);
    case PED_ATTRACTOR_SCRIPTED:       return DeRegisterPed(ped, attractor, m_Attractors[7]);
    case PED_ATTRACTOR_PARK:           return DeRegisterPed(ped, attractor, m_Attractors[8]);
    case PED_ATTRACTOR_STEP:           return DeRegisterPed(ped, attractor, m_Attractors[9]);
    default:                           return;
    }
}

// 0x5EB5F0
void CPedAttractorManager::RemoveEffect(const C2dEffect* effect, const SArray<CPedAttractor*>& array) {
    plugin::CallMethod<0x5EB5F0, CPedAttractorManager*, const C2dEffect*, const SArray<CPedAttractor*>&>(this, effect, array);
}

// 0x5EBA30
void CPedAttractorManager::RemoveEffect(const C2dEffect* effect) {
    plugin::CallMethod<0x5EBA30, CPedAttractorManager*, const C2dEffect*>(this, effect);
}

// 0x5EB640
bool CPedAttractorManager::IsPedRegistered(CPed* ped, const SArray<CPedAttractor*>& array) {
    return plugin::CallMethodAndReturn<bool, 0x5EB640, CPedAttractorManager*, CPed*, const SArray<CPedAttractor*>&>(this, ped, array);
}

// 0x5EBCB0
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped) {
    return IsPedRegistered(ped, m_Attractors[0])
        || IsPedRegistered(ped, m_Attractors[1])
        || IsPedRegistered(ped, m_Attractors[2])
        || IsPedRegistered(ped, m_Attractors[3])
        || IsPedRegistered(ped, m_Attractors[4])
        || IsPedRegistered(ped, m_Attractors[5])
        || IsPedRegistered(ped, m_Attractors[6])
        || IsPedRegistered(ped, m_Attractors[7])
        || IsPedRegistered(ped, m_Attractors[8])
        || IsPedRegistered(ped, m_Attractors[9]);
}

// 0x5EBD70
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped, const C2dEffect* effect, const CEntity* entity) {
    return IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[0])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[1])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[2])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[3])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[4])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[5])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[6])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[7])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[8])
        || IsPedRegisteredWithEffect(ped, effect, entity, m_Attractors[9]) != 0;
}

// 0x5EB690
bool CPedAttractorManager::IsPedRegisteredWithEffect(CPed* ped, const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array) {
    return plugin::CallMethodAndReturn<bool, 0x5EB690, CPedAttractorManager*, CPed*, const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&>(this, ped, effect, entity, array);
}

// 0x5EB6F0
void CPedAttractorManager::FindAssociatedAttractor(const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array) {
    assert(false);
}

// 0x5EBBA0
bool CPedAttractorManager::HasQueueTailArrivedAtSlot(const C2dEffect* effect, const CEntity* entity) {
    return plugin::CallMethodAndReturn<bool, 0x5EBBA0, CPedAttractorManager*, const C2dEffect*, const CEntity*>(this, effect, entity);
}

// 0x5EBB00
bool CPedAttractorManager::HasEmptySlot(const C2dEffect* effect, const CEntity* entity) {
    return plugin::CallMethodAndReturn<bool, 0x5EBB00, CPedAttractorManager*, const C2dEffect*, const CEntity*>(this, effect, entity);
}

// 0x5EB740
void* CPedAttractorManager::GetPedUsingEffect(const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array) {
    return plugin::CallMethodAndReturn<void*, 0x5EB740, CPedAttractorManager*, const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&>(this, effect, entity, array);
}

// 0x5EBE50
CPed* CPedAttractorManager::GetPedUsingEffect(const C2dEffect* effect, const CEntity* entity) {
    return plugin::CallMethodAndReturn<CPed*, 0x5EBE50, CPedAttractorManager*, const C2dEffect*, const CEntity*>(this, effect, entity);
}

// 0x5EB7B0
void* CPedAttractorManager::GetRelevantAttractor(const CPed* ped, const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array) {
    return plugin::CallMethodAndReturn<void*, 0x5EB7B0, CPedAttractorManager*, const CPed*, const C2dEffect*, const CEntity*, const SArray<CPedAttractor*>&>(this, ped, effect, entity, array);
}

// 0x5EBF50
void* CPedAttractorManager::GetRelevantAttractor(const CPed* ped, const C2dEffectBase* effect, const CEntity* entity) {
    return plugin::CallMethodAndReturn<void*, 0x5EBF50, CPedAttractorManager*, const CPed*, const C2dEffectBase*, const CEntity*>(this, ped, effect, entity);
}

// 0x5E96C0
void CPedAttractorManager::ComputeEffectPos(const C2dEffect* effect, const CMatrix& mat, CVector& vec) {
    vec.FromMultiply(mat, effect->m_pos);
}

// 0x5E96E0
void CPedAttractorManager::ComputeEffectUseDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec) {
    assert(false);
}

// 0x5E9730
void CPedAttractorManager::ComputeEffectQueueDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec) {
    assert(false);
}

// 0x5E9780
void CPedAttractorManager::ComputeEffectForwardDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec) {
    assert(false);
}

// 0x5EF980
void CPedAttractorManager::RegisterPed(CPed* ped, C2dEffect*, CEntity*, int32, SArray<CPedAttractor*>& array) {
    assert(false);
}

// 0x5EFCA0
CPedAttractor* CPedAttractorManager::RegisterPedWithAttractor(CPed* ped, C2dEffectBase* fx, CEntity* entity, eMoveState ms) {
    return plugin::CallMethodAndReturn<CPedAttractor*, 0x5EFCA0>(this, ped, fx, entity, ms);
}

// 0x5EA220
bool CPedAttractorManager::IsApproachable(C2dEffect* effect, const CMatrix& mat, int32 unused, CPed* ped) {
    return plugin::CallAndReturn<bool, 0x5EA220, C2dEffect*, const CMatrix&, int32, CPed*>(effect, mat, unused, ped);
}
