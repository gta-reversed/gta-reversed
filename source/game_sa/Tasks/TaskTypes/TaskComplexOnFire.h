#pragma once

#include "TaskComplex.h"

class CPedDamageResponse;
class CTask;
class CPed;

/*!
* Task sequence: COMPLEX_SMART_FLEE_ENTITY (until dead) -> COMPLEX_DIE
*/
class NOTSA_EXPORT_VTABLE CTaskComplexOnFire : public CTaskComplex {
public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_ON_FIRE;

    CTaskComplexOnFire() = default;
    CTaskComplexOnFire(const CTaskComplexOnFire&) : CTaskComplexOnFire{} {} // NOTSA
    ~CTaskComplexOnFire() = default;

    static void ComputeFireDamage(CPed* ped, CPedDamageResponse& resp);

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexOnFire{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed * ped) override;
    CTask*    CreateFirstSubTask(CPed * ped) override;
    CTask*    ControlSubTask(CPed * ped) override;
 
private: // Wrappers for hooks
    // 0x633390
    CTaskComplexOnFire* Constructor() {
        this->CTaskComplexOnFire::CTaskComplexOnFire();
        return this;
    }

    // 0x6333C0
    CTaskComplexOnFire* Destructor() {
        this->CTaskComplexOnFire::~CTaskComplexOnFire();
        return this;
    }
};
