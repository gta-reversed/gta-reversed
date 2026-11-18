#pragma once

#include "TaskComplex.h"
#include "TaskTimer.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexUseMobilePhone : public CTaskComplex {
public:
    constexpr static auto Type = TASK_COMPLEX_USE_MOBILE_PHONE;

    CTaskComplexUseMobilePhone(int32 duration);
    CTaskComplexUseMobilePhone(const CTaskComplexUseMobilePhone&);
    ~CTaskComplexUseMobilePhone() override = default;

    static void RemovePhoneModel(CPed* ped);

    int8 Stop(CPed*);

    CTask*    Clone() const override { return new CTaskComplexUseMobilePhone{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

public:
    static void InjectHooks();

public:
    int32      m_Duration{};
    CTaskTimer m_Timer{};
    bool       m_IsAborting{};
    bool       m_IsQuitting{};
};
