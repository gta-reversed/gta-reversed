#pragma once

#include "TaskComplex.h"
#include "eTargetDoor.h"
#include "Vehicle.h"

class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAndWander : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AND_WANDER;

    CTaskComplexLeaveCarAndWander(CVehicle* veh, eTargetDoor door = TARGET_DOOR_FRONT_LEFT, int32 leaveDelayMs = 0, bool sensibleLeaveVeh = true);
    CTaskComplexLeaveCarAndWander(const CTaskComplexLeaveCarAndWander&);
    ~CTaskComplexLeaveCarAndWander() override = default;

    CTask* CreateSubTask(eTaskType taskType);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAndWander{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override { return m_pSubTask; }

public:
    static void InjectHooks();

private:
    CVehicle::Ref m_CarToLeave{};       // 0xC
    eTargetDoor   m_DoorToUse{};        // 0x10
    int32         m_LeaveDelayMs{};     // 0x14
    bool          m_SensibleLeaveCar{}; // 0x18
};
