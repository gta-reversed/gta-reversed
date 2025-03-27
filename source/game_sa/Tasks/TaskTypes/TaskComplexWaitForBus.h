#pragma once

#include "TaskComplex.h"

class CPed;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexWaitForBus : public CTaskComplex {
public:
    CVehicle* m_bus = {}; // 0xC

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_WAIT_FOR_BUS;

    CTaskComplexWaitForBus() = default;
    CTaskComplexWaitForBus(const CTaskComplexWaitForBus&);
    ~CTaskComplexWaitForBus() = default;

    CTask*    Clone() const override { return new CTaskComplexWaitForBus{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override { return true; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

private: // Wrappers for hooks
    // 0x632900
    CTaskComplexWaitForBus* Constructor() {
        this->CTaskComplexWaitForBus::CTaskComplexWaitForBus();
        return this;
    }

    // 0x632930
    CTaskComplexWaitForBus* Destructor() {
        this->CTaskComplexWaitForBus::~CTaskComplexWaitForBus();
        return this;
    }
};
