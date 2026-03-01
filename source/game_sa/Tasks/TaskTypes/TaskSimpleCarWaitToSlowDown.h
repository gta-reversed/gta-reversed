#pragma once

#include "TaskSimple.h"

class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskSimpleCarWaitToSlowDown : public CTaskSimple {
public:
    enum class SlowDownType {
        SLOW_ENOUGH_TO_STEP_OR_JUMP,
        SLOW_ENOUGH_TO_STEP,
        DONT_WAIT,
    };

    CVehicle*    m_TargetVehicle;
    int32        m_TargetDoor;
    SlowDownType m_SlowType;

public:
    constexpr static auto Type = TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN;

    CTaskSimpleCarWaitToSlowDown(CVehicle* vehicle, SlowDownType type);
    ~CTaskSimpleCarWaitToSlowDown() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask* Clone() const override { return new CTaskSimpleCarWaitToSlowDown(m_TargetVehicle, m_SlowType); }
    bool MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    bool ProcessPed(CPed* ped) override;
    bool SetPedPosition(CPed* ped) override;

    static void InjectHooks();
    CTaskSimpleCarWaitToSlowDown* Constructor(CVehicle* veh, SlowDownType type) { this->CTaskSimpleCarWaitToSlowDown::CTaskSimpleCarWaitToSlowDown(veh, type); return this; }
    CTaskSimpleCarWaitToSlowDown* Destructor() { this->CTaskSimpleCarWaitToSlowDown::~CTaskSimpleCarWaitToSlowDown(); return this; }
};
VALIDATE_SIZE(CTaskSimpleCarWaitToSlowDown, 0x14);
