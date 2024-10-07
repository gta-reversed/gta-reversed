#pragma once

#include "Plugins\TwoDEffectPlugin\2dEffect.h"
#include "SArray.h"

class CEntity;
class CPed;
class CTask;

class NOTSA_EXPORT_VTABLE CPedAttractor {
public:
    C2dEffect*    m_pEffect;
    CEntity*      m_pEntity;
    SArray<CPed>  field_C;
    SArray<CPed>  m_pedQueue;
    SArray<CTask> m_pedTasks;
    int32         field_3C;
    float         queueMp;
    float         time1;
    float         time2;
    char          field_4C[8];
    float         m_fRange;
    float         m_fDeltaHeading;
    CVector       m_vecAttractorPosn;
    CVector       m_vecQueueDir;
    CVector       m_vecUseDir;
    int32         field_80;
    char          m_szScriptName[8];

    inline static SArray<CTask>& ms_tasks = *reinterpret_cast<SArray<CTask>*>(0xC0985C);

public:
    static void InjectHooks();

    void* operator new(unsigned size);
    void  operator delete(void* object);

    CPedAttractor(C2dEffect* effect, CEntity* entity, int32 a3, int32 a4, float a5, float time2, float time1, float a8, float a9, float range, float a11, float a12);

    virtual ePedAttractorType GetType() const { return PED_ATTRACTOR_UNDEFINED; }

    // 0x5EC410, 0x5EC4B0
    virtual ~CPedAttractor();

    void SetTaskForPed(CPed* ped, CTask* task);
    bool RegisterPed(CPed* ped);
    void DeRegisterPed(CPed* ped);

    bool IsRegisteredWithPed(const CPed* ped);

    virtual bool IsAtHeadOfQueue(CPed* ped);

    CTask* GetTaskForPed(CPed* ped);
    uint32 GetTailOfQueue();
    int32 GetQueueSlot(const CPed*);
    bool GetNoOfRegisteredPeds();
    void* GetHeadOfQueue();

    // 0x
    int32 ComputeFreeSlot();
    float ComputeDeltaPos() const;
    float ComputeDeltaHeading() const;

    void ComputeAttractTime(int32 unused, bool time1_or_time2, float& outTime) const;

    virtual void ComputeAttractPos(int32 pedId, CVector& outPos);
    virtual void ComputeAttractHeading(int32 bQueue, float& heading);
    virtual void BroadcastDeparture(CPed* ped);

    void BroadcastArrival(CPed* ped);
    void AbortPedTasks();
};

VALIDATE_SIZE(CPedAttractor, 0x8C);
