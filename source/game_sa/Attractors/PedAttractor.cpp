#include "StdInc.h"

#include "PedAttractor.h"

void CPedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedAttractor, 0x86C538, 6);
    RH_ScopedCategory("Attractors");

    RH_ScopedInstall(Constructor, 0x5EDFB0);
    RH_ScopedInstall(Destructor, 0x5EC410);

    RH_ScopedInstall(SetTaskForPed, 0x5EECA0, { .reversed = false });
    RH_ScopedInstall(RegisterPed, 0x5EEE30, { .reversed = false });
    RH_ScopedInstall(DeRegisterPed, 0x5EC5B0, { .reversed = false });
    RH_ScopedInstall(IsRegisteredWithPed, 0x5EB4C0, { .reversed = false });
    RH_ScopedInstall(IsAtHeadOfQueue, 0x5EB530, { .reversed = false });
    RH_ScopedInstall(GetTaskForPed, 0x5EC500, { .reversed = false });
    RH_ScopedInstall(GetQueueSlot, 0x5EB550, { .reversed = false });
    //RH_ScopedInstall(GetNoOfRegisteredPeds, 0xdeadbeef, { .reversed = false }); // Address incorrect
    RH_ScopedInstall(GetHeadOfQueue, 0x5EB590);
    RH_ScopedInstall(GetTailOfQueue, 0x5EB5B0);
    RH_ScopedInstall(HasEmptySlot, 0x5EAF10);
    //RH_ScopedInstall(ComputeFreeSlot, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeDeltaPos, 0x5E9600);
    RH_ScopedInstall(ComputeDeltaHeading, 0x5E9640);
    RH_ScopedInstall(ComputeAttractTime, 0x5E95E0);
    RH_ScopedInstall(ComputeAttractPos, 0x5EA110);
    RH_ScopedInstall(ComputeAttractHeading, 0x5EA1C0);
    RH_ScopedInstall(BroadcastDeparture, 0x5EF160, { .reversed = false });
    RH_ScopedInstall(BroadcastArrival, 0x5EEF80, { .reversed = false });
    RH_ScopedInstall(AbortPedTasks, 0x5EAF60, { .reversed = false });
}

// 0x5EAFD0
void* CPedAttractor::operator new(unsigned size) {
    return CPools::GetPedAttractorPool()->New();
}

// 0x5EAFE0
void CPedAttractor::operator delete(void* object) {
    CPools::GetPedAttractorPool()->Delete(static_cast<CPedAttractor*>(object));
}

// 0x5EDFB0
CPedAttractor::CPedAttractor(
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
) :
    m_Fx{fx},
    m_Entity{entity},
    m_MoveState{moveState},
    m_MaxNumPeds{maxNoOfPeds},
    m_Spacing{spacing},
    m_AchieveQueueTime{achieveQueueTime},
    m_AchieveQueueShuffleTime{achieveQueueShuffleTime},
    m_ArriveRange{arriveRange},
    m_HeadingRange{headingRange},
    m_DeltaPos{deltaPos},
    m_DeltaHeading{deltaHeading}
{
    const CMatrix mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();
    m_Pos             = CPedAttractorManager::ComputeEffectPos(fx, mat);
    m_QueueDir        = CPedAttractorManager::ComputeEffectQueueDir(fx, mat);
    m_UseDir          = CPedAttractorManager::ComputeEffectUseDir(fx, mat);

    strcpy_s(m_ScriptName, fx->m_szScriptName);
}

void CPedAttractor::Shutdown() {
    ms_tasks.clear();
}

// 0x5EECA0
void CPedAttractor::SetTaskForPed(CPed* ped, CTask* task) {
    return plugin::CallMethod<0x5EECA0, CPedAttractor*, CPed*, CTask*>(this, ped, task);
}

// 0x5EEE30
bool CPedAttractor::RegisterPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x5EEE30, CPedAttractor*, CPed*>(this, ped);
}

// 0x5EC5B0
bool CPedAttractor::DeRegisterPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x5EC5B0, CPedAttractor*, CPed*>(this, ped);
}

// 0x5EB4C0
bool CPedAttractor::IsRegisteredWithPed(const CPed* ped) const {
    return plugin::CallMethodAndReturn<bool, 0x5EB4C0, const CPedAttractor*, const CPed*>(this, ped);
}

// 0x5EB530
bool CPedAttractor::IsAtHeadOfQueue(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x5EB530, CPedAttractor*, CPed*>(this, ped);
}

// 0x5EC500
CTask* CPedAttractor::GetTaskForPed(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x5EC500, CPedAttractor*>(this);
}

// 0x5EB550
int32 CPedAttractor::GetQueueSlot(const CPed*) {
    return plugin::CallMethodAndReturn<int32, 0x5EB550, CPedAttractor*>(this);
}

// 0x5EB590
CPed* CPedAttractor::GetHeadOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.front();
}

// 0x5EB5B0
CPed* CPedAttractor::GetTailOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.back();
}

int32 CPedAttractor::ComputeFreeSlot() {
    assert(0);
    return 0;
}

// 0x5E9600
float CPedAttractor::ComputeDeltaPos() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaPos, m_DeltaPos);
}

// 0x5E9640
float CPedAttractor::ComputeDeltaHeading() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaHeading, m_DeltaHeading);
}

// inlined
// 0x5E95E0
void CPedAttractor::ComputeAttractTime(int32 unused, bool time1_or_time2, float& outTime) const {
    if (time1_or_time2)
        outTime = m_AchieveQueueShuffleTime;
    else
        outTime = m_AchieveQueueTime;
}

// 0x5EA110
void CPedAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        outPos = m_Pos - m_Spacing * (float)pedId * m_QueueDir;

        if (pedId) {
            outPos.x += ComputeDeltaPos();
            outPos.y += ComputeDeltaPos();
        }
    }
}

// bQueue - bad name?
// 0x5EA1C0
void CPedAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    if (m_Fx) {
        if (bQueue)
            m_UseDir = m_QueueDir;

        heading = CGeneral::GetRadianAngleBetweenPoints(m_UseDir.x, m_UseDir.y, 0.0f, 0.0f);
        if (bQueue)
            heading += CPedAttractor::ComputeDeltaHeading();
    }
}

// 0x5EF160
void CPedAttractor::BroadcastDeparture(CPed* ped) {
    plugin::CallMethod<0x5EF160, CPedAttractor*, CPed*>(this, ped);
}

// 0x5EEF80
bool CPedAttractor::BroadcastArrival(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x5EEF80, CPedAttractor*, CPed*>(this, ped);
}

// 0x5EAF60
void CPedAttractor::AbortPedTasks() {
    plugin::CallMethod<0x5EAF60, CPedAttractor*>(this);
}
