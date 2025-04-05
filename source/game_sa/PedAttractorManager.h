#pragma once

#include <Base.h>
#include "SArray.h"
#include <PedSeatAttractor.h>
#include <PedATMAttractor.h>
#include <PedStopAttractor.h>
#include <PedPizzaAttractor.h>
#include <PedShelterAttractor.h>
#include <PedTriggerScriptAttractor.h>
#include <PedLookAtAttractor.h>
#include <PedScriptedAttractor.h>
#include <PedParkAttractor.h>
#include <PedStepAttractor.h>

#include <extensions/Casting.hpp>

class CPedAttractor;
class C2dEffect;
class CEntity;
class CPed;

class CPedAttractorManager {
    SArray<CPedSeatAttractor*>          m_Seats;
    SArray<CPedATMAttractor*>           m_ATMs;
    SArray<CPedStopAttractor*>          m_Stops;
    SArray<CPedPizzaAttractor*>         m_Pizzas;
    SArray<CPedShelterAttractor*>       m_Shelters;
    SArray<CPedTriggerScriptAttractor*> m_TriggerScripts;
    SArray<CPedLookAtAttractor*>        m_LookAts;
    SArray<CPedScriptedAttractor*>      m_Scripted;
    SArray<CPedParkAttractor*>          m_Parks;
    SArray<CPedStepAttractor*>          m_Steps;

public:
    static void InjectHooks();

    CPedAttractorManager() = default; // 0x5EDF10

    void RestoreStuffFromMem();

    // 0x5EF920
    template<std::derived_from<CPedAttractor> T>
    bool BroadcastArrival(CPed* ped, CPedAttractor* attractor, SArray<T*>& attractors) {
        T* const a = notsa::cast_if_present<T>(attractor);
        return a
            && rng::contains(attractors, a)
            && a->BroadcastArrival(ped);
    }
    bool BroadcastArrival(CPed* ped, CPedAttractor* attractor);

    // 0x5EC660
    template<std::derived_from<CPedAttractor> T>
    bool BroadcastDeparture(CPed* ped, CPedAttractor* attractor, SArray<T*>& attractors) {
        T* const a = notsa::cast_if_present<T>(attractor);
        if (!a) {
            return false;
        }
        const auto it = rng::find(attractors, a);
        if (it == attractors.end()) {
            return false;
        }
        a->BroadcastDeparture(ped);
        if (!a->GetNoOfRegisteredPeds()) {
            attractors.erase(it);
        }
        return true;
    }
    bool BroadcastDeparture(CPed* ped, CPedAttractor* attractor);

    // 0x5EC740
    template<std::derived_from<CPedAttractor> T>
    bool DeRegisterPed(CPed* ped, CPedAttractor* attractor, SArray<T*>& attractors) {
        T* const a = notsa::cast_if_present<T>(attractor);
        if (!a) {
            return false;
        }
        const auto it = rng::find(attractors, a);
        if (it == attractors.end()) {
            return false;
        }
        a->DeRegisterPed(ped);
        if (!a->GetNoOfRegisteredPeds()) {
            attractors.erase(it);
        }
        return true;
    }
    bool DeRegisterPed(CPed* ped, CPedAttractor* attractor);

    // 0x5EB5F0
    template<std::derived_from<CPedAttractor> T>
    bool RemoveEffect(const C2dEffectPedAttractor* fx, const SArray<T*>& attractors) {
        for (auto* const a : attractors) {
            if (a->GetEffect() == fx) {
                a->AbortPedTasks();
            }
        }
        return false;
    }
    bool RemoveEffect(const C2dEffectPedAttractor* fx);

    // 0x5EB640
    template<std::derived_from<CPedAttractor> T>
    bool IsPedRegistered(CPed* ped, const SArray<T*>& attractors) {
        return rng::any_of(attractors, [ped](T* a) {
            return a->IsRegisteredWithPed(ped);
        });
    }

    // 0x5EB690
    template<std::derived_from<CPedAttractor> T>
    bool IsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray<T*>& attractors) {
        return GetRelevantAttractor(ped, fx, entity, attractors) != nullptr; // NOTSA: Using `GetRelevantAttractor` instead of duplicate code
    }
    bool IsPedRegisteredWithEffect(CPed* ped);
    bool IsPedRegisteredWithEffect(CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity);

    // 0x5EB6F0
    template<std::derived_from<CPedAttractor> T>
    T* FindAssociatedAttractor(const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray<T*>& attractors) {
        for (auto* const a : attractors) {
            if (a->GetEffect() == fx && a->GetEntity() == entity) {
                return a;
            }
        }
        return nullptr;
    }
    CPedAttractor* FindAssociatedAttractor(const C2dEffectPedAttractor* fx, const CEntity* entity);

    bool HasQueueTailArrivedAtSlot(const C2dEffectPedAttractor* fx, const CEntity* entity);
    bool HasEmptySlot(const C2dEffectPedAttractor* fx, const CEntity* entity);

    // 0x5EB740
    template<std::derived_from<CPedAttractor> T>
    CPed* GetPedUsingEffect(const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray<T*>& attractors) {
        for (const T* const a : attractors) {
            if (a->GetEffect() == fx && a->GetEntity() == entity) {
                return a->GetHeadOfQueue();
            }
        }
        return nullptr;
    }
    CPed* GetPedUsingEffect(const C2dEffectPedAttractor* fx, const CEntity* entity = nullptr);

    // 0x5EB7B0
    template<std::derived_from<CPedAttractor> T>
    const T* GetRelevantAttractor(const CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity, const SArray<T*>& attractors) {
        for (const T* const a : attractors) {
            if (a->GetEffect() == fx && a->GetEntity() == entity && a->IsRegisteredWithPed(ped)) {
                return a;
            }
        }
        return nullptr;
    }
    const CPedAttractor* GetRelevantAttractor(const CPed* ped, const C2dEffectPedAttractor* fx, const CEntity* entity);

    // 0x5EF980
    template<std::derived_from<CPedAttractor> T>
    T* RegisterPed(CPed* ped, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState, SArray<T*>& attractors) {
        T* a = FindAssociatedAttractor(fx, entity, attractors);
        if (!a) {
            a = new T{fx, entity, moveState};
            attractors.emplace_back(a);
        }
        a->RegisterPed(ped);
        return a;
        
    }
    CPedAttractor* RegisterPedWithAttractor(CPed* ped, C2dEffectPedAttractor* fx, CEntity* entity, eMoveState ms);


    // 0x5E96C0
    static CVector ComputeEffectPos(const C2dEffectPedAttractor* fx, const CMatrix& mat) {
        return mat.TransformPoint(fx->m_Pos);
    }

    // 0x5E96E0
    static CVector ComputeEffectUseDir(const C2dEffectPedAttractor* fx, const CMatrix& mat) {
        return mat.TransformVector(fx->m_vecUseDir);
    }

    // 0x5E9730
    static CVector ComputeEffectQueueDir(const C2dEffectPedAttractor* fx, const CMatrix& mat) {
        return mat.TransformVector(fx->m_vecQueueDir);
    }

    // 0x5E9780
    static CVector ComputeEffectForwardDir(const C2dEffectPedAttractor* fx, const CMatrix& mat) {
        return mat.TransformVector(fx->m_vecForwardDir);
    }

    static bool IsApproachable(C2dEffectPedAttractor* fx, const CMatrix& mat, int32 unused, CPed* ped);

private:
    // notsa
    template<typename Callable>
    decltype(auto) VisitAttractorsOfType(ePedAttractorType t, Callable&& visitor) {
        switch (t) {
        case PED_ATTRACTOR_ATM:            return std::invoke(visitor, m_ATMs);
        case PED_ATTRACTOR_SEAT:           return std::invoke(visitor, m_Seats);
        case PED_ATTRACTOR_STOP:           return std::invoke(visitor, m_Stops);
        case PED_ATTRACTOR_PIZZA:          return std::invoke(visitor, m_Pizzas);
        case PED_ATTRACTOR_SHELTER:        return std::invoke(visitor, m_Shelters);
        case PED_ATTRACTOR_TRIGGER_SCRIPT: return std::invoke(visitor, m_TriggerScripts);
        case PED_ATTRACTOR_LOOK_AT:        return std::invoke(visitor, m_LookAts);
        case PED_ATTRACTOR_SCRIPTED:       return std::invoke(visitor, m_Scripted);
        case PED_ATTRACTOR_PARK:           return std::invoke(visitor, m_Parks);
        case PED_ATTRACTOR_STEP:           return std::invoke(visitor, m_Steps);
        default:                           NOTSA_UNREACHABLE();
        }    
    }
};
VALIDATE_SIZE(CPedAttractorManager, 0xA0);

CPedAttractorManager* GetPedAttractorManager();
