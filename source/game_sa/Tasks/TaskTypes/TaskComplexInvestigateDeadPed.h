#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"
#include "Vector.h"

class CPed;
class CTaskComplexInvestigateDeadPed;
class CTask;

class NOTSA_EXPORT_VTABLE CTaskComplexInvestigateDeadPed : public CTaskComplex {

public:
    CPed* m_deadPed = {};       ///< The ped that we have to investigate
    CTaskTimer m_timer = {};    ///< Unused timer, as it's never started
    CVector m_targetPos = {};   ///< The position we shuld go to, it is updated in `ControlSubTask`
    float m_targetHeading = {}; ///< The heading that should be achieved, used for `CTaskSimpleAchieveHeading`
    CVector m_pedInitalPos = {};///< The ped's inital position (Set in `CreateFirstSubTask`)
    int32 m_lookDurMs = {};     ///< The IK LookAt's duration used when the `CTaskSimpleAchieveHeading` task is ran

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_COMPLEX_INVESTIGATE_DEAD_PED;

    CTaskComplexInvestigateDeadPed(CPed* deadPed);
    CTaskComplexInvestigateDeadPed(const CTaskComplexInvestigateDeadPed&);
    ~CTaskComplexInvestigateDeadPed();

    void ComputeTargetPos(CPed* ped, CVector& targetPos);
    CTask* CreateSubTask(eTaskType taskType, CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexInvestigateDeadPed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed * ped) override;
    CTask*    CreateFirstSubTask(CPed * ped) override;
    CTask*    ControlSubTask(CPed * ped) override;
private: // Wrappers for hooks
    // 0x696DF0
    CTaskComplexInvestigateDeadPed* Constructor(CPed* deadPed) {
        this->CTaskComplexInvestigateDeadPed::CTaskComplexInvestigateDeadPed(deadPed);
        return this;
    }
    // 0x696E80
    CTaskComplexInvestigateDeadPed* Destructor() {
        this->CTaskComplexInvestigateDeadPed::~CTaskComplexInvestigateDeadPed();
        return this;
    }
};
