#pragma once

#include "TaskComplexSmartFleeEntity.h"

class CVehicle;
class CEntity;
class CPed;
class CTaskComplexFleeAnyMeans;
class CTaskComplexCarDriveMission;

class NOTSA_EXPORT_VTABLE CTaskComplexFleeAnyMeans : public CTaskComplexSmartFleeEntity {

public:
    bool      m_shootWhileFleeing{};               /// Should the ped be shooting (with a weapon) while fleeing
    int32     m_shootTime{};                       /// @copydoc CTaskComplexFleeShooting::m_shootTime
    int32     m_shootRecoverTime{};                /// @copydoc CTaskComplexFleeShooting::m_shootRecoverTime
    float     m_minDistFromFleeEntityToStealCar{}; /// Minimum distance from the entity we're fleeing from to steal a car (eg.: if we're closer than this to the entity stealing a car won't be attempted)
    CVehicle* m_stealVeh{};                        /// The vehicle to steal

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_FLEE_ANY_MEANS;

    CTaskComplexFleeAnyMeans(
        CEntity* entity,
        bool attackWhileFeeling,
        float safeDist,
        int32 fleeTime = 1'000'000,
        int32 shootTime = 4'000,
        int32 shootRecoverTime = 4'000,
        float stealCarDist = 20.f,
        int32 posChangePeriod = 1'000,
        float posChangeTolerance = 1.f
    );
    CTaskComplexFleeAnyMeans(const CTaskComplexFleeAnyMeans&);
    ~CTaskComplexFleeAnyMeans();

    CTask* CreateSubTask(eTaskType taskId, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexFleeAnyMeans{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x65CC60
    CTaskComplexFleeAnyMeans* Constructor(CEntity* entity, bool attackWhileFeeling, float safeDist, int32 fleeTime, int32 shootTime, int32 shootRecoverTime, float stealCarDist,
                                          int32 posChangePeriod, float posChangeTolerance) {
        this->CTaskComplexFleeAnyMeans::CTaskComplexFleeAnyMeans(entity, attackWhileFeeling, safeDist, fleeTime, shootTime, shootRecoverTime, stealCarDist, posChangePeriod,
                                                                 posChangeTolerance);
        return this;
    }
    // 0x65CD10
    CTaskComplexFleeAnyMeans* Destructor() {
        this->CTaskComplexFleeAnyMeans::~CTaskComplexFleeAnyMeans();
        return this;
    }
};
