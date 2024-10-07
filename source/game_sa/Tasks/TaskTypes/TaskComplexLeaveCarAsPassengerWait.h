#pragma once

#include "TaskComplex.h"

class CPed;
class CTask;
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexLeaveCarAsPassengerWait : public CTaskComplex {
public:
    CVehicle* m_veh{};

public:
    static void InjectHooks();

    static constexpr auto Type = eTaskType::TASK_COMPLEX_LEAVE_CAR_AS_PASSENGER_WAIT;

    CTaskComplexLeaveCarAsPassengerWait(CVehicle* veh);
    CTaskComplexLeaveCarAsPassengerWait(const CTaskComplexLeaveCarAsPassengerWait&);
    ~CTaskComplexLeaveCarAsPassengerWait();

    CTask* CreateSubTask(eTaskType taskType, CPed* ped);
    CPed* GetNextPassengerToLeave();
    void SetPedInZerothPassengerSeat(CPed* ped);

    CTask*    Clone() const override { return new CTaskComplexLeaveCarAsPassengerWait{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63B430
    CTaskComplexLeaveCarAsPassengerWait* Constructor(CVehicle* veh) {
        this->CTaskComplexLeaveCarAsPassengerWait::CTaskComplexLeaveCarAsPassengerWait(veh);
        return this;
    }

    // 0x63B4A0
    CTaskComplexLeaveCarAsPassengerWait* Destructor() {
        this->CTaskComplexLeaveCarAsPassengerWait::~CTaskComplexLeaveCarAsPassengerWait();
        return this;
    }
};
