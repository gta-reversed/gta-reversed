#pragma once

#include "Plugins\TwoDEffectPlugin\2dEffect.h"
#include "PedTaskPair.h"
#include "SArray.h"
#include <Enums/eMoveState.h>

class CEntity;
class CPed;
class CTask;

class NOTSA_EXPORT_VTABLE CPedAttractor {
public:
    C2dEffectPedAttractor* m_Fx{};
    notsa::EntityRef<>     m_Entity{};
    SArray<CPed*>          m_AttractPeds{};
    SArray<CPed*>          m_ArrivedPeds{}; // AKA queue
    SArray<CPedTaskPair>   m_PedTaskPairs{};
    size_t                 m_MaxNumPeds{}; //!< Maximum number of registered peds (ArrivedPeds + AttractPeds, see `GetNoOfRegisteredPeds()`)
    float                  m_Spacing{};
    float                  m_AchieveQueueTime{};
    float                  m_AchieveQueueShuffleTime{};
    float                  m_ArriveRange{};
    float                  m_HeadingRange{};
    float                  m_DeltaPos{};
    float                  m_DeltaHeading{};
    CVector                m_Pos{};
    CVector                m_QueueDir{};
    CVector                m_UseDir{};
    eMoveState             m_MoveState{};
    char                   m_ScriptName[8]{};

    inline static SArray<CTask>& ms_tasks = *reinterpret_cast<SArray<CTask>*>(0xC0985C);

public:
    static void InjectHooks();

    void* operator new(unsigned size);
    void  operator delete(void* object);

    CPedAttractor(
        C2dEffectPedAttractor* fx,
        CEntity*               entity,
        eMoveState             moveState,
        size_t                 maxNoOfPeds,
        float                  spacing,
        float                  achieveQueueTime,
        float                  achieveQueueShuffleTime,
        float                  arriveRange,
        float                  headingRange,
        float                  deltaPos,
        float                  deltaHeading
    );

    virtual ~CPedAttractor() {}

    static void Shutdown();

    virtual ePedAttractorType GetType() const { return PED_ATTRACTOR_UNDEFINED; }

    void SetTaskForPed(CPed* ped, CTask* task);
    bool RegisterPed(CPed* ped);
    bool DeRegisterPed(CPed* ped);

    bool IsRegisteredWithPed(const CPed* ped) const;

    virtual bool IsAtHeadOfQueue(CPed* ped);

    CTask*   GetTaskForPed(CPed* ped);
    size_t   GetNoOfRegisteredPeds() const { return m_AttractPeds.size() + m_ArrivedPeds.size(); }
    int32    GetQueueSlot(const CPed*);
    CPed*    GetHeadOfQueue() const;
    CPed*    GetTailOfQueue() const;
    auto&    GetArrivedPeds() { return m_ArrivedPeds; }
    size_t   GetSizeOfQueue() const { return m_ArrivedPeds.size(); }
    auto&    GetAttractPeds() { return m_AttractPeds; }
    auto     GetEffect() const { return m_Fx; }
    CEntity* GetEntity() const { return m_Entity; }

    bool         HasEmptySlot() const { return GetNoOfRegisteredPeds() < m_MaxNumPeds; }
    int32        ComputeFreeSlot();

    float        ComputeDeltaPos() const;
    float        ComputeDeltaHeading() const;

    void         ComputeAttractTime(int32 slotIdx, bool hasArrived, float& outTime) const;
    virtual void ComputeAttractPos(int32 pedId, CVector& outPos);
    virtual void ComputeAttractHeading(int32 bQueue, float& heading);

    virtual void BroadcastDeparture(CPed* ped);
    bool         BroadcastArrival(CPed* ped);

    void AbortPedTasks();

public:
    // Casting.hpp support //
    template<typename From, typename Self>
    static constexpr bool classof(const From* f) { return f->GetType() == Self::Type; }

private:
    auto* Constructor(C2dEffectPedAttractor* fx, CEntity* entity, eMoveState moveState, size_t maxNoOfPeds, float spacing, float achieveQueueTime, float achieveQueueShuffleTime, float arriveRange, float headingRange, float deltaPos, float deltaHeading) {
        return std::construct_at(this,fx, entity, moveState, maxNoOfPeds, spacing, achieveQueueTime, achieveQueueShuffleTime, arriveRange, headingRange, deltaPos, deltaHeading);
    }
    auto* Destructor() {
        std::destroy_at(this);
        return this;
    }
};
VALIDATE_SIZE(CPedAttractor, 0x8C);
