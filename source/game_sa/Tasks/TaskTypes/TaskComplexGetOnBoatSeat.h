#pragma once

#include "TaskComplex.h"
class CVehicle;

class CTaskComplexGetOnBoatSeat : CTaskComplex {
public:
    CVehicle* m_Vehicle;

public:
    static constexpr auto Type = TASK_COMPLEX_GET_ON_BOAT_SEAT;

    explicit CTaskComplexGetOnBoatSeat(CVehicle* vehicle);
    ~CTaskComplexGetOnBoatSeat() override;

    eTaskType GetTaskType() override { return Type; }
    CTask* Clone() override { return new CTaskComplexGetOnBoatSeat(m_Vehicle); } // 0x64A3B0
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGetOnBoatSeat* Constructor(CVehicle* veh) { this->CTaskComplexGetOnBoatSeat::CTaskComplexGetOnBoatSeat(veh); return this; }
    CTaskComplexGetOnBoatSeat* Destructor() { this->CTaskComplexGetOnBoatSeat::~CTaskComplexGetOnBoatSeat(); return this; }
};
