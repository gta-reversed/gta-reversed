#pragma once

#include "TaskComplex.h"
class CVehicle;

class NOTSA_EXPORT_VTABLE CTaskComplexGetOnBoatSeat final : public CTaskComplex {
public:
    static constexpr auto Type = TASK_COMPLEX_GET_ON_BOAT_SEAT;

    CTaskComplexGetOnBoatSeat(CVehicle* vehicle);
    CTaskComplexGetOnBoatSeat(const CTaskComplexGetOnBoatSeat&);
    ~CTaskComplexGetOnBoatSeat() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexGetOnBoatSeat(m_Vehicle); } // 0x64A3B0
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, const CEvent* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

protected:
    CVehicle* m_Vehicle;

private:
    friend void InjectHooksMain();
    static void InjectHooks();
    CTaskComplexGetOnBoatSeat* Constructor(CVehicle* veh) { this->CTaskComplexGetOnBoatSeat::CTaskComplexGetOnBoatSeat(veh); return this; }
    CTaskComplexGetOnBoatSeat* Destructor() { this->CTaskComplexGetOnBoatSeat::~CTaskComplexGetOnBoatSeat(); return this; }
};
