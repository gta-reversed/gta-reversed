#pragma once

#include "SArray.h"

class CPedAttractor;
class C2dEffect;
class CEntity;
class CPed;

class CPedAttractorManager {
    SArray<CPedAttractor*> m_Attractors[13];

public:
    static void InjectHooks();

    CPedAttractorManager() = default; // 0x5EDF10

    void RestoreStuffFromMem();

    void BroadcastArrival(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array);
    void BroadcastArrival(CPed* ped, CPedAttractor* attractor);

    void BroadcastDeparture(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array);
    void BroadcastDeparture(CPed* ped, CPedAttractor* attractor);

    void DeRegisterPed(CPed* ped, CPedAttractor* attractor, SArray<CPedAttractor*>& array);
    void DeRegisterPed(CPed* ped, CPedAttractor* attractor);

    void RemoveEffect(const C2dEffect* effect, const SArray<CPedAttractor*>& array);
    void RemoveEffect(const C2dEffect* effect);

    bool IsPedRegistered(CPed* ped, const SArray<CPedAttractor*>& array);
    bool IsPedRegisteredWithEffect(CPed* ped);
    bool IsPedRegisteredWithEffect(CPed* ped, const C2dEffect* effect, const CEntity* entity);
    bool IsPedRegisteredWithEffect(CPed* ped, const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array);

    void FindAssociatedAttractor(const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array);
    bool HasQueueTailArrivedAtSlot(const C2dEffect* effect, const CEntity* entity);
    bool HasEmptySlot(const C2dEffect* effect, const CEntity* entity);

    void* GetPedUsingEffect(const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array);
    CPed* GetPedUsingEffect(const C2dEffect* effect, const CEntity* entity = nullptr);

    void* GetRelevantAttractor(const CPed* ped, const C2dEffect* effect, const CEntity* entity, const SArray<CPedAttractor*>& array);
    void* GetRelevantAttractor(const CPed* ped, const C2dEffectBase* effect, const CEntity* entity);

    static void ComputeEffectPos(const C2dEffect* effect, const CMatrix& mat, CVector& vec);
    static void ComputeEffectUseDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec);
    static void ComputeEffectQueueDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec);
    static void ComputeEffectForwardDir(const C2dEffect* effect, const CMatrix& mat, CVector& vec);

    void RegisterPed(CPed* ped, C2dEffect*, CEntity*, int32, SArray<CPedAttractor*>& array);
    CPedAttractor* RegisterPedWithAttractor(CPed* ped, C2dEffectBase* fx, CEntity* entity, eMoveState ms);

    static bool IsApproachable(C2dEffect* effect, const CMatrix& mat, int32 unused, CPed* ped);
};

VALIDATE_SIZE(CPedAttractorManager, 0xD0);

CPedAttractorManager* GetPedAttractorManager();
