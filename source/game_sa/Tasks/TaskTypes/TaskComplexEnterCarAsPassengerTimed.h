#pragma once

#include "TaskTimer.h"
#include "TaskComplex.h"
#include "Vehicle.h"

class CPed;
class CEvent;

class NOTSA_EXPORT_VTABLE CTaskComplexEnterCarAsPassengerTimed final : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED;

    static void InjectHooks();

    CTaskComplexEnterCarAsPassengerTimed(CVehicle* vehicle, uint32 targetSeat, uint32 time, bool bCarryOnAfterFallingOff);
    CTaskComplexEnterCarAsPassengerTimed(const CTaskComplexEnterCarAsPassengerTimed&);
    ~CTaskComplexEnterCarAsPassengerTimed() override = default;

    void      StopTimer(const CEvent* event) override;
    CTask*    Clone() const override { return new CTaskComplexEnterCarAsPassengerTimed{ *this }; }
    eTaskType GetTaskType() const override { return Type; }
    bool      MakeAbortable(CPed* ped, eAbortPriority priority = ABORT_PRIORITY_URGENT, CEvent const* event = nullptr) override;
    CTask*    CreateNextSubTask(CPed* ped) override { return nullptr; }
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;

private: // Wrappers for hooks
    // 0x63B030
    CTaskComplexEnterCarAsPassengerTimed* Constructor(CVehicle* vehicle, uint32 targetSeat, uint32 time, bool bCarryOnAfterFallingOff) {
        this->CTaskComplexEnterCarAsPassengerTimed::CTaskComplexEnterCarAsPassengerTimed(vehicle, targetSeat, time, bCarryOnAfterFallingOff);
        return this;
    }

    // 0x63B0C0
    CTaskComplexEnterCarAsPassengerTimed* Destructor() {
        this->CTaskComplexEnterCarAsPassengerTimed::~CTaskComplexEnterCarAsPassengerTimed();
        return this;
    }

private:
    CVehicle::Ref m_TargetCar{};
    uint32        m_Time{};
    uint32        m_TargetSeat{};
    bool          m_bCarryOnAfterFallingOff{};
    eMoveState    m_MoveState{eMoveState::PEDMOVE_RUN};
    CTaskTimer    m_Timer{};
};
