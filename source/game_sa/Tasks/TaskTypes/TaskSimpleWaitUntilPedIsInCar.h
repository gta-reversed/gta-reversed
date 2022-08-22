#pragma once

#include "TaskSimple.h"

class CEvent;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskSimpleWaitUntilPedIsInCar : public CTaskSimple {
public:
    CPed* m_pedToWaitFor{};
    bool  m_bIsWalkingToDoor{};

public:
    static void InjectHooks();

    constexpr static auto Type = eTaskType::TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR;

    CTaskSimpleWaitUntilPedIsInCar(CPed* pedToWaitFor);
    ~CTaskSimpleWaitUntilPedIsInCar();

    CTask*    Clone() override { return new CTaskSimpleWaitUntilPedIsInCar{ *this }; }
    eTaskType GetTaskType() override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) override { return true; }
    bool      ProcessPed(CPed* ped) override;

private: // Wrappers for hooks
    // 0x649620
    CTaskSimpleWaitUntilPedIsInCar* Constructor(CPed* pedToWaitFor) {
        this->CTaskSimpleWaitUntilPedIsInCar::CTaskSimpleWaitUntilPedIsInCar(pedToWaitFor);
        return this;
    }
    // 0x6496A0
    CTaskSimpleWaitUntilPedIsInCar* Destructor() {
        this->CTaskSimpleWaitUntilPedIsInCar::~CTaskSimpleWaitUntilPedIsInCar();
        return this;
    }
};
