#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"
#include "Ped.h"

class CTaskComplexWaitForPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForPed : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_WAIT_FOR_PED;

    static void InjectHooks();

    CTaskComplexWaitForPed(CPed* waitForPed, float targetDist, int32 timeout, bool lookAtPed);
    CTaskComplexWaitForPed(const CTaskComplexWaitForPed&);
    ~CTaskComplexWaitForPed() override = default;

    CTask*    Clone() const override { return new CTaskComplexWaitForPed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

public:
    CPed::Ref  m_WaitForPed{};
    float      m_TargetDist{};
    int32      m_Timeout{};
    bool       m_LookAtPed{};
    CTaskTimer m_Timer{};
    int32      m_FrameCounter{};
};
