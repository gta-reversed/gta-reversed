#pragma once

#include "TaskComplex.h"

class NOTSA_EXPORT_VTABLE CTaskComplexObserveTrafficLightsAndAchieveHeading : public CTaskComplex {
public:
    int32 m_nDurationInMs;
    float m_fTargetHeading;

public:
    static constexpr auto Type = TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING;

    CTaskComplexObserveTrafficLightsAndAchieveHeading(int32 durationInMs, float targetHeading);
    ~CTaskComplexObserveTrafficLightsAndAchieveHeading() override = default; // 0x631980

    eTaskType GetTaskType() const override { return Type; } // 0x631940
    CTask* Clone() const override;
    CTask* ControlSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;

    CTask* CreateSubTask(eTaskType taskType) const;

private:
    friend void InjectHooksMain();
    static void InjectHooks() {
        RH_ScopedVirtualClass(CTaskComplexObserveTrafficLightsAndAchieveHeading, 0x86DFC4, 11);
        RH_ScopedCategory("Tasks/TaskTypes");

        RH_ScopedVMTInstall(Clone, 0x636490);
        RH_ScopedVMTInstall(ControlSubTask, 0x631AD0);
        RH_ScopedVMTInstall(CreateFirstSubTask, 0x631AC0);
        RH_ScopedVMTInstall(CreateNextSubTask, 0x631A70);
        RH_ScopedVMTInstall(MakeAbortable, 0x631950);
    }

    CTaskComplexObserveTrafficLightsAndAchieveHeading* Constructor(int32 durationInMs, float fTargetHeading) {
        this->CTaskComplexObserveTrafficLightsAndAchieveHeading::CTaskComplexObserveTrafficLightsAndAchieveHeading(durationInMs, fTargetHeading);
        return this;
    }

};

VALIDATE_SIZE(CTaskComplexObserveTrafficLightsAndAchieveHeading, 0x14);
