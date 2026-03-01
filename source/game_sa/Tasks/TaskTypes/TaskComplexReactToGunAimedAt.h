#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"

class CTaskComplexReactToGunAimedAt;
class CTask;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexReactToGunAimedAt : public CTaskComplex {
public:
    CPed* m_aimedBy{};               /// The ped that is aiming the gun at us
    CTaskTimer m_wanderBeginTimer{}; /// Started once the `ComplexWander` subtask has been created, it's interval is 10'000

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_REACT_TO_GUN_AIMED_AT;

    CTaskComplexReactToGunAimedAt(CPed* aimedBy);
    CTaskComplexReactToGunAimedAt(const CTaskComplexReactToGunAimedAt&);
    ~CTaskComplexReactToGunAimedAt();

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexReactToGunAimedAt{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6971B0
    auto Constructor(CPed* aimedBy) {
        this->CTaskComplexReactToGunAimedAt::CTaskComplexReactToGunAimedAt(aimedBy);
        return this;
    }
    // 0x697230
    auto Destructor() {
        this->CTaskComplexReactToGunAimedAt::~CTaskComplexReactToGunAimedAt();
        return this;
    }
};
